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
#include <config/read_key.h>
#include <filesystem/filepath.h>
#include <socket/request_arguement.h>
#include <config/global_config/global_config_file.h>

/// @brief check if the path provided exsists
/// @param path target
/// @return success or exit
int path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0; // returns 1 if exists, 0 if not
}

/// @brief Construct the file path from the config file and append a file name
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
const char *build_file_path(const char *filename) {
    const char *root_path = parse_config_root_path();
    if (!root_path || !*root_path) {
        fprintf(stderr, FATAL "Root Path was empty\n");
        return "";
    }

    if (!filename) filename = "";         // Check NULL first
    if (filename[0] == '/') filename++;   // Skip leading '/'

    static char full_path[PATH_MAX];
    
    if (strcmp(filename, "") == 0) {     // serve default index.html
        snprintf(full_path, sizeof(full_path), "%sindex.html", root_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", root_path, filename);
    }

    return strip_arguemnts(full_path);
}

/// @brief Builds the Config Root Path, the server will read from here when looking for files
/// @return Parsed file path, the root for which the servers accessable files are
const char *parse_config_root_path(void){
    char *config_file = get_config_file();

    const char *root_key = file_get_key(config_file, "root");
    if (!root_key){
        fprintf(stderr, FATAL "No root key provided, or is unreadable.\n");
        free(config_file);
        return NULL;
    }

    char total_path[PATH_MAX];
    strncpy(total_path, root_key, sizeof(total_path) - 1);
    total_path[sizeof(total_path) - 1] = '\0';

    static char parsed_path[PATH_MAX];
    if (total_path[0] == '~'){
        const char *home_path = get_home_path();
        snprintf(parsed_path, sizeof(parsed_path), "%s%s", home_path, total_path + 1);
    }else{
        strcpy(parsed_path, total_path);
    }

    free(config_file);
    return parsed_path;
}

/// @brief gets the stripped name of a parent directory
/// @param file_path target path
/// @return only returns the name of the directory name, NOT WHOLE PATH!
const char *get_file_directory_name(const char *file_path){
    char path[PATH_MAX];

    // copy path and remove trailing slashes
    strncpy(path, file_path, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';

    size_t len = strlen(path);
    while (len > 1 && path[len - 1] == '/') path[--len] = '\0';

    // Remove last component or filename
    char *last_slash = strrchr(path, '/');
    if (!last_slash) return NULL; // no parent dir
    *last_slash = '\0';

    // Find parent directory name
    char *parent_slash = strrchr(path, '/');
    char *parent_name = parent_slash ? parent_slash + 1 : path;

    // put the result of parent name in name and return as the result
    char *name = strdup(parent_name);
    return name;
}