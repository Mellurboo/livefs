#pragma once
#include <socket/socket.h>
#include <config/descriptor/descriptor_file.h> 
#include <stdio.h>

typedef struct request{
    char method[8];
    char path[255];
    char version[16];
}request_t;

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param filename filename location
void send_file_request(int client_sock, const char *filename);

descriptor_t *interpret_descriptor(int client_socket, const char *full_path);

FILE *open_file(int client_socket, const char *filepath);