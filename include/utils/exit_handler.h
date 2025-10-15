#pragma once

/// @brief catches signal response
/// @param sig 
void graious_shutdown(int sig);

/// @brief register the signals to handler functions
void exit_handlers();