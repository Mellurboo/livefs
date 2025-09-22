#
#	This makefile is coppied from my other project mellurboo/specseek
#	I have removed 32 bit support, but if anyone would like to reintroduce it
#	this makefile should be easy enough to append to, good luck.
#

#
# Target Output Details
#
ELF_TARGET_64 = livefs64

#
# Compilers for Differing Targets
#
CC ?= gcc

#
# Flag Options for Compilers
#
CFLAGS ?=
COMMON_CFLAGS = -static -Wall -Wextra -Werror -Wno-unused-function -Wno-unused-but-set-parameter -Wno-unused-parameter -O2 -D_FILE_OFFSET_BITS=64 -I include

#
# Runtime arguments
#
RUN_ARGS ?=

#
# Output directories as variables
#
ELF_BIN_DIR_64 = bin/elf/64
ELF_OBJ_DIR_64 = $(ELF_BIN_DIR_64)/obj

#
# Detect Source files in Code, this is very broad
# and will just compile anything it finds
#
SRCS = $(shell find src -name '*.c')

#
# Object files per arch
#
ELF_OBJS_64 = $(patsubst src/%.c, $(ELF_OBJ_DIR_64)/%.gcc.o, $(SRCS))

#
# Default Command (no args)
#
all: $(ELF_TARGET_64)

#
# Linux 64-bit build
#
$(ELF_TARGET_64): $(ELF_OBJS_64)
	@mkdir -p $(ELF_BIN_DIR_64)
	$(CC) $(COMMON_CFLAGS) $(CFLAGS) -m64 -o $(ELF_BIN_DIR_64)/$(ELF_TARGET_64) $^
	cp config.cfg $(ELF_BIN_DIR_64)
#
# Object build rules per architecture
#
$(ELF_OBJ_DIR_64)/%.gcc.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_CFLAGS) $(CFLAGS) -m64 -c $< -o $@

run: $(ELF_TARGET_64)
	@./$(ELF_BIN_DIR_64)/$(ELF_TARGET_64) $(RUN_ARGS)

#
# Clean
#
clean:
	rm -rf bin
	rm -rf .vscode

.PHONY: all clean run debug
