#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <socket/socket.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <openssl/err.h>
#include <config/global_config/global_config_file.h>

struct sockaddr_in server_addr;
SSL_CTX *ssl_ctx = NULL;

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

    int flags = fcntl(server_socket, F_GETFL, 0);
    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        fprintf(stderr, FATAL "Could not set SO_REUSEADDR\n");
        close(server_socket);
        exit(-1); // fail immediately
    }

    return server_socket;
}

/// @brief initialise SSL
/// @param config_file config file context
/// @return int success
int initSSL(global_config_t *global_config, int server_socket){

    char certificate_path[PATH_MAX];
    char key_path[PATH_MAX];

    ssl_ctx = SSL_CTX_new(TLS_server_method());
    if (!ssl_ctx){
        ERR_print_errors_fp(stderr);
        return 0;
    }

    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL); // this is normal

    strncpy(key_path, global_config->ssl_key_path, sizeof(key_path)-1);
    key_path[sizeof(key_path)-1] = 0;
    key_path[strcspn(key_path, "\r\n")] = 0;

    strncpy(certificate_path, global_config->ssl_cert_path, sizeof(certificate_path)-1);
    certificate_path[sizeof(certificate_path)-1] = 0;
    certificate_path[strcspn(certificate_path, "\r\n")] = 0;

    if (SSL_CTX_use_certificate_file(ssl_ctx, certificate_path, SSL_FILETYPE_PEM) <= 0 ||
    SSL_CTX_use_PrivateKey_file(ssl_ctx, key_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return 0;
    }

    printf(SUCCESS "SSL Enabled with certificate '%s'\n", certificate_path);
    return 1;
}

/// @brief Binds the socket to a port and address, address is set to INADDR_ANY. handle cleanup if failed
/// @param port target port to bind to
void server_bind_socket(int port, int server_socket){
    memset(&server_addr, 0, sizeof(server_addr));

    if (port == -1) {
        fprintf(stderr, FATAL "Invalid or missing port value this error is fatal\n");
        exit(-1);
    }

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
