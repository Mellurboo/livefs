#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vendor/gt/gt.h>
#include <utils/terminal.h>
#include <filesystem/filepath.h>
#include <filesystem/read_file.h>
#include <filesystem/cache/filecache.h>

file_cache_t file_data_cache;
file_cache_t directory_descriptor_cache;

/// @brief find file_cache_entry in file_cache context
/// @param cache cache context
/// @param path path of file
/// @return file_cache_entry index of filepath if cached, -1 indicates failure
static ssize_t cache_find_entry(file_cache_t *cache, const char *path){
    for (size_t i = 0; i < cache->count; ++i){
        if (strcmp(cache->entries[i].path, path) == 0){
            return (ssize_t)i;
        }
    }

    return -1;
}

/// @brief creates a new cache context of size
/// @param capacity context size
/// @return file cache context pointer
void cache_new(file_cache_t *cache, size_t capacity){
    memset(cache, 0, sizeof(*cache));
    cache->entries = calloc(capacity, sizeof(file_cache_entry_t));
    if (!cache->entries) {
        fprintf(stderr, "FATAL: failed to allocate cache entries\n");
        exit(1);
    }
    cache->capacity = capacity;
    gtmutex_init(&cache->threadlock);
}

/// @brief free a cache context and all of its entries
/// @param cache cache context
void cache_free(file_cache_t *cache){
    if (!cache){
        printf(ERROR "Failed to free cache, there is no cache allocated here\n");
        return;
    }

    gtmutex_lock(&cache->threadlock);
    for(size_t i = 0; i < cache->count; ++i){
        free(cache->entries[i].path);
        free(cache->entries[i].data);
    }

    free(cache->entries);
    gtmutex_unlock(&cache->threadlock);
    free(cache);
}

/// @brief Clear the cache, reuable after as we dont the file cache itself, just entries.
/// @param cache file cache context
void cache_clear(file_cache_t *cache){
    if (!cache) return;
    gtmutex_lock(&cache->threadlock);
    for (size_t i = 0; i < cache->count; ++i){
        free(cache->entries[i].path);
        free(cache->entries[i].data);
    }
    cache->count = 0;
    gtmutex_unlock(&cache->threadlock);
}

/// @brief gets a file and attempts to read it from cache, if it doesnt, it will read it and cache it
/// @param cache cache context
/// @param path filepath
/// @param size_out size of file
/// @return file data
const char *cache_get_file(file_cache_t *cache, const char *path, size_t *size_out){
    // we can use path_exsists here but we need some other stat functions so we will stat it
    struct stat st;
    if (stat(path, &st) != 0) return NULL;
    gtmutex_lock(&cache->threadlock);

    ssize_t index = cache_find_entry(cache, path);
    if (index != -1){
        file_cache_entry_t *entry = &cache->entries[index];
        if (entry->last_modified == st.st_mtime){
            *size_out = entry->size;
            gtmutex_unlock(&cache->threadlock);
            printf(INFO "Found cache for file '%s' at cache index [%li]\n", entry->path, cache->count);
            return entry->data;
        }

        // the file has changed. so we need to evict that entry
        free(entry->data);
        free(entry->path);
        memmove(&cache->entries[index], &cache->entries[index+1], (cache->count - index - 1) * sizeof(file_cache_entry_t));
    }

    gtmutex_unlock(&cache->threadlock);

    //TODO: Move this over to a different usermode thread as its blocking
    FILE *fp = open_file(path);
    if (!fp) return NULL;

    size_t filesize = get_filesize(fp);

    char *file = read_file(fp);
    if (!file) return NULL;
    // End blocking code

    gtmutex_lock(&cache->threadlock);
    if (cache->count >= cache->capacity){
        // no more space, evict oldest. this actually isnt perfect but should be fine for now
        free(cache->entries[0].path);
        free(cache->entries[0].data);
        memmove(&cache->entries[0], &cache->entries[1],
            (cache->count - 1) * sizeof(file_cache_entry_t));
        cache->count--;
    }

    file_cache_entry_t *entry = &cache->entries[cache->count++];
    entry->path = strdup(path);
    entry->data = file;
    entry->size = filesize;
    entry->last_modified = st.st_mtime;

    *size_out = entry->size;
    printf(INFO "Created cache for file '%s' at cache index [%li] (%zu bytes)\n", entry->path, cache->count, filesize);
    const char *retdata = entry->data;
    gtmutex_unlock(&cache->threadlock);
    return retdata;
}