#pragma once

#include <sys/types.h>
#include <stdio.h>

// content type structure
typedef struct {
    const char *extention;
    size_t extention_length;
    const char *content_type;
} content_type_t;

/// @brief uses the file name to serve the correct content type
/// @param filename target file
/// @return HTTP content type
const char *get_content_type(const char *filename);

/// @return HTTP bad request header
void http_bad_request_header(int client_socket);

/// @return HTTP 403 not found header
void http_forbidden(int client_socket);

/// @return HTTP 500 internal server error
void http_internal_server_error(int client_socket);

/// @return HTTP 405 method not allowed header
void http_method_not_allowed(int client_socket);

/// @return HTTP 404 not found header
void http_not_found_header(int client_socket);

/// @return HTTP 418 :)
void http_im_a_teapot(int client_socket);

/// @return Place HTTP 200 in Buffer Provided
const char *http_success(const char *fname, off_t content_size, int force_download);