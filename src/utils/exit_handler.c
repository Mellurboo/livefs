#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/terminal.h>

void graious_shutdown(int sig){
    printf(INFO "Signal Caught, Stopping safely\n");
    exit(0);
}

/// @brief register the signals to handler functions
void exit_handlers(){
    signal(SIGINT, graious_shutdown);
    signal(SIGTERM, graious_shutdown);
}