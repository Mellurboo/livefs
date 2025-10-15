#pragma once

/// @brief gets the config file from disk
/// @param working_path current working directory
/// @return FILE config
char *get_config_file();

/// @brief Gets the value related to a key from the configuration file
/// @param config_file config file context
/// @param key key to retrieve
/// @return int value of key
int config_get_int(const char *config_file, const char *key);

/// @brief gets the value of any non number key
/// @param config_file config file context
/// @param key name
/// @return value of key
char *config_get_value(const char *config_file, const char *key);