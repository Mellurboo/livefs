#pragma once

typedef struct descriptor{
    int hidden;
    char *page;
}descriptor_t;

/// @brief builds a path for the descriptor file and returns the file
/// @param file_path target directory
/// @return pointer to file
descriptor_t *read_descriptor_file(const char* file_path);

/// @brief memory safe free, use this instead of free(descriptor)
/// @param descriptor 
void free_descriptor(descriptor_t *descriptor);
