#include <config/config_file.h>
#include <utils/path.h>

/// @brief gets the config file from disk
/// @return FILE config
char *get_config_file(char *buf) {
    char *config_file = "/config.cfg";
    char config_file_path[PATH_MAX];

    // if there is a config file in the same directory as the executable, we will piroritise it
    strcpy(config_file_path, get_current_working_path());
    strcat(config_file_path, config_file);
    FILE *fp = fopen(config_file_path, "rb");
    if (fp) {
        size_t read_config = fread(buf, 1, PATH_MAX - 1, fp);
        fclose(fp);
        buf[read_config] = '\0';

        char *trimmed_config = trim_whitespaces(buf);
        if (trimmed_config != buf){
            memmove(buf, trimmed_config, strlen(trimmed_config) + 1);
        }

        return buf;
    }
    return "\0";
}