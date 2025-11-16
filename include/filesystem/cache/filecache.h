#pragma once

#include <time.h>
#include <stddef.h>
#include <vendor/gt/gt.h>
#include <sys/types.h>

/// @brief Individual File Cache Entries
typedef struct {
    char *path;
    char *data;
    size_t size;
    time_t last_modified;
} file_cache_entry_t;

/// @brief (Collection of File Cache entries) structure
typedef struct {
    file_cache_entry_t *entries;
    size_t count;
    size_t capacity;
    GTMutex threadlock;
} file_cache_t;

extern file_cache_t file_data_cache;
extern file_cache_t directory_descriptor_cache;

/// @brief Initialize server Cache context
void server_cache_init(void);

void server_free_all_cache(void);

/// @brief find file_cache_entry in file_cache context
/// @param cache cache context
/// @param path path of file
/// @return file_cache_entry index of filepath if cached, -1 indicates failure
static ssize_t cache_find_entry(file_cache_t *cache, const char *path);

/// @brief creates a new cache context of size
/// @param capacity context size
/// @return file cache context pointer
file_cache_t *cache_new(file_cache_t *cache, size_t capacity);

/// @brief free a cache context and all of its entries
/// @param cache cache context
void cache_free(file_cache_t *cache);

/// @brief Clear the cache, reuable after as we dont the file cache itself, just entries.
/// @param cache file cache context
void cache_clear(file_cache_t *cache);

/// @brief gets a file and attempts to read it from cache, if it doesnt, it will read it and cache it
/// @param cache cache context
/// @param path filepath
/// @param size_out size of file
/// @return file data
const char *cache_get_file(file_cache_t *cache, const char *path, size_t *size_out);
