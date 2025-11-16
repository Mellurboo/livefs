#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/path.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <config/global_config/global_config_file.h>
#include <config/global_config/create_global_config.h>

/// @brief Creates .config/livefs file if it doesn't exsist
/// @return success?
int create_config_directory(){
    const char *home_path = get_home_path();
    if (!home_path){
        fprintf(stderr, FATAL "failure to determine home path");
        return -1;
    }

    const char *config_file_path = "/.config/livefs";
    char config_dir[PATH_MAX];

    size_t directory_path_size = strlen(get_home_path()) + strlen(config_file_path) + 1;
    char *directory_path = malloc(directory_path_size);

    if (!directory_path){
        fprintf(stderr, FATAL "Out of Memory!\n");
        exit(-1);
    }

    snprintf(config_dir, sizeof(config_dir), "%s/.config", get_home_path());
    snprintf(directory_path, directory_path_size, "%s%s", get_home_path(), config_file_path);

    if (mkdir(config_dir, 0700) != 0 && errno != EEXIST){
        perror("mkdir ~/.config");
        return -1;
    }

    if (mkdir(directory_path, 0700) == 0){
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

/// @brief creates the config.cfg file
/// @return success?
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

    fprintf(fp, DEFAULT_CONFIG);   // write the default config file content
    fclose(fp);

    return 0;
}
