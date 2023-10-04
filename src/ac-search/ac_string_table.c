#include "another-c-library/ac_string_table.h"
#include "the-macro-library/macro_map.h"
#include "the-macro-library/macro_bsearch.h"
#include "another-c-library/ac_io.h"
#include <string.h>
#include <inttypes.h>

static inline int compare_string_node(const macro_map_t *a, const macro_map_t *b) {
  return strcmp((char *)(a+1), (char *)(b+1));
}

static inline int compare_string_node_for_find(const char *a, const macro_map_t *b) {
  return strcmp(a, (char *)(b+1));
}

static inline macro_map_insert(string_node_insert, ac_map_t, compare_string_node);
static inline macro_map_find_kv(string_node_find, char, ac_map_t, compare_string_node_for_find);

struct ac_string_table_s {
    char *all_strings;
    uint32_t *offsets;
    size_t num_strings;

    // only needed for id rewrite
    ac_pool_t *pool;
    ac_map_t *root;

    uint32_t map_size;
};


static inline int compare_strings(const char *key, const uint32_t *v, const void *arg) {
    const char *value = (char *)arg + *v;
    // printf( "cmp: %s %s => %d\n", key, value, strcmp(key, value));
    return strcmp(key, value);
}

static inline _macro_bsearch_kv(search_strings, core, cmp_arg, char, uint32_t, compare_strings);

uint32_t ac_string_table_find(ac_string_table_t *h, const char *s) {
    if(!s) return 0;
    uint32_t *res = search_strings(s, h->offsets, h->num_strings, h->all_strings);
    if(!res)
        return 0;

    return res-h->offsets;
}

const char *ac_string_table_lookup(ac_string_table_t *h, uint32_t id) {
    if(id <= h->num_strings)
        return h->all_strings + h->offsets[id];
    return NULL; // shouldn't happen
}

uint32_t ac_string_table_size(ac_string_table_t *h) {
    return h->num_strings;
}

void ac_string_table_add(ac_string_table_t *h, const char *s) {
    if(!s)
        return;

    uint32_t *res = search_strings(s, h->offsets, h->num_strings, h->all_strings);
    if(!res && !string_node_find(h->root, s)) {
        // printf( "adding %s\n", s );
        ac_map_t *n =
            (ac_map_t *)ac_pool_calloc(h->pool, sizeof(ac_map_t) + strlen(s)+1);
        strcpy((char *)(n+1), s);
        string_node_insert(&h->root, n);
        h->map_size++;
    }
}

ac_string_table_t *ac_string_table_init(const char *filename) {
    ac_string_table_t *h = (ac_string_table_t *)ac_calloc(sizeof(*h));
    size_t len = 0;
    char *buffer = (char *)ac_io_read_file(&len, filename);
    if(buffer) {
        h->offsets = (uint32_t *)buffer;
        h->num_strings = h->offsets[0];
        h->offsets[0] = 0;
        h->all_strings = buffer + (sizeof(uint32_t) * h->num_strings);
    }
    return h;
}

void ac_string_table_expand(ac_string_table_t *h) {
    h->pool = ac_pool_init(1024*1024);
    h->root = NULL;
    h->map_size = 0;
    if(!h->num_strings)
        ac_string_table_add(h, "");
}

/* This will merge the offsets/all_strings with the map and returns a mapping
   table to map old ids to new ids.  The string table will remain valid even
   after updating (allowing for old strings to be looked up).

   After saving, create a new string table using the filename to lookup the ids
   and use the mapping table returned from this function to map old ids to new
   ones.

    Slight optimization would be to write out the current state when map_size = 0,
    meaning that no new strings were inserted.  In this case, mapping could be NULL
    or just map each id to itself (0=0, 1=1, 2=2, etc.)
*/
uint32_t *ac_string_table_save(ac_string_table_t *h, const char *filename) {
    // create the mapping table
    uint32_t *mapping = NULL;
    if(h->num_strings)
        mapping = (uint32_t *)ac_calloc(sizeof(uint32_t) * h->num_strings);

    printf( "Adding %u strings to %s\n", h->map_size, filename );

    // could optionally write every offset or write in chunks to save memory (not sure it matters)
    uint32_t *offs = (uint32_t *)ac_calloc(sizeof(uint32_t) * (h->map_size+h->num_strings+1));
    uint32_t *op = offs;
    uint32_t *sp = h->offsets;
    uint32_t *p = sp;
    uint32_t *ep = p+h->num_strings;
    ac_map_t *n = ac_map_first(h->root);
    uint32_t cur_offs = 0;
    char *s;
    // merge old and new
    while(p < ep && n) {
        char *v1 = h->all_strings + *p;
        char *v2 = (char *)(n+1);
        int cmp = strcmp(v1, v2);
        if(cmp != 0) {
            if(cmp < 0) {
                mapping[p-sp] = op-offs;
                s = v1;
                p++;
            }
            else {
                s = v2;
                n = ac_map_next(n);
            }
            *op++ = cur_offs;
            cur_offs += strlen(s) + 1;
        }
        else
            abort();
    }
    // finish old (at this point, old or new will be finished, possibly both)
    while(p < ep) {
        s = h->all_strings + *p;
        mapping[p-sp] = op-offs;
        p++;
        *op++ = cur_offs;
        cur_offs += strlen(s) + 1;
    }
    // finish new
    while(n) {
        s = (char *)(n+1);
        n = ac_map_next(n);
        *op++ = cur_offs;
        cur_offs += strlen(s) + 1;
    }

    offs[0] = h->map_size + h->num_strings;
    if(offs[0] != op-offs)
        abort();

    FILE *out = fopen(filename, "wb");
    fwrite(offs, sizeof(uint32_t) * offs[0], 1, out);
    ac_free(offs);

    p = sp;
    n = ac_map_first(h->root);
    while(p < ep && n) {
        char *v1 = h->all_strings + *p;
        char *v2 = (char *)(n+1);
        int cmp = strcmp(v1, v2);
        if(cmp != 0) {
            if(cmp < 0) {
                s = v1;
                p++;
            }
            else {
                s = v2;
                n = ac_map_next(n);
            }
            fwrite(s, strlen(s)+1, 1, out);
        }
        else
            abort();
    }
    // finish old (at this point, old or new will be finished, possibly both)
    while(p < ep) {
        s = h->all_strings + *p;
        p++;
        fwrite(s, strlen(s)+1, 1, out);
    }
    // finish new
    while(n) {
        s = (char *)(n+1);
        n = ac_map_next(n);
        fwrite(s, strlen(s)+1, 1, out);
    }
    fclose(out);
    ac_pool_destroy(h->pool);
    h->pool = NULL;
    h->root = NULL;
    h->map_size = 0;
    return mapping;
}

void ac_string_table_destroy(ac_string_table_t *h) {
    if(!h) return;
    if(h->offsets)
        ac_free(h->offsets);
    if(h->pool)
        ac_free(h->pool);
    ac_free(h);
}
