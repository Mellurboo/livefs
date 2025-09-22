#pragma once
#include <unistd.h>
#include <stdio.h>
#include <utils/terminal.h>
#include <linux/limits.h>

const char *get_current_working_path(char *buf, size_t buf_size);