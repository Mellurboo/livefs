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
void send_buffered_bytes(int client_socket, char *buffer, FILE *fp){
    size_t fbytes;
    while ((fbytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0){
        send(client_socket, buffer, fbytes, 0);
    }
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

    // this only checks Request Type and request path, not data validation
    if (!validate_request(client_socket, request)){
        free(request);
        return;
    }

    // Build the full filepath
    char file_path[PATH_MAX];
    strncpy(file_path, build_file_path(request->path), sizeof(file_path) - 1);
    file_path[sizeof(file_path) - 1] = '\0';
    printf(INFO "Opening file: %s\n", file_path);

    // redirect if we dont have the following / at the end
    if (is_directory(file_path)){
        if (request->path[strlen(request->path) - 1] != '/'){
            http_redirect(client_socket, request);
        }
    }

    // get and check descriptor file, check its hidden value now so we dont send any data
    descriptor_t *descriptor = get_descriptor_file(client_socket, file_path);
    if (!descriptor || descriptor->hidden == 1){
        http_not_found_header(client_socket);
        return;
    }

    // if the file doesnt exsist handle it here.
    if (!path_exists(file_path)){
        fprintf(stderr, BADRESPONSE "Couldn't stat path %s, It probably doesn't exsist.", file_path);
        http_not_found_header(client_socket);
        free(request);
        free(descriptor);
        return;
    }

    char buffer[BUFFER_SIZE];

    /*
        Handle Files and Directories Differently
        Files:
            - Respond 200
            - Allocate file to Memory
            - Send raw bytes from Memory
        Directories:
            - Respond 200
            - Use the directory descriptor to locate the webpage file
            - send the html file with the html content type

        Files get sent and downloaded, directories send a html file
    */
    if (is_directory(file_path) == 0){
        FILE *fp = open_file(file_path);
        if (!fp){
            fprintf(stderr, BADRESPONSE "Failure to open file %s\n", file_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }
        
        char *filename = basename(file_path);       // this is ONLY so we know what filename to serve to the client.

        // get the filesizeand then return the correct header.
        ssize_t filesize = get_filesize(fp);
        if (filesize == -1) return;
        const char *success_header =
            http_success(filename, filesize, request_has_arguement((request->path), "download"));
        send(client_socket, success_header, strlen(success_header), 0);

        send_buffered_bytes(client_socket, buffer, fp);

        printf(SUCREQUEST "Sent file %s to Client\n", file_path);
        fclose(fp);
    }else{
        // build the webpage path
        char webpage_path[PATH_MAX];
        int written = snprintf(webpage_path, sizeof(webpage_path), "%s%s", file_path, descriptor->page);
        if (written < 0 || written >= (int)sizeof(webpage_path)){
            fprintf(stderr, ERROR "Filepath was too long when joining %s and %s", file_path, descriptor->page);
            http_internal_server_error(client_socket);
            free(request);
            free(descriptor);
            return;
        }
        
        FILE *fp = open_file(webpage_path);
        if (!fp){
            fprintf(stderr, BADRESPONSE "Failure to open file for directory page %s\n", webpage_path);
            http_not_found_header(client_socket);
            free(request);
            free(descriptor);
            return;
        }

        ssize_t filesize = get_filesize(fp);
        if (filesize == -1) return;
        
        // filename to serve to client
        char *filename = basename(webpage_path);
        const char *success_header = http_success(filename, filesize, request_has_arguement(request->path, "download"));
        send(client_socket, success_header, strlen(success_header), 0);

        send_buffered_bytes(client_socket, buffer, fp);

        printf(SUCREQUEST "Sent Descriptor page %s to client\n", file_path);
        fclose(fp);
    }

    free(request);
    free(descriptor);
    return;
}