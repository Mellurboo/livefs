#pragma once

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

/// @brief Takes a path and stats it to see if it is a dir.
/// @param path target
/// @return 1 = dir, 0 = file, -1 = errornous
int is_directory(const char *path);
