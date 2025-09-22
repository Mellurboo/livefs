#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Construct the file path from the config file and append a file name
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
void build_file_path(char *buf, size_t buf_size, const char *filename);

/// @brief check if the path provided exsists
/// @param path target
/// @return success or exit
int path_exsists(const char *path);

/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
void get_root_path(char *buf, size_t buf_size);