#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <utils/terminal.h>
#include <utils/path.h>
#include <linux/limits.h>

#define PATTERN_SIZE    128


/// @brief gets the value of a key
/// @param file target file
/// @param key key name str
/// @return key value
char *get_key_in_file(char *pattern, const char *config_file, const char *key){
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

/// @brief gets the integer value of a file passed
/// @param file target file
/// @param key key name str
/// @return value of key if found
int file_get_int(const char *file, const char *key){
    char pattern[PATTERN_SIZE];
    char *pos = get_key_in_file(pattern, file, key);
    if (!pos) return 0;

    // extract the value
    pos += strlen(pattern);
    char *endptr;
    long value = strtol(pos, &endptr, 10);

    if (pos == endptr) {
        fprintf(stderr, ERROR "Invalid integer for key %s\n", key);
        return 0;
    }
    return (int)value;
}

/// @brief gets the value of any non number key
/// @param config_file config file context
/// @param key name
/// @return value of key
char *file_get_value(const char *file, const char *key){
    char pattern[PATTERN_SIZE];
    snprintf(pattern, sizeof(pattern), "%s = ", key);

    const char *pos = strstr(file, pattern);
    if (!pos) {
        fprintf(stderr, ERROR "Config key not found: %s\n", key);
        exit(1);
        return NULL;
    }

    pos += strlen(pattern);

    // if something is empty it segfaults here! fix it!
    // NOTE BY GLIDE: can't really prevent a segfault because these aren't optional
    // so the best option is to just exit.

    // Find the end of the line
    const char *end = strchr(pos, '\n');
    static char temp[PATH_MAX]; // temporary buffer
    size_t len = end ? (size_t)(end - pos) : strlen(pos);
    if (len >= sizeof(temp)) len = sizeof(temp) - 1;
    memcpy(temp, pos, len);
    temp[len] = '\0';

    return temp;
}
