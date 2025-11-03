#include <openssl/ssl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE     1024

int is_secured_connection(int client_socket){
    unsigned char tls_signature[1];
    int byte = recv(client_socket, tls_signature, 1, MSG_PEEK);
    if (byte <= 0) return 0;
    return tls_signature[0] == 0x16;
}