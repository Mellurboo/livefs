#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Construct the file path from the config file and append a file name
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
const char *build_file_path(const char *filename);

/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
const char *parse_config_root_path(void);

/// @brief gets the stripped name of a parent directory
/// @param file_path target path
/// @return only returns the name of the directory name, NOT WHOLE PATH!
const char *get_file_directory_name(const char *file_path);

/// @brief check if the path provided exsists
/// @param path target
/// @return success or exit
int path_exsists(const char *path);