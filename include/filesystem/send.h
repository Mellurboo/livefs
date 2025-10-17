#pragma once
#include <socket/socket.h>

typedef struct request{
    char method[8];
    char path[255];
    char version[16];
}request_t;

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param filename filename location
void send_file(int client_sock, const char *filename);
