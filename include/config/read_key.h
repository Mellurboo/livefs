#pragma once

/// @brief gets the value of a key
/// @param file target file
/// @param key key name str
/// @return key value
char *get_key_value(const char *file, const char *key);

/// @brief gets the integer value of a file passed
/// @param file target file
/// @param key key name str
/// @return value of key if found
int file_get_int(const char *file, const char *key);
