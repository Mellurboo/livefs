#define GT_IMPLEMENTATION

#include <stdio.h>
#include <unistd.h>
#include <utils/path.h>
#include <vendor/gt/gt.h>
#include <socket/socket.h>
#include <utils/terminal.h>
#include <config/read_key.h>
#include <utils/exit_handler.h>
#include <filesystem/filepath.h>
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

int server_socket = 0;      // I also want to move this i think having this in main.c is silly

/// @brief this code executes when the server catches an exit signal
void exitcall(void){
    printf(INFO "livefs has closed\n");
    close(server_socket);
}

/// Program Entry point, initializes the server
/// @param argc
/// @param argv
/// @return success?
int main(int argc, const char *argv[]){
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

    int srvport = config_get_int(config_file, "port");
    server_socket = server_create_socket(server_socket);
    
    free(config_file);

    server_bind_socket(srvport, server_socket);
    server_listen(server_socket);
    client_listener(server_socket);

    // Clean up, at this point the server is closed
    close(server_socket);
}