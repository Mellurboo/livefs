#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>
#include <utils/path.h>
#include <utils/time.h>
#include <sys/socket.h>
#include <linux/limits.h>
#include <protocol/http.h>
#include <utils/terminal.h>
#include <filesystem/send.h>
#include <filesystem/filepath.h>
#include <filesystem/read_file.h>
#include <filesystem/cache/filecache.h>
#include <socket/request_arguement.h>
#include <config/descriptor/descriptor_file.h>
#include <config/global_config/global_config_file.h>

#define BUFFER_SIZE 1024

request_t *parse_request_structure(int client_socket, const char *request_line){
    request_t *request = calloc(1, sizeof(request_t));
    if (!request){
        printf(FATAL "Error allocating memory for request structure\n");
        exit(-1);
    }

    // Parse the first line: method, path, version
    if (sscanf(request_line, "%7s %255s %15s", request->method, request->path, request->version) != 3) {
        http_bad_request_header(client_socket);
        return NULL;
    }

    return request;
}

int validate_request(int client_socket, request_t *request){
    // only allow get
    if (strcmp(request->method, "GET") != 0) {
        http_method_not_allowed(client_socket);
        printf(BADRESPONSE "Method Disallowed\n");
        return 0;
    }

    // Block unsafe paths: '..' or '~'
    if (strstr(request->path, "..") || strchr(request->path, '~')) {
        printf(BADRESPONSE "Path Forbidden\n");
        http_forbidden(client_socket);
        return 0;
    }
    return 1;
}

/// @brief attempts to read the descriptor file and check if the folder is hidden
/// @param client_socket client socket
/// @param full_path full provided path
/// @return descriptor file pointer
descriptor_t *get_descriptor_file(int client_socket, const char *full_path){
    descriptor_t *descriptor_file = read_descriptor_file(full_path);
    return descriptor_file;
}

/// @brief dont send the entire file at once, buffer it
/// @param client_socket client socket
/// @param buffer 1kib buffer
/// @param fp file pointer
void send_buffered_bytes(int client_socket, SSL *ssl, const char *data, size_t size){
    char databuffer[BUFFER_SIZE];
    size_t sent = 0;

    while (sent < size){
        size_t chunk = (size - sent > BUFFER_SIZE) ? BUFFER_SIZE : (size - sent);
        memcpy(databuffer, data + sent, chunk);

        ssize_t w = send_data(client_socket, ssl, databuffer, chunk);
        if (w <= 0){
            fprintf(stderr, ERROR "Error sending data\n");
            break;
        }

        sent += w;
    }
}

ssize_t send_data(int client_socket, SSL *ssl, const void *data, size_t size){
    if (ssl){
        size_t w = SSL_write(ssl, data, size);
        if (w <= 0){
            int err = SSL_get_error(ssl, w);
            fprintf(stderr, ERROR "SSL Error: %i", err);
            return -1;
        }
        return w;
    }else{
        return send(client_socket, data, size, 0);
    }
}

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param request_line full HTTP request
void send_file_request(int client_socket, SSL *ssl, const char *request_line){

    request_t *request = parse_request_structure(client_socket, request_line);
    if (!request){
        fprintf(stderr, ERROR "Failed to parse request '%s'\n", request_line);
        return;
    }

    if (!validate_request(client_socket, request)){
        free(request);
        return;
    }

    // Strip query arguments and normalize path
    char query_stripped_path[PATH_MAX];
    strncpy(query_stripped_path, strip_arguemnts(request->path), sizeof(query_stripped_path)-1);
    query_stripped_path[sizeof(query_stripped_path)-1] = '\0';

    // Remove trailing slash if not root
    size_t path_len = strlen(query_stripped_path);
    if (path_len > 1 && query_stripped_path[path_len - 1] == '/')
        query_stripped_path[path_len - 1] = '\0';

    // Build the full file path to serve
    char file_path[PATH_MAX];
    strncpy(file_path, build_file_path(query_stripped_path), sizeof(file_path)-1);
    file_path[sizeof(file_path)-1] = '\0';

    printf(INFO "Opening File: '%s'\n", file_path);

    // Only redirect for existing directories
    if (path_exists(file_path) && is_directory(file_path) && query_stripped_path[strlen(query_stripped_path) - 1] != '/') {
        http_redirect(client_socket, request);
        free(request);
        return;
    }

    // Get descriptors
    descriptor_t *descriptor = get_descriptor_file(client_socket, file_path);
    if (!descriptor || descriptor->hidden == 1){
        http_not_found_header(client_socket);
        free(request);
        return;
    }

    if (!path_exists(file_path)){
        fprintf(stderr, BADRESPONSE "Path '%s' doesn't exist or failed to read\n", file_path);
        http_not_found_header(client_socket);
        free(request);
        free(descriptor);
        return;
    }

    // Prepare file data
    size_t filesize = 0;
    const char *filedata = NULL;
    char filename[PATH_MAX];

    if (!is_directory(file_path)){
        strncpy(filename, file_path, sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
        filedata = cache_get_file(&file_data_cache, file_path, &filesize);
        if (!filedata){
            fprintf(stderr, ERROR "Failed to read file '%s'\n", file_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }
    } else {
        // Serve page from descriptor
        char webpage_path[PATH_MAX];
        int w = snprintf(webpage_path, sizeof(webpage_path), "%s%s", file_path, descriptor->page);
        if (w < 0 || w >= (int)sizeof(webpage_path)){
            fprintf(stderr, ERROR "Filepath too long joining '%s' and '%s'\n", file_path, descriptor->page);
            http_internal_server_error(client_socket);
            free(request);
            free(descriptor);
            return;
        }
        
        // Prep filename so it will serve the directory file instead
        strncpy(filename, webpage_path, sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
        filedata = cache_get_file(&file_data_cache, webpage_path, &filesize);
        if (!filedata){
            fprintf(stderr, BADRESPONSE "Failed to read webpage '%s'\n", webpage_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }
    }

    // Buffer comes from the file_name that got stripped before
    // so we don't include our arguments like ?download in the filename
    const char *header_filename = basename(filename);

    const char *success_header = http_success(
        header_filename,
        filesize,
        request_has_arguement(request->path, "download")
    );

    // Send header and file
    send_data(client_socket, ssl, success_header, strlen(success_header));
    send_buffered_bytes(client_socket, ssl, filedata, filesize);

    printf(SUCREQUEST "Served file '%s' to Client\n", file_path);

    free(request);
    free(descriptor);
}