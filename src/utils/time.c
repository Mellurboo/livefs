#include <stdio.h>
#include <time.h>
#include <utils/terminal.h>

/// @brief gets the current time of the server
/// @return time string
const char *get_current_server_time(){
    time_t current_time = time(NULL);
    if (current_time == -1) {
        fprintf(stderr, ERROR "Failure to get server time");
        return "error";
    }

    char *time = ctime(&current_time);
    return time;
}