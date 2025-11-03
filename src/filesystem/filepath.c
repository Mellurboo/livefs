#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <utils/terminal.h>
#include <utils/path.h>
#include <config/global_config/global_config_file.h>

/// @brief check if the path provided exsists
/// @param path provided path
/// @return success or exit
int path_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

/// @brief Returns the pure server root path from config
/// @return buffer containing root path with slash
const char *parse_config_root_path(void){
    global_config_t *cfg = get_global_config_structure();
    if (!cfg || !cfg->root){
        fprintf(stderr, FATAL "No root path provided in global config.\n");
        return NULL;
    }

    static char root_path[PATH_MAX];
    memset(root_path, 0, sizeof(root_path));

    if (cfg->root[0] == '~'){
        snprintf(root_path, sizeof(root_path), "%s%s", get_home_path(), cfg->root + 1);
    } else {
        snprintf(root_path, sizeof(root_path), "%s", cfg->root);
    }

    // add trailing slash
    size_t len = strlen(root_path);
    if (len && root_path[len-1] != '/'){
        strncat(root_path, "/", sizeof(root_path) - len - 1);
    }

    return root_path;
}

/// @brief Build full file path from request path
/// @param request_path HTTP request path
/// @return static buffer with full path
const char *build_file_path(const char *request_path){
    const char *root = parse_config_root_path();
    if (!root) return NULL;
    static char full_path[PATH_MAX];
    memset(full_path, 0, sizeof(full_path));

    // skip leading slash on request path
    if (request_path && request_path[0] == '/'){
        snprintf(full_path, sizeof(full_path), "%s%s", root, request_path + 1);
    } else if (request_path) {
        snprintf(full_path, sizeof(full_path), "%s%s", root, request_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s", root);
    }

    return full_path;
}

/// @brief Get parent directory name of a path
/// @param file_path full path
/// @return static buffer with directory name
const char *get_file_directory_name(const char *file_path){
    static char name[PATH_MAX];
    if (!file_path) return NULL;

    char path[PATH_MAX];
    strncpy(path, file_path, sizeof(path)-1);
    path[sizeof(path)-1] = '\0';

    // Remove trailing slash
    size_t len = strlen(path);
    while (len > 1 && path[len-1] == '/') path[--len] = '\0';

    //return last component if its a dir
    if (is_directory(path)) {
        const char *last_slash = strrchr(path, '/');
        snprintf(name, sizeof(name), "%s", last_slash ? last_slash+1 : path);
        return name;
    }

    // if file get parent directory name
    char *last_slash = strrchr(path, '/');
    if (!last_slash) return NULL;

    *last_slash = '\0'; // terminate at parent
    char *parent_slash = strrchr(path, '/');
    snprintf(name, sizeof(name), "%s", parent_slash ? parent_slash+1 : path);
    return name;
}
