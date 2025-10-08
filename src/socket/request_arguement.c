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

/// @brief removes the arguemnts from a Request
/// @param request target
/// @return stripped request
const char *strip_arguemnts(const char *request){
    static char clean_request[PATH_MAX];
    if (!request) return "";

    strncpy(clean_request, request, sizeof(clean_request) - 1);
    clean_request[sizeof(clean_request) - 1] = '\0';

    char *arg_token = strchr(clean_request, '?');
    if (arg_token) *arg_token = '\0';

    return clean_request;
}