#include <stdio.h>
#include <linux/limits.h>
#include <utils/terminal.h>
#include <utils/trimws.h>
#include <utils/path.h>
#include <stdlib.h>
#include <filesystem/filepath.h>
#include <filesystem/read_file.h>
#include <config/global_config/create_global_config.h>

/// @brief Opens the config file or creates it or calls to create it if it doesnt exsist
/// @return returns the file pointer THE CALLER *MUST* CLOSE IT!
static char *get_config_file_dir() {
    const char *config_file = "/config.cfg";
    static char config_file_path[PATH_MAX];

    const char *exec_path = get_current_exec_path();
    
    if (exec_path) {
        if (snprintf(config_file_path, sizeof(config_file_path), "%s%s", exec_path, config_file) < 0) return NULL;
        if (path_exists(config_file_path)) return config_file_path;
    }

    const char *home_path = get_home_path();
    if (home_path) {
        if (snprintf(config_file_path, sizeof(config_file_path), "%s/.config/livefs%s", home_path, config_file) < 0) return NULL;
        if (path_exists(config_file_path)) return config_file_path;
    }

    if (create_config_directory() == 0 && create_config_file() == 0) {
        printf(INFO "livefs has generated a config file at ~/.config/livefs/\n");
        if (home_path) {
            if (snprintf(config_file_path, sizeof(config_file_path), "%s/.config/livefs%s", home_path, config_file) < 0) return NULL;
            if (path_exists(config_file_path)) return config_file_path;
        }
        fprintf(stderr, FATAL "Failed to open config files after creation, this error was fatal\n");
        exit(-1);
    }

    fprintf(stderr, FATAL "livefs has failed to generate config files\n");
    return NULL;
}


/// @brief trims config file whitespace, realloc the buffer
/// @return returns new pointer to config file
char *get_config_file(){
    char *config_file = (read_file(open_file(get_config_file_dir())));
    if (!config_file) return NULL;

    char *trimmed_config = trim_whitespaces(config_file);
    if (trimmed_config != config_file){
        free(config_file);
    }
    
    return trimmed_config;
}