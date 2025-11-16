#pragma once

typedef struct{
    int port;
    char *root;

    int enable_ssl;
    int allow_insecure_HTTP_connections;
    const char *ssl_cert_path;
    const char *ssl_key_path;

    int enforce_descriptor_files;
}global_config_t;

extern global_config_t *global_config;

/// @brief gets the config file from disk
/// @param working_path current working directory
/// @return FILE config
char *get_config_file();

global_config_t *create_global_config_structure();
global_config_t *get_global_config_structure();

/// @brief free the config file structure
/// @param config_file config context
void clear_config(global_config_t *config_file);