#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <config/config_file.h>
#include <config/create_config.h>
#include <utils/path.h>

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
static FILE *open_config_file(){
    const char *config_file = "/config.cfg";
    char config_file_path[PATH_MAX];
    /*
        First we will try see if the config file is in the executable directory
        then we will check if it is in ~/.config/livefs
        if it isnt there then we will try and make one in ~/.config/livefs
    */

    const char *exec_path = get_current_exec_path();
    
    if (exec_path){
        if (snprintf(config_file_path, sizeof(config_file_path), "%s%s", exec_path, config_file) < 0) return NULL;
        FILE *fp = fopen(config_file_path, "rb");
        if (fp) return fp;
    }

    const char *home_path = get_home_path();
    if (home_path){
        if (snprintf(config_file_path, sizeof(config_file_path), "%s/.config/livefs%s", home_path, config_file) < 0) return NULL;
        FILE *fp = fopen(config_file_path, "rb");
        if (fp) return fp;
    }

    if (create_config_directory() == 0 && create_config_file() == 0){
        printf(INFO "livefs has generated a config file at ~/.config/livefs/\n");
        if (home_path){
            if (snprintf(config_file_path, sizeof(config_file_path), "%s/.config/livefs%s", home_path, config_file) < 0) return NULL;
            FILE *fp = fopen(config_file_path, "rb");
            if (fp) return fp;
            
            // fallthrough here means something went south.
            fprintf(stderr, FATAL "Failed to open config files after creation, this error was fatal\n");
            exit(-1);
        }
    }else{
        fprintf(stderr, FATAL "livefs has failed to generate config files\n");
        return NULL;
    }

    return NULL;
}

/// @brief Reads the config file from disk and writes it to memory
/// @return pointer to config file in memory
char *read_config_file(void){
    char config_file_path[PATH_MAX];
    FILE *fp = open_config_file(config_file_path, sizeof config_file_path);
    
    if (!fp){
        fprintf(stderr, FATAL "Failed to open the config file from path %s\n", config_file_path);
        exit(-1);
    }

    if (fseek(fp, 0, SEEK_END) != 0){
        fprintf(stderr, FATAL "Reading config file size failure?\n");
        fclose(fp);
        exit(-1);
    }

    const size_t config_file_size = (size_t)ftell(fp);
    rewind(fp);

    char *untrimmed_config_file = malloc(config_file_size + 1);
    if (!untrimmed_config_file){
        fprintf(stderr, FATAL "Failure to allocate untrimmed config file, out of memory (allocating %zu bytes)\n", config_file_size);
        fclose(fp);
        return NULL;
    }

    size_t bytes_read = 0;
    if (config_file_size > 0){
        bytes_read = fread(untrimmed_config_file, 1, config_file_size, fp);
        if (bytes_read != config_file_size && ferror(fp)){
            fprintf(stderr, FATAL "Error reading Config file from memory: %s\n", strerror(errno));
            free(untrimmed_config_file);
            fclose(fp);
            return NULL;
        }
    }

    untrimmed_config_file[bytes_read] = '\0';
    fclose(fp);

    return untrimmed_config_file;
}

/// @brief trims config file whitespace, realloc the buffer
/// @return returns new pointer to config file
char *get_config_file(void){
    char *config_file = read_config_file();
    if (!config_file) return NULL;

    char *trimmed_config = trim_whitespaces(config_file);
    if (trimmed_config != config_file){
        memmove(config_file, trimmed_config, strlen(trimmed_config) + 1);
        config_file = realloc(config_file, strlen(config_file) + 1);
    }

    return config_file;
}