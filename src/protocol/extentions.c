#include <protocol/http.h>
#include <utils/terminal.h>
#include <stdio.h>
#include <string.h>

/// @brief content type lookup table
static const content_type_t content_types[] = {
    {".html", 5, "text/html"},
    {".htm",  4, "text/html"},
    {".css",  4, "text/css"},
    {".js",   3, "application/javascript"},
    {".jpg",  4, "image/jpeg"},
    {".jpeg", 5, "image/jpeg"},
    {".png",  4, "image/png"},
    {".gif",  4, "image/gif"},
    {".ico",  4, "image/x-icon"},
};

/// @brief uses the file name to serve the correct content type
/// @param filename target file
/// @return HTTP content type
const char *get_content_type(const char *filename){
    const char *extention = strrchr(filename, '.');
    if (!extention) return "application/octet-stream";

    char clean_extention[16] = {0};
    size_t i = 0;

    while (extention[i] && i < sizeof(clean_extention) - 1 && extention[i] != '\r' && extention[i] != '\n' && extention[i] != ' '){
        clean_extention[i] = extention[i];
        i++;
    }

    for (size_t j = 0; j < sizeof(content_types)/sizeof(content_types[0]); j++){
        if (strcmp(clean_extention, content_types[j].extention) == 0){
            return content_types[j].content_type;
        }
    }

    return "application/octet-stream";
}