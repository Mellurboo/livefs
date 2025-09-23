#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <config/config_file.h>
#include <utils/path.h>

/// @brief Finds and Isolates the key in the config file
/// @param pattern search pattern
/// @param config_file config file context
/// @param key key name
/// @return key in the config file
char *find_key_in_config(char *pattern, const char *config_file, const char *key){
    /*
        Here we define the formatting must be
        key = value
        and it is whitespace sensitive because im lazy :p
    */
    snprintf(pattern, PATTERN_SIZE, "%s = ", key);

    // find the key in the loaded config file
    char *pos = strstr(config_file, pattern);
    if (!pos) {
        fprintf(stderr, ERROR "Config key not found: %s\n", key);
        return NULL;
    }

    return pos;
}

/// @brief Gets the value related to a key from the configuration file
/// @param config_file config file context
/// @param key key to retrieve
/// @return int value of key
int config_get_int(const char *config_file, const char *key){
    char pattern[PATTERN_SIZE];
    char *pos = find_key_in_config(pattern, config_file, key);

    // extract the value
    pos += strlen(pattern);
    char *endptr;
    long value = strtol(pos, &endptr, 10);

    if (pos == endptr) {
        fprintf(stderr, ERROR "Invalid integer for key %s\n", key);
        return -1;
    }

    return (int)value;
}

/// @brief gets the value of any non number key
/// @param config_file config file context
/// @param key name
/// @return value of key
char *config_get_value(const char *config_file, const char *key){
    char pattern[PATTERN_SIZE];
    char *pos = find_key_in_config(pattern, config_file, key);

    pos += strlen(pattern);
    return pos;
}