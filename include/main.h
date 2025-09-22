#pragma once
// Port the server will listen on by defualt
#define SERVER_LISTEN_PORT  8080
#define BUFFER_SIZE         1024

/// @brief create and bind the server socket
/// @param server_socket server socket target
/// @return return server socket
int init_server();
