#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <utils/time.h>
#include <sys/socket.h>
#include <linux/limits.h>
#include <protocol/http.h>
#include <utils/terminal.h>
#include <filesystem/send.h>
#include <filesystem/filepath.h>
#include <socket/request_arguement.h>
#include <config/descriptor/descriptor_file.h>

#define BUFFER_SIZE 1024

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param request_line full HTTP request
void send_file(int client_sock, const char *request_line) {
    char method[8] = {0};
    char path[256] = {0};
    char version[16] = {0};

    // Parse the first line: method, path, version
    if (sscanf(request_line, "%7s %255s %15s", method, path, version) != 3) {
        printf(INFO "Failed to parse request_line='%s'\n", request_line);
        const char *bad_request = http_bad_request_header();
        send(client_sock, bad_request, strlen(bad_request), 0);
        close(client_sock);
        return;
    }

    printf(REQUEST "method='%s', path='%s', version='%s'\n", method, path, version);

    if (strcmp(method, "GET") != 0) {
        const char *not_allowed = http_method_not_allowed();
        send(client_sock, not_allowed, strlen(not_allowed), 0);
        close(client_sock);
        return;
    }

    // Block unsafe paths: '..' or '~'
    if (strstr(path, "..") || strchr(path, '~')) {
        const char *forbidden = http_forbidden();
        send(client_sock, forbidden, strlen(forbidden), 0);
        close(client_sock);
        return;
    }

    char full_path[PATH_MAX];
    strncpy(full_path, build_file_path(path), sizeof(full_path) - 1);
    full_path[sizeof(full_path) - 1] = '\0';

    printf(REQUEST "opening file '%s'\n", full_path);

    char *descriptor_file = read_descriptor_file(full_path);

    if (!descriptor_file){
        const char *not_found = http_not_found_header();
        send(client_sock, not_found, strlen(not_found), 0);
        close(client_sock);
        return;
    }
    free(descriptor_file);

    FILE *fp = fopen(full_path, "rb");
    if (!fp) {
        printf(REQUEST "Failure to open file %s it probably doesnt exsist\n", full_path);
        const char *not_found = http_not_found_header();
        send(client_sock, not_found, strlen(not_found), 0);
        close(client_sock);
        return;
    }

    // We need to know the file size and the location of the file
    // for now im going to use the base location of the server exec
    // but later we will allow the user to define this
    struct stat fileinfo;
    stat(full_path, &fileinfo);
    char *fname = basename(full_path);

    const char *success_header = http_success(fname, fileinfo.st_size, request_has_arguement(path, "download"));
    send(client_sock, success_header, strlen(success_header), 0);

    // Send file to the client, this is unencrypted and insecure
    char buffer[BUFFER_SIZE];
    size_t fbytes;
    while ((fbytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(client_sock, buffer, fbytes, 0);
    }

    // clean up the connection, we can close it now!
    fclose(fp);
    close(client_sock);
}