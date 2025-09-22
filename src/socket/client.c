#include <socket/socket.h>
#include <utils/terminal.h>
#include <fs/send.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/// @brief starts listening for client connections
/// @param server_socket server socket 
/// @return success
int client_listener(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    while (1) {
        int client_socket = accept(server_socket,
                                   (struct sockaddr*)&client_addr,
                                   &addr_len);
        
        if (client_socket == -1) {
            perror("accept");
            fprintf(stderr, "%s Failure to accept Client Connection\n", ERROR);
            continue;
        }

        char request[BUFFER_SIZE];
        int n = recv(client_socket, request, BUFFER_SIZE - 1, 0);
        if (n > 0) {
            request[n] = '\0';

            char *first_line = strtok(request, "\r\n");
            if (first_line) {
                printf("%s first_line='%s'\n", INFO, first_line);
                send_file(client_socket, first_line);
            }
        }

        close(client_socket);
    }
    return 0;
}
