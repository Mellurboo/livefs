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

    size_t extention_len = strlen(extention);

    // woah spagetti, loop over the content types, check length and then compare the full extention
    for (size_t i = 0; i < sizeof(content_types)/sizeof(content_types[0]); i++){
        if (extention_len == content_types[i].extention_length && extention[0] == content_types[i].extention[0]){
            if (memcmp(extention, content_types[i].extention, extention_len) == 0){
                return content_types[i].content_type;
            }
        }
    }

    return "application/octet-stream";
}