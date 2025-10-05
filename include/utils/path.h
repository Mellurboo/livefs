#pragma once
#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>

char *get_current_exec_path(void);
char *get_home_path(void);