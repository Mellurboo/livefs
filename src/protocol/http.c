#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <sys/socket.h>

const char *get_content_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".ico") == 0) return "image/x-icon";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    return "application/octet-stream"; // default
}

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

/// @return HTTP 418 I'm a teapot
const char *http_im_a_teapot(){
    const char *im_a_teapot =
            "HTTP/1.1 418 I'm a teapot\r\n"
            "Content-Length: 11\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "I'm a teapot";
    return im_a_teapot;
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