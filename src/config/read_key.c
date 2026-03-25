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
    snprintf(pattern, PATTERN_SIZE, "%s", key);
    const char *config_read = config_file;

    while (*config_read){
        if (*config_read == '#'){
            while (*config_read && *config_read != '\n') config_read++;
            if (*config_read == '\n') config_read++;
            continue;
        }

        if (*config_read == '\n') {
            config_read++;
            continue;
        }

        if (strncmp(config_read, key, strlen(key)) == 0){
            const char *pos = config_read + strlen(key);

            // Skip whitespace before the assignment token =
            while (*pos == ' ' || *pos == '\t') pos++;
            
            if (*pos != '='){
                fprintf(stderr, ERROR "Invalid Syntax, key '%s' is missing assignment operator '='\n", key);
                return NULL;
            }

            pos++; //skip =

            while (*pos == ' ' || *pos == '\t') pos++;
            return (char*)pos;
        }

        while (*config_read && *config_read != '\n') config_read++;
        if (*config_read == '\n') config_read++;
    
    }

    fprintf(stderr, ERROR "Failed to find key requested? '%s'", key);
    return NULL;
}

/// @brief gets the integer value of a file passed
/// @param file target file
/// @param key key name str
/// @return value of key if found
int file_get_int(const char *file, const char *key){
    char pattern[PATTERN_SIZE];
    char *pos = get_key_in_file(pattern, file, key);
    if (!pos) return 0;

    char *endpointer;
    long value = strtol(pos, &endpointer, 10);

    if (pos == endpointer){
        fprintf(stderr, ERROR "Expected Integer at key '%s'\n", key);
        return -1;
    }

    return (int)value;
}

/// @brief gets the value of any non number key
/// @param config_file config file context
/// @param key name
/// @param out output buffer
/// @param out_size output buffer size
void file_get_value(const char *file, const char *key, char *out, size_t out_size){
    char pattern[PATTERN_SIZE];
    char *pos = get_key_in_file(pattern, file, key);

    if (!pos || *pos == '\n' || *pos == '\0'){
        out[0] = '\0';
        return;
    }

    // extract entire line
    const char *end = strchr(pos, '\n');

    size_t len = end ? (size_t)(end - pos) : strlen(pos);
    if (len >= out_size) len = out_size - 1;

    memcpy(out, pos, len);
    out[len] = '\0';
}