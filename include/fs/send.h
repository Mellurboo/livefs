#pragma once
#include <socket/socket.h>

/// @brief serve the client with the file
/// @param client_sock client socket
/// @param filename filename location
void send_file(int client_sock, const char *filename);