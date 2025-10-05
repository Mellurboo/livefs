#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/terminal.h>

void sigint_handler(int sig){
    printf(INFO "Interrupt Caught, Stopping safely\n");
    exit(0);
}

/// @brief register the signals to handler functions
void exit_handlers(){
    signal(SIGINT, sigint_handler);
}