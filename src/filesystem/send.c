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
#include <utils/path.h>
#include <filesystem/read_file.h>
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

descriptor_t *interpret_descriptor(int client_socket, const char *full_path){
    descriptor_t *descriptor_file = read_descriptor_file(full_path);

    if (!descriptor_file || descriptor_file->hidden == 1){
        http_not_found_header(client_socket);
        return NULL;
    }

    return descriptor_file;
}

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param request_line full HTTP request
void send_file_request(int client_socket, const char *request_line){
    request_t *request = parse_request_structure(client_socket, request_line);
    if (!request){
        printf(ERROR "Failed to parse request_line='%s'\n", request_line);
        return;
    }

    if (!validate_request(client_socket, request)){
        free(request);
        return;
    }

    char file_path[PATH_MAX];
    strncpy(file_path, build_file_path(request->path), sizeof(file_path) - 1);
    file_path[sizeof(file_path) - 1] = '\0';
    printf(INFO "Opening file: %s\n", file_path);

    descriptor_t *descriptor = interpret_descriptor(client_socket, file_path);
    if (!path_exists(file_path)){
        fprintf(stderr, BADRESPONSE "Couldn't stat path %s, It probably doesn't exsist.", file_path);
        http_not_found_header(client_socket);
        free(request);
        free(descriptor);
        return;
    }

    char *filename = basename(file_path);
    char buffer[BUFFER_SIZE];

    if (is_directory(file_path) == 0){
        FILE *fp = open_file(file_path);
        if (!fp){
            fprintf(stderr, BADRESPONSE "Failure to open file %s\n", file_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }

        struct stat fileinfo;
        stat(file_path, &fileinfo);

        const char *success_header =
            http_success(filename, fileinfo.st_size, request_has_arguement((request->path), "download"));
        send(client_socket, success_header, strlen(success_header), 0);

        size_t fbytes;
        while ((fbytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0){
            send(client_socket, buffer, fbytes, 0);
        }

        printf(SUCREQUEST "Sent file %s to Client\n", file_path);
        fclose(fp);
    }else{
        if (!descriptor || !descriptor->page){
            fprintf(stderr, ERROR "Failed to open descriptor page for %s\n", file_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }
        
        char webpage_path[PATH_MAX];
        int written = snprintf(webpage_path, sizeof(webpage_path), "%s/%s", file_path, descriptor->page);
        if (written < 0 || written >= (int)sizeof(webpage_path)){
            fprintf(stderr, ERROR "Filepath was too long when joining %s and %s", file_path, descriptor->page);
            http_internal_server_error(client_socket);
            free(request);
            free(descriptor);
            return;
        }

        struct stat fileinfo;
        if (stat(webpage_path, &fileinfo) < 0) {
            perror("stat failed");
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }
        
        char *serve_filename = basename(webpage_path);
        const char *success_header = http_success(serve_filename, fileinfo.st_size, request_has_arguement(request->path, "download"));
        send(client_socket, success_header, strlen(success_header), 0);

        FILE *fp = open_file(webpage_path);
        if (!fp){
            perror("open_file failed");
            fprintf(stderr, BADRESPONSE "Failure to open file for directory page %s\n", webpage_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }

        size_t fbytes;
        while ((fbytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0){
            send(client_socket, buffer, fbytes, 0);
        }

        printf(SUCREQUEST "Sent Descriptor page %s to client\n", file_path);
        fclose(fp);
    }

    free(request);
    free(descriptor);
    return;
}
