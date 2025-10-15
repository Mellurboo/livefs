#pragma once

/// @brief gets the parent path of the current working directory.
/// @return path
const char *get_current_exec_path();

/// @brief returns the home path of the current user
/// @return path
const char *get_home_path();

/// @brief gets the parent of the direcotry or file provided
/// @param path target
/// @return child path
char *get_parent_directory_path(const char *filepath);