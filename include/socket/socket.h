#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>

/// @brief returns the server socket address
/// @return server socket address
struct sockaddr_in get_server_sockaddr(void);

/// @brief Creates the Server Socket (socket() wrapper)
/// @return 'int' server socket
int server_create_socket(void);

/// @brief Binds the socket to a port and address, address is set to INADDR_ANY. handle cleanup if failed
/// @param port target port to bind to
void server_bind_socket(int port, int server_socket);

/// @brief starts the server listener
void server_listen(int server_socket);

/// @brief starts listening for client connections
/// @param client_socket client socket
/// @param server_socket server socket
/// @return success
int client_listener(int server_socket);
