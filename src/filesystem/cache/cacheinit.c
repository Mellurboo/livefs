#include <filesystem/cache/filecache.h>

#define FILE_DATA_CACHE_SIZE            128
#define DESCRIPTOR_FILE_CACHE_SIZE      64

void server_cache_init(void){
    cache_new(&file_data_cache, FILE_DATA_CACHE_SIZE);
    cache_new(&directory_descriptor_cache, DESCRIPTOR_FILE_CACHE_SIZE);
}

void server_free_all_cache(void);