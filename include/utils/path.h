#pragma once
#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>

/// @brief gets the parent path of the current working directory.
/// @return path
const char *get_current_exec_path(void);

/// @brief returns the home path of the current user
/// @return path
const char *get_home_path(void);

/// @brief gets the parent of the direcotry or file provided
/// @param path target
/// @return child path
char *get_parent_directory_path(const char *filepath);