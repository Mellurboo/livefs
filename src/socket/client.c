#include <utils/terminal.h>
#include <socket/socket.h>
#include <vendor/gt/gt.h>
#include <filesystem/send.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/// @brief Handles the client request and closing it when appropriate
/// @param fd_void file descriptor
void client_handler(void* fd_void) {
    int client_socket = (uintptr_t)fd_void;
    char request[BUFFER_SIZE];
    gtblockfd(client_socket, GTBLOCKIN);
    int n = recv(client_socket, request, BUFFER_SIZE - 1, 0);
    if (n > 0) {
        request[n] = '\0';
        char *first_line = strtok(request, "\r\n");
        if (first_line) {
            printf(REQUEST "First Line = '%s'\n", first_line);
            gtblockfd(client_socket, GTBLOCKOUT);
            send_file(client_socket, first_line);
        }
    }
    close(client_socket);
}

/// @brief starts listening for client connections
/// @param server_socket server socket 
/// @return success
int client_listener(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    while (1) {
        gtblockfd(server_socket, GTBLOCKIN);
        int client_socket = accept(server_socket,
                                   (struct sockaddr*)&client_addr,
                                   &addr_len);
        
        if (client_socket == -1) {
            perror("accept");
            fprintf(stderr, ERROR "Failure to accept Client Connection\n");
            continue;
        }
        gtgo(client_handler, (void*)(uintptr_t)client_socket);
    }
    return 0;
}
