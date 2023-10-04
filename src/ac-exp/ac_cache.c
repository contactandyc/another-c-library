#include "another-c-library/ac_cache.h"
#include "another-c-library/ac_map.h"

struct ac_cache_free_node_s;
typedef struct ac_cache_free_node_s ac_cache_free_node_t;

struct ac_cache_mutex_s;
typedef struct ac_cache_mutex_s ac_cache_mutex_t;

struct ac_cache_free_node_s {
    ac_cache_free_node_t *next;
};

struct ac_cache_mutex_s {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

typedef struct {
    ac_map_t time_stamp_node;
    ac_map_t key_node;
    uint64_t key;
    size_t bytes;
    void *data;
    time_t time_stamp;
    uint32_t hits;
    uint32_t seconds;
    ac_cache_mutex_t *fetching;
} ac_cache_item_t;

static inline
int compare_time_stamp_for_insert(const ac_cache_item_t *a, const ac_cache_item_t *b) {
    if(a->time_stamp != b->time_stamp)
        return (a->time_stamp < b->time_stamp) ? -1 : 1;
    if(*key != node->key)
        return (*key < node->key) ? -1 : 1;
    return 0;
}

static ac_map_find_m(_time_stamp_find, ac_cache_item_t, ac_cache_item_t, compare_time_stamp_for_insert);
static ac_map_insert_m(_time_stamp_insert, ac_cache_item_t, compare_time_stamp_for_insert);

static inline
int compare_key_for_find(const uint64_t *key, const ac_cache_item_t *node) {
    if(*key != node->key)
        return (*key < node->key) ? -1 : 1;
    return 0;
}

static inline
int compare_key_for_insert(const ac_cache_item_t *a, const ac_cache_item_t *b) {
    if(a->key != b->key)
        return (a->key < b->key) ? -1 : 1;
    return 0;
}

static ac_map_find2_m(_key_find, uint64_t, ac_cache_item_t, key_node, compare_key_for_find);
static ac_map_insert2_m(_key_insert, ac_cache_item_t, key_node, compare_key_for_insert);

struct ac_cache_s {
    ac_map_t *key_root;
    ac_map_t *time_stamp_root;
    ac_cache_free_node_t *free_item_nodes;
    ac_cache_free_node_t *free_mutex_nodes;
    size_t bytes;
    size_t cap;
    pthread_mutex_t mutex;
};

ac_cache_t * ac_cache_init(size_t cap) {
    ac_cache_t *h = (ac_cache_t *)ac_calloc(sizeof(*h));
    pthread_mutex_init(&h->mutex);
    h->cap = cap;
    return h;
}

uint64_t ac_cache_key(const char *key) {
    return ac_md5_str(key);
}

/*
    checks if data is available in cache
*/
const void * cache_get(size_t *len, ac_cache_t *h, uint64_t cache_key) {
    pthread_mutex_lock(&h->mutex);
    ac_cache_item_t *item = _key_find(cache_key, h->key_root);
    if(!item || item->fetching) {
        *len = 0;
        return NULL;
    }
    len = item->bytes;
    return item->data;
}

/*
    starts a cache operation.  If the given key is already started, this will
    wait on the previously started version
*/
const void * cache_start(ssize_t *len, ac_cache_t *h, uint64_t cache_key) {
    pthread_mutex_lock(&h->mutex);
    ac_cache_item_t *item = _key_find(&cache_key, h->key_root);
    if(!item) {
        // create a new item and start process
        ac_cache_free_node_t *fn = h->free_item_nodes;
        if(fn) {
            item = (ac_cache_item_t *)fn;
            h->free_item_nodes = fn->next;
            memset(item, 0, sizeof(*item));
        }
        else
            item = (ac_cache_item_t *)ac_pool_calloc(h->cache_pool, sizeof(*item));
        item->key = cache_key;
        _key_insert(item, &(h->key_root));
        fn = h->free_mutex_nodes;
        if(fn) {
            item->fetching = (ac_cache_mutex_t *)fn;
            h->free_mutex_nodes = fn->next;
        }
        else
            item->fetching = (ac_cache_mutex_t *)ac_pool_calloc(h->cache_pool, sizeof(*(item->fetching)));
        item->fetching->mutex = PTHREAD_MUTEX_INITIALIZER;
        item->fetching->cond = PTHREAD_COND_INITIALIZER;
        item->fetching->ref_count = 1;
        item->fetching->complete = 0;
        *len = -1;
        return item;
    }
    else if(item->fetching && item->fetching->ref_count) {
        // wait on item to be fetched and return
        ac_cache_mutex_t *fetching = item->fetching;

        pthread_mutex_lock(&fetching->mutex);
        pthread_mutex_unlock(&h->mutex);
        while(fetching->complete == 0) {
            pthread_cond_wait(&fetching->cond, &fetching->mutex);
        }
        pthread_mutex_lock(&h->mutex);
        fetching->ref_count--;
        if(fetching->ref_count == 0) {
            pthread_mutex_unlock(&fetching->mutex);
            item->fetching = NULL;
            ac_cache_free_node_t *fn = (ac_cache_free_node_t *)fetching;
            fn->next = h->free_mutex_nodes;
            h->free_mutex_nodes = fn;
        }
        else
            pthread_mutex_unlock(&fetching->mutex);
    }
    len = item->bytes;
    return item->data;
}

/*
    call after cache_get/cache_start to unlock the cache key
*/
void cache_unlock(ac_cache_t *h, uint64_t cache_key) {
    pthread_mutex_unlock(&h->mutex);
}

/*
    ends a cache operation.  If caching a NULL entry, pass a non-NULL/zero len
    to indicate that the key was not found
*/
void cache_end(ac_cache_t *h, uint64_t cache_key, const void *d, size_t len, uint32_t seconds) {
    pthread_mutex_lock(&h->mutex);
    ac_cache_item_t *item = _key_find(cache_key, h->key_root);
    if(!item) {
        ac_cache_free_node_t *fn = h->free_item_nodes;
        if(fn) {
            item = (ac_cache_item_t *)fn;
            h->free_item_nodes = fn->next;
            memset(item, 0, sizeof(*item));
        }
        else
            item = (ac_cache_item_t *)ac_pool_calloc(h->cache_pool, sizeof(*item));
        item->data = d;
        item->bytes = len;
        item->key = cache_key;
        item->time_stamp = time(NULL) + seconds;
        item->seconds = seconds;
        _time_stamp_insert(item, &(h->time_stamp_root));
        _key_insert(item, &(h->key_root));
        pthread_mutex_unlock(&h->mutex);
        return;
    }

    ac_cache_mutex_t *fetching = item->fetching;
    pthread_mutex_lock(&fetching->mutex);
    pthread_mutex_unlock(&h->mutex);
    fetching->ref_count--;
    if(fetching->ref_count) {
        pthread_cond_broadcast(&fetching->cond);
        pthread_mutex_unlock(&fetching->mutex);
        pthread_mutex_lock(&h->mutex);
    }
    else {
        pthread_mutex_unlock(&fetching->mutex);
        pthread_mutex_lock(&h->mutex);
        item->fetching = NULL;
        ac_cache_free_node_t *fn = (ac_cache_free_node_t *)fetching;
        fn->next = h->free_mutex_nodes;
        h->free_mutex_nodes = fn;
    }
    if(item->time_stamp)
        ac_map_erase(&(item->time_stamp_node), &(h->time_stamp_root));
    item->data = d;
    item->bytes = len;
    item->key = cache_key;
    item->time_stamp = time(NULL) + seconds;
    item->seconds = seconds;
    _time_stamp_insert(item, &(h->time_stamp_root));
    pthread_mutex_unlock(&h->mutex);
}

void ac_cache_destroy(ac_cache_t *h) {
    ac_free(h);
}
