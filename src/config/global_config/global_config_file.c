#include <stdio.h>
#include <linux/limits.h>
#include <utils/terminal.h>
#include <utils/trimws.h>
#include <utils/path.h>
#include <stdlib.h>
#include <filesystem/filepath.h>
#include <filesystem/read_file.h>
#include <config/global_config/create_global_config.h>

/*
    =======================================
    This file is planned for a large reform
    =======================================

    I want to move over to a more static structure than what I have right now.
    for example I want to have the config file be a structure such as config_t?
    I am not too sure I want to make this an extention of what we have no where we
    just have set fields such as port. or if i just leave it as it is and dynamically access it
    I feel it would be a bigger buren on memory, but also I could store the structure and then free
    the config file straight away, it would lock down the config file.

    I am thinking I will go through with it though because It means i can just free the config file here
    and keep the structure. it feels more sensible in terms of memory. also it means I can cut out the usage
    of expensive functions such as config_get_int(), config_get_value() and find_key_in_config().

    at this point in time the config file stays in memory until just before the server port is bound
    which means accessing it beforehand means reopening, rereading and reallocating the config file.
    off the top of my head I don't think this is done at any point yet? but this approach will be more robust

    on the other hand im sick of working on the config file
*/

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