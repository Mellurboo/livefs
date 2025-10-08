#pragma once

/// @return HTTP bad request header
const char *http_bad_request_header();

/// @return HTTP 403 not found header
const char *http_forbidden();

/// @return HTTP 405 method not allowed header
const char *http_method_not_allowed();

/// @return HTTP 404 not found header
const char *http_not_found_header();

const char *http_im_a_teapot();

/// @return Place HTTP 200 in Buffer Provided
const char *http_success(char *fname, off_t content_size, int force_download);