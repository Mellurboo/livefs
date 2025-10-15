#pragma once

/// @brief checks if the argument exsists (automatically appends ?)
/// @param request request line
/// @param argument arguement key
/// @return 0 false, 1 true
int request_has_arguement(const char *request, const char *argument);

/// @brief removes the arguemnts from a Request
/// @param request target
/// @return stripped request
const char *strip_arguemnts(const char *request);