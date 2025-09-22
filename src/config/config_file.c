#include <config/config_file.h>

char *cached_config_file = NULL;

/// @brief gets the config file from disk
/// @param working_path current working directory
/// @return FILE config
char *get_config_file(const char *working_path) {
    if (cached_config_file) return cached_config_file;

    // new config path buffer to not tamper with the other one, also we add a null terminator.
    char config_path[PATH_MAX];
    strncpy(config_path, working_path, sizeof(config_path) - 1);
    config_path[sizeof(config_path) - 1] = '\0';

    // ensure we end in a / so that we can just append filename without worrying about leading /'s
    size_t len = strlen(config_path);
    if (len > 0 && config_path[len - 1] != '/') {
        strncat(config_path, "/", sizeof(config_path) - strlen(config_path) - 1);
    }

    // concatonate the config file name and then check if it actually exsists
    strncat(config_path, "config.cfg", sizeof(config_path) - strlen(config_path) - 1);
    FILE *fp = fopen(config_path, "rb");
    if (!fp) {
        //TODO: maybe implement config file rebuilding here?
        fprintf(stderr, "%s Failed to open config file (%s).\n", ERROR, config_path);
        perror("fopen");
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    cached_config_file = malloc(filesize);
    if (!cached_config_file){
        return NULL;
    }

    size_t read_size = fread(cached_config_file, 1, filesize, fp);
    if (read_size != filesize){
        fprintf(stderr, "%s Shortread :( %zu/%zu bytes\n", ERROR, read_size, filesize);
        free(cached_config_file);
        cached_config_file = NULL;
        return NULL;
    }

    // finally add null term
    cached_config_file[filesize] = '\0';
    cached_config_file = trim_whitespaces(cached_config_file);  // do us a favour and trim the whitespace
    return cached_config_file;
}
