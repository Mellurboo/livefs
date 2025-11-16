#define _GNU_SOURCE
#include <config/read_key.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils/terminal.h>
#include <config/global_config/global_config_file.h>

// global config is loaded and parsed at start so we dont need to worry about reloading or anythign

global_config_t *global_config = {0};

global_config_t *create_global_config_structure(){
    char *config_file = strdup(get_config_file());
    if (!config_file){
        fprintf(stderr, FATAL "Config File Not Found\n");
    }
    
    global_config = malloc(sizeof(global_config_t));
    memset(global_config, 0, sizeof(global_config_t));
    
    if (!global_config){
        fprintf(stderr, FATAL "Failure to allocate global config structure\n");
        exit(-1);
    }

    global_config->port         = file_get_int(config_file, "port");
    global_config->root         = strdup(file_get_value(config_file, "root"));
    global_config->enable_ssl   = file_get_int(config_file, "enable_ssl");
    global_config->allow_insecure_connections = file_get_int(config_file, "allow_insecure_connections");

    if (global_config->enable_ssl == 1) {
        global_config->ssl_cert_path = strdup(file_get_value(config_file, "ssl_cert_path"));
        global_config->ssl_key_path = strdup(file_get_value(config_file, "ssl_key_path"));
    } else {
        global_config->ssl_cert_path = NULL;
        global_config->ssl_key_path = NULL;
    }
    
    free(config_file);
    return global_config;
}

global_config_t *get_global_config_structure(){
    return global_config;
}