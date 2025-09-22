#include <stdio.h>
#include <inttypes.h>
#include <sys/socket.h>

/// @return ERROR 400: HTTP bad request header
const char *http_bad_request_header(){
    const char *bad_request_header =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 11\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Bad Request";
    return bad_request_header;
}

/// @return HTTP 403 not found header
const char *http_forbidden(){
    const char *forbidden =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 9\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Forbidden";
    return forbidden;
}

/// @return HTTP 405 method not allowed header
const char *http_method_not_allowed(){
    const char *method_not_allowed =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 18\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Method Not Allowed";
    return method_not_allowed;
}

/// @return HTTP 404 not found header
const char *http_not_found_header(){
    const char *not_found_header =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found";
    return not_found_header;
}

/// @return HTTP 200 not found header
const char *http_success(char *fname, off_t content_size) {
    static char success_header[512];
    snprintf(success_header, sizeof(success_header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Disposition: attachment; filename=\"%s\"\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        fname, content_size);
    return success_header;
}