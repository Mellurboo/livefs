#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <utils/path.h>
#include <utils/trimws.h>

#include <config/global_config/create_global_config.h>
#include <config/global_config/config_file.h>

/// @brief Reads the file and places it in memory, Must be freed by the caller
/// @param directory_path target directory path
/// @return file memory pointer
char *read_file(const char* directory_path);