#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <config/create_config.h>
#include <utils/path.h>

int create_config_directory(void){
    const char *config_file_path = "/.config/livefs";
    size_t path_size = strlen(get_home_path()) + strlen(config_file_path) + 1;

    char *directory_path = malloc(path_size);
    if (!directory_path){
        fprintf(stderr, FATAL "Out of Memory!\n");
        exit(-1);
    }

    snprintf(directory_path, path_size, "%s%s", get_home_path(), config_file_path);

    if (mkdir(directory_path, 0777) == 0){
        free(directory_path);
        return 0;
    }else {
        if (errno == EEXIST){
            free(directory_path);
            return 0;
        }
        perror("mkdir");
        free(directory_path);
        return 1;
    }
}

int create_config_file(){
    const char *config_file_path = "/.config/livefs/config.cfg";
    size_t path_size = strlen(get_home_path()) + strlen(config_file_path) + 1;

    char *directory_path = malloc(path_size);
    if (directory_path == NULL){
        fprintf(stderr, FATAL "Out of Memory!\n");
        exit(-1);
    }

    snprintf(directory_path, path_size, "%s%s", get_home_path(), config_file_path);
    FILE *fp = fopen(directory_path, "w");

    if (!fp){
        fprintf(stderr, FATAL "Error Creating file in folder %s\n", directory_path);
        exit(-1);
    }

    fprintf(fp, STANDARD_CONFIG);
    fclose(fp);

    return 0;
}
