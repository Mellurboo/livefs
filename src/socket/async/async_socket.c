#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <vendor/gt/gt.h>
#include <socket/socket.h>

/// @brief non-blocking send wrapper
/// @param client_socket client socket
/// @param data data in buffer
/// @param data_size data size
/// @param flags flags
/// @return return data sent size
ssize_t async_send(int client_socket, const void *data, size_t data_size, int flags){
    size_t sent_data = 0;
    while(sent_data < data_size){
        ssize_t n = send(client_socket, (const char *)data + sent_data, data_size - sent_data, flags);

        // yeild if were not ready
        if (n > 0){
            sent_data += n;
        }else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)){
            gtblockfd(client_socket, GTBLOCKOUT);
        }else{
            return -1;
        }
    }
    return sent_data;
}

/// @brief non-blocking data recieve wrapper
/// @param client_socket client socket
/// @param read_buf read in buffer
/// @param read_size read size
/// @param flags flags
/// @return success / error
ssize_t async_recv(int client_socket, void *read_buf, size_t read_size, int flags){
    while(1){
        ssize_t n = recv(client_socket, read_buf, read_size, flags);
        if (n > 0) return n;
        if (n == 0) return 0; // eof
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            gtblockfd(client_socket, GTBLOCKIN);
            continue;
        }

        return -1;
    }
}

/// @brief non-blocking SSL Aware send wrapper
/// @param client_socket client socket
/// @param data data in buffer
/// @param data_size data size
/// @param flags flags
/// @return return data sent size
ssize_t ssl_async_write(SSL *ssl, const void *data, size_t data_size){
    size_t sent_data = 0;
    while (sent_data > data_size){
        int w = SSL_write(ssl, (const char*)data + sent_data, data_size - sent_data);

        if (w > 0){
            sent_data += w;
            continue;
        }

        int error = SSL_get_error(ssl, w);
        if (error == SSL_ERROR_WANT_READ){
            gtblockfd(SSL_get_fd(ssl), GTBLOCKIN);
        }else if (error == SSL_ERROR_WANT_WRITE){
            gtblockfd(SSL_get_fd(ssl), GTBLOCKOUT);
        }else{
            return -1;
        }
    }

    return sent_data;
}

/// @brief non-blocking SSL Aware data recieve wrapper
/// @param client_socket client socket
/// @param read_buf read in buffer
/// @param read_size read size
/// @param flags flags
/// @return success / error
ssize_t ssl_async_read(SSL *ssl, void *read_buf, size_t read_size){
    while (1){
        int n = SSL_read(ssl, read_buf, read_size);
        if (n > 0) return n;

        int error = SSL_get_error(ssl, n);
        if (error == SSL_ERROR_WANT_READ){
            gtblockfd(SSL_get_fd(ssl), GTBLOCKIN);
            continue;
        }else if(error == SSL_ERROR_WANT_WRITE){
            gtblockfd(SSL_get_fd(ssl), GTBLOCKOUT);
            continue;
        }else{
            return -1;
        }
    }
}