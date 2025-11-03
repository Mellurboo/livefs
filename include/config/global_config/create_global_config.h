#pragma once

#define STANDARD_CONFIG "port = 8080\nroot = ~/\n\n#security\nenable_ssl = 0\nallow_insecure_connections = 1\nssl_cert_path =\nssl_key_path ="

/// @brief Creates .config/livefs file if it doesn't exsist
/// @return success?
int create_config_directory();

/// @brief creates the config.cfg file
/// @return success?
int create_config_file();