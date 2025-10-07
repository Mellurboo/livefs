#include <config/config_file.h>
#include <utils/exit_handler.h>
#include <socket/socket.h>
#include <fs/filepath.h>
#include <utils/path.h>
#include <stdio.h>
#include <main.h>

int server_socket = 0;

void exitcall(void){
    printf(INFO "livefs has closed\n");
    close(server_socket);
}

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

    char root_path[PATH_MAX];
    get_root_path(root_path, sizeof(root_path));
    path_exsists(root_path);

    int srvport = config_get_int(config_file, "port");
    server_socket = server_create_socket(server_socket);

    server_bind_socket(srvport, server_socket);
    server_listen(server_socket);
    client_listener(server_socket);

    // Clean up, at this point the server is closed
    close(server_socket);
    free(config_file);
}