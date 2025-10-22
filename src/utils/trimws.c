#include <string.h>
#include <ctype.h>

/// @brief trims the whitespace of a string completly
/// @param str target
/// @return trimmed string
char *trim_whitespaces(char *str){
    char *end;

    while (isspace((unsigned char)*str)) str++; // remove leading
    if (*str == 0){ return str; }               // if the string is now empty, just return str.
    end = str+strlen(str) - 1;                  // remove trailing
    while (end > str && isspace((unsigned char) *end)) end--;

    // replace the null term
    *(end + 1) = '\0';
    return str;
}
