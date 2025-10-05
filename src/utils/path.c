#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <string.h>
#include <stdlib.h>

/// @brief gets the parent path of the current working directory.
/// @param buf dest buffer
/// @return path
char *get_current_exec_path(char *buf){
    ssize_t len = readlink("/proc/self/exe", buf, PATH_MAX - 1);
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

/// @brief returns the home path of the current user
/// @return path
const char *get_home_path(void){
    static char *cached_home_path = NULL;
    if (cached_home_path != NULL) return cached_home_path;

    char *home = getenv("HOME");
    if (home == NULL){
        // we have to check for this and then return as we are about
        // to add a '/', so if this value is empty it will be just '/' a.k.a
        // root, potentially exposing the servers root directory!
        return NULL;
    }

    size_t len = strlen(home);
    if (len + 1 < sizeof(home)){
        home[len] = '/';
        home[len+1] = '\0';
    }

    cached_home_path = home;
    return cached_home_path;
}