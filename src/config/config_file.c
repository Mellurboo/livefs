#include <config/config_file.h>
#include <utils/path.h>

/// @brief gets the config file from disk
/// @return FILE config
char *get_config_file(char *buf) {
    char *config_file = "/config.cfg";
    char config_file_path[PATH_MAX];

    // if there is a config file in the same directory as the executable, we will piroritise it
    strcpy(config_file_path, get_current_exec_path(config_file_path, PATH_MAX));
    strcat(config_file_path, config_file);
    FILE *fp;
    fp = fopen(config_file_path, "rb");

    if (!fp) {
        // now we check if its in .config/
        memset(config_file_path, 0, strlen(config_file_path));
        strcpy(config_file_path, get_home_path());
        strcat(config_file_path, "/.config/livefs");
        strcat(config_file_path, config_file);
        fp = fopen(config_file_path, "rb");
        if (!fp) {
            fprintf(stderr, FATAL "No Config file found!\n");
            exit(-1);
        }
    }
    
    size_t read_config = fread(buf, 1, PATH_MAX - 1, fp);
    fclose(fp);
    buf[read_config] = '\0';

    char *trimmed_config = trim_whitespaces(buf);
    if (trimmed_config != buf){
        memmove(buf, trimmed_config, strlen(trimmed_config) + 1);
    }

    return buf;
}