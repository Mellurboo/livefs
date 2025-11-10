#define GT_IMPLEMENTATION

#include <main.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/path.h>
#include <vendor/gt/gt.h>
#include <socket/socket.h>
#include <utils/terminal.h>
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

    global_config_t *global_config = create_global_config_structure();
    if (!global_config){
        printf(FATAL "Failure to register config file or it doesnt exsist!\n");
        return -1;
    }
    printf(SUCCESS "Registered Config File\n");
    path_exists(global_config->root);

    gtinit();

    int srvport = global_config->port;
    server_socket = server_create_socket();

    if (global_config->enable_ssl){
        if (!initSSL(global_config, server_socket)) { // ssl fails to enable even though its set to enable in config, wheyy
            fprintf(stderr, FATAL "Found and Registered Certificate and Key Files\n");
            exit(-1);
        }
    }

    server_cache_init();

    server_bind_socket(srvport, server_socket);
    server_listen(server_socket);
    client_listener(server_socket);

    // Clean up, at this point the server is closed
    clear_config(global_config);
    close(server_socket);
}
