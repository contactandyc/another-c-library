#include "ac_bit_set.h"

struct ac_bit_set_s {
    uint64_t *items;
    uint64_t *ep;
    uint64_t last_mask;
};

void ac_bit_set_not(ac_bit_set_t *dest, ac_bit_set_t *to_not) {
    uint64_t *p = dest->items;
    uint64_t *p2 = to_not->items;
    uint64_t *ep = dest->ep;
    while(p < ep) {
        *p = *p & (~*p2);
        p++;
        p2++;
    }
}

void ac_bit_set_or(ac_bit_set_t *dest, ac_bit_set_t *to_or) {
    uint64_t *p = dest->items;
    uint64_t *p2 = to_or->items;
    uint64_t *ep = dest->ep;
    while(p < ep) {
        *p = *p | *p2;
        p++;
        p2++;
    }
}

void ac_bit_set_and(ac_bit_set_t *dest, ac_bit_set_t *to_and) {
    uint64_t *p = dest->items;
    uint64_t *p2 = to_and->items;
    uint64_t *ep = dest->ep;
    while(p < ep) {
        *p = *p & *p2;
        p++;
        p2++;
    }
}

void ac_bit_set_false(ac_bit_set_t *h) {
    uint64_t *p = h->items;
    uint64_t *ep = h->ep;
    while(p < ep)
        *p++ = 0;
}

void ac_bit_set_true(ac_bit_set_t *h) {
    uint64_t *p = h->items;
    uint64_t *ep = h->ep;
    while(p < ep)
        *p++ = 0xFFFFFFFFFFFFFFFFULL;
    if(h->items < h->ep)
        ep[-1] = ep[-1] & h->last_mask;
}

void ac_bit_set_complement(ac_bit_set_t *h) {
    uint64_t *p = h->items;
    uint64_t *ep = h->ep;
    while(p < ep) {
        *p = ~(*p);
        p++;
    }
    if(h->items < h->ep)
        ep[-1] = ep[-1] & h->last_mask;
}

ac_bit_set_t *ac_bit_set_copy(ac_pool_t *pool, ac_bit_set_t *src) {
    ac_bit_set_t *h = (ac_bit_set_t *)ac_pool_calloc(pool, sizeof(*h));
    h->items = (uint64_t *)ac_pool_dup(pool, src->items, sizeof(uint64_t) * (src->ep-src->items));
    h->ep = h->items + (src->ep-src->items);
    return h;
}

ac_bit_set_t * ac_bit_set_init(ac_pool_t *pool, uint32_t num_items) {
    uint32_t n=num_items & 63;
    uint64_t mask = 0;
    while(n) {
        mask = (mask << 1ULL)+1;
        n--;
    }

    num_items = (num_items >> 6)+1;
    ac_bit_set_t *h = (ac_bit_set_t *)ac_pool_calloc(pool, sizeof(*h));
    h->items = (uint64_t *)ac_pool_calloc(pool, sizeof(uint64_t) * num_items);
    h->ep = h->items + num_items;
    h->last_mask = mask;
    return h;
}

void ac_bit_set_set(ac_bit_set_t *h, uint32_t id) {
    uint32_t block = id >> 6;
    uint32_t mask = id & 63;
    uint64_t *p = h->items + block;
    uint64_t *ep = h->ep;
    if(p >= ep)
        return;
    *p |= (1ULL<<mask);
}

void ac_bit_set_unset(ac_bit_set_t *h, uint32_t id) {
    uint32_t block = id >> 6ULL;
    uint32_t mask = id & 63ULL;
    uint64_t *p = h->items + block;
    uint64_t *ep = h->ep;
    if(p >= ep)
        return;
    *p &= ~(1ULL<<mask);
}

void ac_bit_set_boolean(ac_bit_set_t *h, uint32_t id, bool v) {
    if(v)
        ac_bit_set_set(h, id);
    else
        ac_bit_set_unset(h, id);
}

uint32_t ac_bit_set_count(ac_bit_set_t *h) {
    uint64_t *p = h->items;
    uint64_t *ep = h->ep;
    unsigned int count = 0;
    while(p < ep) {
        uint64_t n = *p++;
        // Brian Kernighan’s Algorithm
        while (n) {
            n &= (n - 1);
            count++;
        }
    }
    return count;
}

uint32_t ac_bit_set_count_and_zero(ac_bit_set_t *h) {
    uint64_t *p = h->items;
    uint64_t *ep = h->ep;
    unsigned int count = 0;
    while(p < ep) {
        uint64_t n = *p;
        if(n) {
            // Brian Kernighan’s Algorithm
            while (n) {
                n &= (n - 1);
                count++;
            }
            *p = 0;
        }
        p++;
    }
    return count;
}

bool ac_bit_set(ac_bit_set_t *h, uint32_t id) {
    uint32_t block = id >> 6;
    uint32_t mask = id & 63;
    uint64_t *p = h->items + block;
    uint64_t *ep = h->ep;
    if(p >= ep)
        return false;
    return (*p & (1ULL<<mask)) != 0 ? true : false;
}

struct ac_bit_set_cursor_s;
typedef struct ac_bit_set_cursor_s ac_bit_set_cursor_t;

struct ac_bit_set_cursor_s {
    ac_cursor_t cursor;
    ac_bit_set_t *bit_set;
};

uint32_t ac_bit_set_first(ac_bit_set_t *h) {
    uint64_t *p = h->items;
    uint64_t *ep = h->ep;
    while(p < ep) {
        uint64_t n = *p;
        if(n) {
            uint32_t id = (p-h->items)<<6;
            while((n & 1) == 0) {
                n >>= 1ULL;
                id++;
            }
            return id;
        }
        p++;
    }
    return 0;
}

static
uint32_t advance_bit_set(ac_bit_set_cursor_t *c)
{
    uint32_t id = c->cursor.current;
    id++;
    uint32_t block = id >> 6;
    uint32_t mask = id & 63;
    ac_bit_set_t *h = c->bit_set;
    uint64_t *p = h->items + block;
    uint64_t *ep = h->ep;
    if(p >= ep)
        return ac_cursor_empty(&c->cursor);
    uint64_t item = *p >> mask;
    if(item) {
        while((item & 0x1) == 0 && mask < 63) {
            mask++;
            id++;
            item >>= 1ULL;
        }
        c->cursor.current = id;
        return id;
    }
    id += 64-mask;
    p++;
    while(p < ep && *p == 0) {
        id += 64;
        p++;
    }
    if(p == ep)
        return ac_cursor_empty(&c->cursor);
    mask = 0;
    item = *p;
    while((item & 0x1) == 0 && mask < 63) {
        mask++;
        id++;
        item >>= 1ULL;
    }
    c->cursor.current = id;
    return id;
}

static
uint32_t advance_bit_set_to( ac_bit_set_cursor_t *c, uint32_t id)
{
    if(c->cursor.current >= id)
        return c->cursor.current;

    c->cursor.current = id;
    if(ac_bit_set(c->bit_set, id))
        return id;
    return advance_bit_set(c);
}

ac_cursor_t *ac_bit_set_open_cursor(ac_pool_t *pool, ac_bit_set_t *h) {
    ac_bit_set_cursor_t *r = ac_pool_calloc(pool, sizeof(*r));
    ac_cursor_t *c = (ac_cursor_t *)r;
    r->bit_set = h;
    c->pool = pool;
    c->type = NORMAL_CURSOR;
    c->advance = (ac_cursor_advance_cb)advance_bit_set;
    c->advance_to = (ac_cursor_advance_to_cb)advance_bit_set_to;
    c->advance(c);
    ac_cursor_reset(c);
    return c;
}
