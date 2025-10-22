#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <utils/terminal.h>
#include <socket/socket.h>

struct sockaddr_in server_addr;

/// @brief returns the server socket address
/// @return server socket address
struct sockaddr_in get_server_sockaddr(void){
    return server_addr;
}

/// @brief Creates the Server Socket (socket() wrapper)
/// @return 'int' server socket
int server_create_socket(void) {
    int opt = 1;
    int server_socket;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, FATAL "Error creating socket: %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        fprintf(stderr, FATAL "Could not set SO_REUSEADDR\n");
        close(server_socket);
        exit(-1); // fail immediately
    }

    return server_socket;
}

/// @brief Binds the socket to a port and address, address is set to INADDR_ANY. handle cleanup if failed
/// @param port target port to bind to
void server_bind_socket(int port, int server_socket){
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("bind");
        fprintf(stderr, FATAL "Error in Binding Socket, Socket Bind Return -1 %s:%d\n", __FILE__, __LINE__);
    }
}

/// @brief starts the server listener
void server_listen(int server_socket){
    if (listen(server_socket, 5) == -1){
        fprintf(stderr, FATAL "Error in Starting Listener, Listen Returned -1 %s:%d\n", __FILE__, __LINE__);
        close(server_socket);
    }else{
        printf(SUCCESS "Listening on port %d\n", ntohs(server_addr.sin_port));
    }
}
