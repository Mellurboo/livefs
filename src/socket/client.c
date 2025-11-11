#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <utils/tls.h>
#include <utils/time.h>
#include <openssl/err.h>
#include <vendor/gt/gt.h>
#include <socket/socket.h>
#include <protocol/http.h>
#include <utils/terminal.h>
#include <filesystem/send.h>
#include <socket/async/async.h>


#define BUFFER_SIZE     1024

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}


/// @brief Handles the client request and closing it when appropriate
/// @param fd_void file descriptor
void client_handler(void* fd_void) {
    uint64_t start = now_ns();
    printf("\n%sRequest Start %s", INFO, get_current_server_time());
    
    global_config_t *global_config = get_global_config_structure();
    char request[BUFFER_SIZE] = {0};
    int client_socket = (uintptr_t)fd_void;
    SSL *ssl = NULL;

    int requestread = 0;

    if (global_config->enable_ssl && is_secured_connection(client_socket)){
        // Create SSL object for this client
        ssl = SSL_new(ssl_ctx);
        if (!ssl) {
            ERR_print_errors_fp(stderr);
            close(client_socket);
            return;
        }

        SSL_set_fd(ssl, client_socket);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(client_socket);
            return;
        }

        SSL_CTX_set_session_cache_mode(ssl_ctx, SSL_SESS_CACHE_SERVER);

        const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl);
        printf(SUCCESS "Cipher: '%s'\n", SSL_CIPHER_get_name(cipher));
        requestread = ssl_async_read(ssl, request, BUFFER_SIZE - 1);

    }else if (global_config->allow_insecure_connections){
        gtblockfd(client_socket, GTBLOCKIN);
        requestread = async_recv(client_socket, request, BUFFER_SIZE - 1, 0);
    }else{
        http_forbidden(client_socket);
    }

    if (requestread > 0) {
        request[requestread] = '\0';
        char *first_line = strtok(request, "\r\n");
        if (first_line) {
            printf(REQUEST "First Line = '%s'\n", first_line);
            gtblockfd(client_socket, GTBLOCKOUT);
            send_file_request(client_socket, ssl, first_line);
        }
    }
    
    if (is_secured_connection(client_socket)){
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    uint64_t end = now_ns();
    uint64_t delta_ns = end - start;
    double delta_ms = delta_ns / 1e6;
    printf("Request Took %.3fms", delta_ms);

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
