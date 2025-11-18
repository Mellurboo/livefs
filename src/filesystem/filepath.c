#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utils/path.h>
#include <linux/limits.h>
#include <utils/terminal.h>
#include <filesystem/filepath.h>
#include <socket/request_arguement.h>
#include <config/descriptor/descriptor_file.h>
#include <config/global_config/global_config_file.h>

/// @brief check if the path provided exsists
/// @param path target
/// @return success or exit
int path_exists(const char *path) {
    if (!path) return 0;
    struct stat st;
    return stat(path, &st) == 0; // returns 1 if exists, 0 if not
}

/// @brief removes weird doubleslashes
/// @param path filepath
void normalize_path(char *path) {
    char *src = path;
    char *dst = path;
    while (*src) {
        *dst = *src++;
        if (*dst == '/') {
            while (*src == '/') src++;
        }
        dst++;
    }
    *dst = '\0';
}

/// @brief Construct the file path from the config file and append a file name
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
const char *build_file_path(const char *filename) {
    const char *root_path = parse_config_root_path();

    if (!filename) filename = "";         // Check NULL first
    if (filename[0] == '/') filename++;   // Skip leading '/'

    static char full_path[PATH_MAX];

    snprintf(full_path, sizeof(full_path), "%s/%s", root_path, filename);

    normalize_path(full_path);
    return strip_arguemnts(full_path);
}

/// @brief Builds the Config Root Path, the server will read from here when looking for files
/// @return Parsed file path, the root for which the servers accessable files are
const char *parse_config_root_path(void){
    global_config_t *global_config = get_global_config_structure();
    if (!global_config) {
        fprintf(stderr, FATAL "Global config not initialized\n");
        return "";
    }

    const char *root_key = global_config->root;
    if (!root_key || !*root_key){
        fprintf(stderr, FATAL "No root key provided, or it is empty.\n");
        return "";
    }

    static char parsed_path[PATH_MAX];
    if (root_key[0] == '~'){
        const char *home_path = get_home_path();
        snprintf(parsed_path, sizeof(parsed_path), "%s%s", home_path, root_key + 1);
    } else {
        strncpy(parsed_path, root_key, sizeof(parsed_path) - 1);
        parsed_path[sizeof(parsed_path) - 1] = '\0';
    }

    //remove trailing slashes
    size_t len = strlen(parsed_path);
    while (len > 1 && parsed_path[len - 1] == '/') parsed_path[--len] = '\0';

    return parsed_path;
}

/// @brief gets the stripped name of a parent directory
/// @param file_path target path
/// @return only returns the name of the directory name, NOT WHOLE PATH!
const char *get_file_directory_name(const char *file_path){
    static char name[PATH_MAX];
    char path[PATH_MAX];

    // sanitize file path
    strncpy(path, file_path, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';

    // remove trailing slashes
    size_t len = strlen(path);
    while (len > 1 && path[len - 1] == '/') path[--len] = '\0';

    // return dir name only
    if (is_directory(file_path) == 1) {
        const char *last_slash = strrchr(path, '/');
        const char *dir_name = last_slash ? last_slash + 1 : path;
        snprintf(name, sizeof(name), "%s", dir_name);
        return name;
    }

    char *last_slash = strrchr(path, '/');
    if (!last_slash) return NULL; // no parent dir
    *last_slash = '\0';

    char *parent_slash = strrchr(path, '/');
    const char *parent_name = parent_slash ? parent_slash + 1 : path;

    snprintf(name, sizeof(name), "%s", parent_name);
    return name;
}
