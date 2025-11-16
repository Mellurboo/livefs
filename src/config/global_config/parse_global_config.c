#define _GNU_SOURCE
#include <config/read_key.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils/terminal.h>
#include <utils/path.h>
#include <filesystem/filepath.h>
#include <config/global_config/global_config_file.h>

// global config is loaded and parsed at start so we dont need to worry about reloading or anythign

global_config_t *global_config = {0};

/// @brief runs some checks to ensure the values are valid, if not we just print an error message and return -1
/// @return 0 success -1 errornous
int verify_config(){
    if (global_config->port < 0){
        fprintf(stderr, FATAL "Integer Value 'port' was invalid or missing 'port = %i'\n", global_config->port);
        return 0;
    }


    if (!path_exists(parse_config_root_path())){
        fprintf(stderr, FATAL "Server Root Path was invalid or missing, 'root = %s'\n", global_config->root);
        return 0;
    }

    if (!global_config->enable_ssl){
        if(!global_config->allow_insecure_HTTP_connections){
            fprintf(stderr, WARN "SSL and Insecure Connections are both false, no one will ever be able to connect as both protocols are disabled\n");
        }
    }

    if (global_config->enable_ssl && (!path_exists(global_config->ssl_cert_path) || !path_exists(global_config->ssl_key_path))){
        fprintf(stderr, FATAL "SSL Certificate or Key path is empty or invalid and SSL is enabled, please provide a valid cert and key file\n");
        fprintf(stderr, FATAL "ssl_cert_path = %s\n", global_config->ssl_cert_path);
        fprintf(stderr, FATAL "ssl_key_path = %s\n", global_config->ssl_key_path);
        return 0;
    }

    return 1;
}

global_config_t *create_global_config_structure(){
    char *config_file = get_config_file();
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
    global_config->allow_insecure_HTTP_connections = file_get_int(config_file, "allow_insecure_HTTP_connections");
    global_config->enforce_descriptor_files = file_get_int(config_file, "enforce_descriptor_files");

    if (global_config->enable_ssl == 1) {
        global_config->ssl_cert_path = strdup(file_get_value(config_file, "ssl_cert_path"));
        global_config->ssl_key_path = strdup(file_get_value(config_file, "ssl_key_path"));
    } else {
        global_config->ssl_cert_path = NULL;
        global_config->ssl_key_path = NULL;
    }
    
    free(config_file);

    if (!verify_config()) exit(-1);
    return global_config;
}

global_config_t *get_global_config_structure(){
    return global_config;
}