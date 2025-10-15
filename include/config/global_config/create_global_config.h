#pragma once

#define STANDARD_CONFIG "port = 8080\nroot = ~/"

/// @brief Creates .config/livefs file if it doesn't exsist
/// @return success?
int create_config_directory();

/// @brief creates the config.cfg file
/// @return success?
int create_config_file();