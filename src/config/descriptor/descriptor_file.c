#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <filesystem/filepath.h>
#include <utils/trimws.h>
#include <utils/path.h>
#include <utils/terminal.h>
#include <filesystem/read_file.h>
#include <config/read_key.h>
#include <filesystem/cache/filecache.h>
#include <config/descriptor/descriptor_file.h>

/// @brief Locates and Opens the descriptor file for the given path
/// @param directory_path target path
/// @return FILE descriptor path
void build_descriptor_path(char *buf, size_t buf_size, const char *file_path) {
    if (!file_path || !buf) return;

    if (is_directory(file_path) == -1){
        fprintf(stderr, ERROR "Couldnt stat path, does the target exsist?\n");
        buf[0] = '\0';
        return;
    }

    char* parent_directory_path = get_parent_directory_path(file_path);
    char dir_name[PATH_MAX];
    get_file_directory_name(dir_name, sizeof(dir_name), file_path);

    snprintf(buf, buf_size, "%s%s.cfg", parent_directory_path, dir_name);
    printf(REQUEST "Found a descriptor file: %s\n", buf);

    free(parent_directory_path);
}

/// @brief builds a path for the descriptor file and returns the file
/// @param file_path target directory
/// @return pointer to file
descriptor_t *read_descriptor_file(const char* file_path){
    if (!file_path) return NULL;
    size_t filesize = 0;

    char descriptor_path[PATH_MAX];
    build_descriptor_path(descriptor_path, sizeof(descriptor_path), file_path);
    if (!descriptor_path[0]) return NULL;

    const char *descriptor_file = cache_get_file(&directory_descriptor_cache, descriptor_path, &filesize);
    if (!descriptor_file) return NULL;

    descriptor_t *descriptor = calloc(1, sizeof(descriptor_t));
    if (!descriptor) return NULL;
    
    descriptor->hidden = file_get_int(descriptor_file, "hidden");
    descriptor->page = file_get_value(descriptor_file, "page");
    descriptor->page = trim_whitespaces(descriptor->page);

    return descriptor;
}