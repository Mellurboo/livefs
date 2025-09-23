#include <socket/socket.h>

struct sockaddr_in server_addr;

/// @brief returns the server socket address
/// @return server socket address
struct sockaddr_in get_server_sockaddr(){
    return server_addr;
}

/// @brief Creates the Server Socket (socket() wrapper)
/// @return 'int' server socket
int server_create_socket(int server_socket){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_socket == -1){
        fprintf(stderr, FATAL "Error in Creating Socket, Socket Return -1 %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }else{
        return server_socket;
    }
}

/// @brief Binds the socket to a port and address, address is set to INADDR_ANY. handle cleanup if failed
/// @param port target port to bind to
void server_bind_socket(int port, int server_socket){
    if (port == -1) {
        port = FALLBACK_SERVER_PORT;
        printf(WARN "Failure in getting server port, defaulting to %d", FALLBACK_SERVER_PORT);
    } 

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