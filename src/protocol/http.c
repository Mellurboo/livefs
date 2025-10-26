#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <protocol/http.h>
#include <inttypes.h>

/// @return ERROR 400: HTTP bad request header
void http_bad_request_header(int client_socket){
    const char *bad_request_header =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 11\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Bad Request";
    send(client_socket, bad_request_header, strlen(bad_request_header), 0);
    close(client_socket);
    return;
}

/// @return HTTP 403 not found header
void http_forbidden(int client_socket){
    const char *forbidden =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 9\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Forbidden";
    send(client_socket, forbidden, strlen(forbidden), 0);
    close(client_socket);
    return;
}

/// @return HTTP 500 not found header
void http_internal_server_error(int client_socket){
    const char *internal_server_error =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Length: 21\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Internal Server Error";
    send(client_socket, internal_server_error, strlen(internal_server_error), 0);
    close(client_socket);
    return;
}

/// @return HTTP 405 method not allowed header
void http_method_not_allowed(int client_socket){
    const char *method_not_allowed =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 18\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Method Not Allowed";
    send(client_socket, method_not_allowed, strlen(method_not_allowed), 0);
    close(client_socket);
    return;
}

/// @return HTTP 404 not found header
void http_not_found_header(int client_socket){
    const char *not_found_header =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found";
    send(client_socket, not_found_header, strlen(not_found_header), 0);
    close(client_socket);
    return;
}

/// @return HTTP 418 I'm a teapot
void http_im_a_teapot(int client_socket){
    const char *im_a_teapot =
            "HTTP/1.1 418 I'm a teapot\r\n"
            "Content-Length: 11\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "I'm a teapot";
    send(client_socket, im_a_teapot, strlen(im_a_teapot), 0);
    close(client_socket);
    return;
}

/// @return HTTP 200 not found header
const char *http_success(const char *fname, off_t content_size, int force_download) {
    static char success_header[512];
    const char *content_type = get_content_type(fname);
    if (force_download) content_type = "application/octet-stream";

    snprintf(success_header, sizeof(success_header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Connection: close\r\n"
        "Content-Length: %" PRId64 "\r\n"
        "\r\n",
        content_type,
        (int64_t)content_size);
    return success_header;
}