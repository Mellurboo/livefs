#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <main.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <config/config_file.h>
#include <utils/path.h>
#include <unistd.h>

/// @brief check if the path provided exsists
/// @param path target
/// @return success or exit
int path_exsists(const char *path){
    struct stat st;
    if (stat(path, &st) == -1 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s root path does not exist: %s\n", FATAL, path);
        exit(-1);
    }
    return 0;
}

/// @brief Construct the file path from the config file and append a file name
/// @param buf dest buffer
/// @param buf_size dest buffer size
/// @param filename target filename
void build_file_path(char *buf, size_t buf_size, const char *filename){

    char working_directory[PATH_MAX];
    get_current_working_path(working_directory, PATH_MAX);

    char *config_file = get_config_file(working_directory);
    char totalpath[PATH_MAX];
    strncpy(totalpath, config_get_value(config_file, "root"), sizeof(totalpath) - 1);

    if (filename[0] == '/'){
        filename++;
    }

    if (totalpath[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) home = "";

        snprintf(buf, buf_size, "%s%s", home, totalpath + 1);
    } else {
        strncpy(buf, totalpath, buf_size - 1);
        buf[buf_size - 1] = '\0';
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] != '/') {
        strncat(buf, "/", buf_size - len - 1);
    }

    strncat(buf, filename, buf_size - strlen(buf) - 1);
}

void get_root_path(char *buf, size_t buf_size, const char *filename){

    char working_directory[PATH_MAX];
    get_current_working_path(working_directory, PATH_MAX);

    char *config_file = get_config_file(working_directory);
    char totalpath[PATH_MAX];
    strncpy(totalpath, config_get_value(config_file, "root"), sizeof(totalpath) - 1);

    if (filename[0] == '/'){
        filename++;
    }

    if (totalpath[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) home = "";

        snprintf(buf, buf_size, "%s%s", home, totalpath + 1);
    } else {
        strncpy(buf, totalpath, buf_size - 1);
        buf[buf_size - 1] = '\0';
    }
}