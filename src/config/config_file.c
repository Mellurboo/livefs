#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <config/config_file.h>
#include <config/create_config.h>
#include <utils/path.h>

static FILE *open_config_file(char *out_buffer, size_t out_len){
    const char *config_file = "/config.cfg";
    char exec_path_buffer[PATH_MAX];    //this is dodgy i hate it

    /*
        First we will try see if the config file is in the executable directory
        then we will check if it is in ~/.config/livefs
        if it isnt there then we will try and make one in ~/.config/livefs
    */

    const char *exec_path = get_current_exec_path(exec_path_buffer);
    
    if (exec_path){
        if (snprintf(out_buffer, out_len, "%s%s", exec_path, config_file) < 0) return NULL;
        FILE *fp = fopen(out_buffer, "rb");
        if (fp) return fp;
    }

    const char *home_path = get_home_path();
    if (home_path){
        if (snprintf(out_buffer, out_len, "%s/.config/livefs%s", home_path, config_file) < 0) return NULL;
        FILE *fp = fopen(out_buffer, "rb");
        if (fp) return fp;
    }

    if (create_config_directory() == 0 && create_config_file() == 0){
        printf(INFO "livefs has generated a config file at ~/.config/livefs/\n");
        if (home_path){
            if (snprintf(out_buffer, out_len, "%s/.config/livefs%s", home_path, config_file) < 0) return NULL;
            FILE *fp = fopen(out_buffer, "rb");
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

    size_t config_file_size = (size_t)ftell(fp);
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