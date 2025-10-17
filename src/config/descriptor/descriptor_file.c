#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <filesystem/filepath.h>
#include <utils/path.h>
#include <filesystem/read_file.h>
#include <config/read_key.h>
#include <config/descriptor/descriptor_file.h>

/// @brief Locates and Opens the descriptor file for the given path
/// @param directory_path target path
/// @return FILE descriptor path
static FILE *open_descriptor_file(const char* file_path) {
    if (!file_path) return NULL;
    char descriptor_file[PATH_MAX];

    // Build descriptor path
    snprintf(descriptor_file, sizeof(descriptor_file), "%s%s.cfg", get_parent_directory_path(file_path), get_file_directory_name(file_path));
    printf("descriptor_file: %s\n", descriptor_file);

    FILE *fp = fopen(descriptor_file, "rb");
    if (!fp) return NULL;
    return fp;
}

/// @brief builds a path for the descriptor file and returns the file
/// @param file_path target directory
/// @return pointer to file
descriptor_t *read_descriptor_file(const char* file_path){
    if (!file_path) return NULL;
    const char *dir_name = get_file_directory_name(file_path);
    char *dir_path = get_parent_directory_path(strdup(file_path));
    if (!dir_path || !dir_name) return NULL;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s%s.cfg", dir_path, dir_name);
    
    char *descriptor_file = read_file(path);
    if (!descriptor_file){
        free(dir_path);
        return NULL;
    }

    descriptor_t *descriptor = calloc(1, sizeof(descriptor_t));
    if (!descriptor){
        free(descriptor_file);
        return NULL;
    }

    descriptor->hidden = file_get_int(descriptor_file, "hidden");
    descriptor->page = file_get_key(descriptor_file, "page");
    
    return descriptor;
}
