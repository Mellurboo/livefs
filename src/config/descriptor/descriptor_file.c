#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <utils/path.h>
#include <filesystem/filepath.h>

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
    if (!fp) {
        printf("Could not open descriptor file: %s\n", descriptor_file);
        return NULL;
    }

    return fp;
}

char *read_descriptor_file(const char* directory_path){
    if (!directory_path) return NULL;
    char *path = strdup(directory_path);
    path = get_parent_directory_path(path);
    
    FILE *fp = open_descriptor_file(directory_path);
    if (!fp) {
        fprintf(stderr, "Could not open descriptor file in '%s'\n", path);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "Failed to seek descriptor file\n");
        fclose(fp);
        return NULL;
    }

    size_t file_size = (size_t)ftell(fp);
    rewind(fp);

    char *descriptor_file = malloc(file_size + 1);
    if (!descriptor_file){
        fprintf(stderr, "Failed to allocate %zu bytes for descriptor file\n", file_size);
        fclose(fp);
        return NULL;
    }

    size_t bytes_read = fread(descriptor_file, 1, file_size, fp);
    if (bytes_read != file_size && ferror(fp)) {
        fprintf(stderr, "Error reading descriptor file!\n");
        free(descriptor_file);
        fclose(fp);
        return NULL;
    }

    descriptor_file[bytes_read] = '\0';
    fclose(fp);

    return descriptor_file;
}
