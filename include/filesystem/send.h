#pragma once
#include <socket/socket.h>
#include <config/descriptor/descriptor_file.h> 
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>

typedef struct request{
    char method[8];
    char path[255];
    char version[16];
}request_t;

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param filename filename location
void send_file_request(int client_sock, SSL *ssl, const char *filename);

ssize_t send_data(int client_socket, SSL *ssl, const void *data, size_t size);

descriptor_t *get_descriptor_file(int client_socket, const char *full_path);