#pragma once
#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>
#include <stdlib.h>

const char *get_current_exec_path(char *buf, size_t buf_size);
const char *get_home_path(void);