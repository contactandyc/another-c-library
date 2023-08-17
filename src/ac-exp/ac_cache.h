#ifndef _ac_cache_h
#define _ac_cache_h

#include <time.h>

struct ac_cache_s;
typedef struct ac_cache_s ac_cache_t;

ac_cache_t * ac_cache_init();

/*
    returns a key to be used with the cached data
*/
uint64_t ac_cache_key(const char *key);

/*
    checks if data is available in cache
*/
const void * cache_get(size_t *len, ac_cache_t *h, uint64_t cache_key);
/*
    starts a cache operation.  If the given key is already started, this will
    wait on the previously started version
*/
const void * cache_start(size_t *len, ac_cache_t *h, uint64_t cache_key);

/*
    call after cache_get/cache_start to unlock the cache key
*/
void cache_unlock(ac_cache_t *h, uint64_t cache_key);

/*
    ends a cache operation.  If caching a NULL entry, pass a non-NULL/zero len
    to indicate that the key was not found
*/
void cache_end(ac_cache_t *h, uint64_t cache_key, const void *d, size_t len);

void ac_cache_destroy(ac_cache_t *h);

#endif