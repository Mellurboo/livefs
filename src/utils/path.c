#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <string.h>
#include <stdlib.h>

/// @brief gets the parent path of the current working directory.
/// @return path
const char *get_current_exec_path(void){
    static char buf[PATH_MAX];

    ssize_t len = readlink("/proc/self/exe", buf, PATH_MAX - 1);
    if (len == -1) {
        fprintf(stderr, ERROR "failed to get executable path\n");
        return NULL;
    }
    buf[len] = '\0'; // null terminate

    // strip off the filename to get the parent directory and 
    // truncate at the last '/'
    char *last_slash = strrchr(buf, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    return buf;
}

/// @brief returns the home path of the current user
/// @return path
const char *get_home_path(void){
    static char cached_home_path[PATH_MAX] = {0};

    if (cached_home_path[0] != '\0') return cached_home_path;

    const char *home_path = getenv("HOME");
    if (home_path == NULL || home_path[0] == '\0'){
        fprintf(stderr, ERROR "HOME environment variable is not set\n");
        return NULL;
    }

    strncpy(cached_home_path, home_path, sizeof(cached_home_path) - 1);
    cached_home_path[sizeof(cached_home_path) - 1] = '\0';

    return cached_home_path;
}

/// @brief gets the parent of the direcotry or file provided
/// @param path target
/// @return child path
char *get_parent_directory_path(const char *filepath){
    if (filepath == NULL) return NULL;
    char *path = strdup(filepath);
    int path_length = strlen(path);

    while (path_length > 1 && path[path_length - 1] == '/'){
        path[path_length--] = '\0';
    }

    char *parent_slash = strrchr(path, '/');
    if (parent_slash == NULL) return NULL;  // no parent?

    if (parent_slash == path){
        path[1] = '\0';
        return path;
    }

    *parent_slash = '\0';

    path_length = strlen(path);
    path[path_length] = '/';
    path[path_length + 1] = '\0';

    return path;
}