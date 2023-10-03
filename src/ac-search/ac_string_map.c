#include "another-c-library/ac_string_map.h"
#include "another-c-library/ac_map.h"

struct ac_string_map_s {
    ac_pool_t *pool;
    ac_map_t *root;
};

typedef struct {
    ac_map_t node;
    uint32_t offs;
    uint32_t payload;
} string_node_t;

static inline int compare_string_node(const string_node_t *a, const string_node_t *b) {
  return strcmp((char *)(a+1), (char *)(b+1));
}

static inline int compare_string_node_for_find(const char *a, const string_node_t *b) {
  return strcmp(a, (char *)(b+1));
}

static inline ac_map_insert_m(string_node_insert, string_node_t, compare_string_node);
static inline ac_map_find_m(string_node_find, char, string_node_t, compare_string_node_for_find);

ac_string_map_t *ac_string_map_init(ac_pool_t *pool) {
    ac_string_map_t *r = (ac_string_map_t *)ac_pool_calloc(pool, sizeof(*r));
    r->pool = pool;
    return r;
}

void ac_string_map_add_with_payload(ac_string_map_t *h, const char *s, const void *payload, uint32_t len) {
    if(!string_node_find(s, h->root)) {
        string_node_t *n =
            (string_node_t *)ac_pool_calloc(h->pool, sizeof(string_node_t) + strlen(s)+1+len);
        n->payload = len;
        char *p = (char *)(n+1);
        strcpy(p, s);
        if(len > 0) {
            p += strlen(p) + 1;
            memcpy(p, payload, len);
        }
        string_node_insert(n, &h->root);
    }
}

void ac_string_map_add(ac_string_map_t *h, const char *s) {
    ac_string_map_add_with_payload(h, s, NULL, 0);
}

size_t ac_string_map_estimate(ac_string_map_t *h) {
    size_t total = 0;
    ac_map_t *n = ac_map_first(h->root);
    while(n) {
        string_node_t *sn = (string_node_t *)(n);
        const char *s = (const char *)(sn+1);
        total = total + strlen(s) + 1 + sn->payload;
        n = ac_map_next(n);
    }
    return total;
}

void ac_string_map_order(ac_string_map_t *h, ac_buffer_t *bh) {
    ac_map_t *n = ac_map_first(h->root);
    while(n) {
        string_node_t *sn = (string_node_t *)(n);
        const char *s = (const char *)(sn+1);
        sn->offs = ac_buffer_length(bh);
        ac_buffer_append(bh, s, strlen(s)+1+sn->payload);
        n = ac_map_next(n);
    }
}

uint32_t ac_string_map_find(ac_string_map_t *h, const char *s) {
    string_node_t *n = string_node_find(s, h->root);
    if(!n) return 0;
    return n->offs;
}
