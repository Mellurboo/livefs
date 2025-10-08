#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <main.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <config/config_file.h>
#include <utils/path.h>
#include <unistd.h>
#include <fs/filepath.h>
#include <socket/request_arguement.h>

/// @brief check if the path provided exsists
/// @param path target
/// @return success or exit
int path_exsists(const char *path){
    struct stat st;
    if (stat(path, &st) == -1 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s root path does not exist: %s\n", FATAL, path);
        exit(-1);
    }
    return 0;
}

/// @brief Construct the file path from the config file and append a file name
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
const char *build_file_path(const char *filename) {
    const char *root_path = parse_config_root_path();
    if (!root_path || !*root_path) {
        fprintf(stderr, "FATAL: Root Path was empty\n");
        return "";
    }

    if (!filename) filename = "";         // Check NULL first
    if (filename[0] == '/') filename++;   // Skip leading '/'

    static char full_path[PATH_MAX];
    
    if (strcmp(filename, "") == 0) {     // serve default index.html
        snprintf(full_path, sizeof(full_path), "%s/index.html", root_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s/%s", root_path, filename);
    }

    return strip_arguemnts(full_path);
}

/// @brief Builds the Config Root Path, the server will read from here when looking for files
/// @return Parsed file path, the root for which the servers accessable files are
const char *parse_config_root_path(void){
    char *config_file = get_config_file();

    const char *root_key = config_get_value(config_file, "root");
    if (!root_key){
        fprintf(stderr, FATAL "No root key provided, or is unreadable.");
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