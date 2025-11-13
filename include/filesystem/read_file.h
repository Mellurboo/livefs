#pragma once

/// @brief Reads the file and places it in memory, Must be freed by the caller
/// @param directory_path target directory path
/// @return file memory pointer
char *read_file(const char* filepath, size_t *filesize);