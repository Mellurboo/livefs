#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <string.h>

/// @brief gets the parent path of the current working directory.
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @return path
const char *get_current_working_path(char *buf, size_t buf_size){
    ssize_t len = readlink("/proc/self/exe", buf, buf_size - 1);
    if (len == -1) {
        fprintf(stderr, ERROR "failed to get executable path\n");
        return NULL;
    }
    buf[len] = '\0'; // null terminate

    // strip off the filename to get the parent directory
    char *last_slash = strrchr(buf, '/');
    if (last_slash != NULL) {
        *last_slash = '\0'; // truncate at the last '/'
    }

    return buf;
}