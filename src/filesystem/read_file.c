#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <utils/path.h>
#include <utils/trimws.h>
#include <utils/terminal.h>
#include <config/global_config/create_global_config.h>
#include <config/global_config/global_config_file.h>

/// @brief Reads the file and places it in memory, Must be freed by the caller
/// @param directory_path 
/// @return 
char *read_file(const char* directory_path){
    if (!directory_path) return NULL;

    FILE *fp = fopen(directory_path, "rb");
    if (!fp){
        fprintf(stderr, ERROR "Failure to open file: %s\n", directory_path);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0){
        fprintf(stderr, ERROR "failure to seek along file %s\n", directory_path);
        fclose(fp);
        return NULL;
    }

    size_t filesize = (size_t)ftell(fp);
    rewind(fp);

    char *file = malloc(filesize + 1);
    if (!file){
        fprintf(stderr, ERROR "Failure to allocate %zu bytes for file, out of memory?\n", filesize);
        fclose(fp);
        return NULL;
    }


    size_t file_read = 0;
    if (filesize > 0){
        file_read = fread(file, 1, filesize, fp);
        if (file_read != filesize && ferror(fp)){
            fprintf(stderr, ERROR "Error reading config file into memory: %s\n", strerror(errno));
            free(file);
            fclose(fp);
            return NULL;
        }
    }

    file[file_read] = '\0';
    fclose(fp);
    return file;
}