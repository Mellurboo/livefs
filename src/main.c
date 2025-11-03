#define GT_IMPLEMENTATION

#include <main.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/path.h>
#include <vendor/gt/gt.h>
#include <socket/socket.h>
#include <utils/terminal.h>
#include <config/read_key.h>
#include <utils/exit_handler.h>
#include <filesystem/filepath.h>
#include <filesystem/cache/filecache.h>
#include <config/global_config/global_config_file.h>

/*
    =======================================
    This file is planned for a large reform
    =======================================

    I have neglected the use of arguments, one argument that I want
    to implement is a config generate argument, that generates a config
    file without starting the server, because right now as it stands the
    config file generates, the port default is 8080, and then the server starts
    with port 8080 being a standard port it may be unsafe. As a result. I am going to add
    a cli argument such as './livefs --generate-config' that will simply

    another argument I have considered is an argument such as './livefs --path-add [PATH]'
    this will simply generate a folder descriptor file on the path provided as the second argument
    then exit.

    Obviously I will want to have other standard arguments like '--help' ect...
*/

/// @brief this code executes when the server catches an exit signal
void exitcall(void){
    printf(INFO "livefs has closed\n");
    close(get_server_socket());
}

/// Gets the server socket
int get_server_socket(void){
    static int server_socket = 0;
    return server_socket;
}

/// Program Entry point, initializes the server
/// @param argc
/// @param argv
/// @return success?
int main(int argc, const char *argv[]){
    int server_socket = get_server_socket();
    int atexit_return = atexit(exitcall);
    if (atexit_return != 0) {printf(WARN "atexit was not set\n");}
    
    exit_handlers();

    char *config_file = get_config_file();

    if (!config_file){
        printf(FATAL "No Config File Found!\n");
        return -1;
    }
    printf(INFO "Registered Config File\n");

    const char *root_path = parse_config_root_path();
    path_exists(root_path);

    gtinit();

    int srvport = file_get_int(config_file, "port");
    server_socket = server_create_socket();

    if (file_get_int(config_file, "enable_ssl") == 1){
        #define ENABLE_SSL  1
        if (!initSSL(config_file)) { // ssl fails to enable even though its set to enable in config, wheyy
            fprintf(stderr, FATAL "Failure to enable SSL despite being set in config, ensure the key and cert is valid\n");
            exit(-1);
        }
    }

    server_cache_init();

    // I intend on making the cache size editable in the config file so
    // we will do cache init before we free it here.
    free(config_file);

    server_bind_socket(srvport, server_socket);
    server_listen(server_socket);
    client_listener(server_socket);

    // Clean up, at this point the server is closed
    close(server_socket);
}
