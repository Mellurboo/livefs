#include <stdio.h>
#include <string.h>
#include <utils/terminal.h>
#include <linux/limits.h>

/// @brief checks if the argument exsists (automatically appends ?)
/// @param request request line
/// @param argument arguement key
/// @return 0 false, 1 true
int request_has_arguement(const char *request, const char *argument){
    if (!request || !argument) return 0;
    char search_request[PATH_MAX];
    snprintf(search_request, sizeof(search_request), "?%s", argument);

    return strstr(request, search_request) != NULL;
}

/// @brief removes the arguemnts from a Request in place
/// @param buf target buffer to strip in place
/// @param buf_size size of buffer
void strip_arguemnts_buf(char *buf, size_t buf_size){
    (void)buf_size;
    if (!buf) return;
    char *arg_token = strchr(buf, '?');
    if (arg_token) *arg_token = '\0';
}