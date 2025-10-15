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
const char *http_bad_request_header();

/// @return HTTP 403 not found header
const char *http_forbidden();

/// @return HTTP 405 method not allowed header
const char *http_method_not_allowed();

/// @return HTTP 404 not found header
const char *http_not_found_header();

/// @return HTTP 418 :)
const char *http_im_a_teapot();

/// @return Place HTTP 200 in Buffer Provided
const char *http_success(const char *fname, off_t content_size, int force_download);