#include "another-c-library/ac-search/ac_cursor.h"

#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_pool.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static
uint32_t advance_empty_to(void *c, uint32_t id)
{
    return 0;
}

static
uint32_t advance_empty(void *c)
{
    return 0;
}

ac_cursor_t *ac_cursor_init_empty(ac_pool_t *pool) {
    ac_cursor_t *r = (ac_cursor_t *)ac_pool_calloc(pool, sizeof(ac_cursor_t));
    r->advance = advance_empty;
    r->advance_to = advance_empty_to;
    r->type = EMPTY_CURSOR;
    return r;
}


struct or_cursor_s;
typedef struct or_cursor_s or_cursor_t;

struct or_cursor_s {
    ac_cursor_t cursor;
    ac_cursor_t **cursors;
    uint32_t num_cursors;
    uint32_t cursor_size;

    ac_cursor_t **active;
    ac_cursor_t **heap;
    uint32_t num_active;
    uint32_t num_heap;
};

static
void or_add( or_cursor_t *dest, ac_cursor_t *src ) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    if(dest->num_cursors >= dest->cursor_size) {
        uint32_t num_cursors = (dest->cursor_size+1)*2;
        ac_cursor_t **cursors = 
            (ac_cursor_t **)ac_pool_alloc(dest->cursor.pool, sizeof(ac_cursor_t *) * num_cursors);
        if(dest->num_cursors)
            memcpy(cursors, dest->cursors, dest->num_cursors * sizeof(ac_cursor_t *));
        dest->cursor_size = num_cursors;
        dest->cursors = cursors;
    }
    dest->cursors[dest->num_cursors] = src;
    dest->num_cursors++;
}

static
void or_push(or_cursor_t *c, ac_cursor_t *src) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );
    // ac_cursor_reset(src);
    ac_cursor_t *tmp, **heap = c->heap;

    c->num_heap++;
    uint32_t i=c->num_heap;
    heap[i] = src;
    uint32_t j=i>>1;

    while (j > 0 && heap[i]->current < heap[j]->current) {
        tmp = heap[i];
        heap[i] = heap[j];
        heap[j] = tmp;
        i = j;
        j = j >> 1;
    }
}

static
ac_cursor_t *or_pop(or_cursor_t *c) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    c->num_heap--;
    ssize_t num = c->num_heap;
    ac_cursor_t **heap = c->heap;
    ac_cursor_t *r = heap[1];
    heap[1] = heap[num + 1];

    ssize_t i = 1;
    ssize_t j = i << 1;
    ssize_t k = j + 1;

    if (k <= num && heap[k]->current < heap[j]->current)
        j = k;

    while (j <= num && heap[j]->current < heap[i]->current) {
        ac_cursor_t *tmp = heap[i];
        heap[i] = heap[j];
        heap[j] = tmp;

        i = j;
        j = i << 1;
        k = j + 1;
        if (k <= num && heap[k]->current < heap[j]->current)
            j = k;
    }
    return r;
}

static
uint32_t advance_or(or_cursor_t *c) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    for( uint32_t i=0; i<c->num_active; i++ ) {
        if(c->active[i]->advance(c->active[i]))
            or_push(c, c->active[i]);
    }
    if(!c->num_heap)
        return ac_cursor_empty(&c->cursor);

    c->active[0] = or_pop(c);
    c->num_active = 1;

    ac_cursor_t **heap = c->heap;
    uint32_t current = c->active[0]->current;
    while(c->num_heap && heap[1]->current == current) {
        c->active[c->num_active] = or_pop(c);
        c->num_active++;
    }

    c->cursor.current = current;
    return current;
}

static
uint32_t advance_or_to(or_cursor_t *c, uint32_t id) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    if (id <= c->cursor.current)
        return c->cursor.current;

    bool pushed = false;
    for( uint32_t i=0; i<c->num_active; i++ ) {
        if(c->active[i]->advance_to(c->active[i], id)) {
            or_push(c, c->active[i]);
            pushed = true;
        }
    }
    ac_cursor_t *p;
    if(!pushed) {
        if(!c->num_heap)
            return ac_cursor_empty(&c->cursor);

        ac_cursor_t *p = or_pop(c);
        while(!p->advance_to(p, id)) {
            if(!c->num_heap)
                return ac_cursor_empty(&c->cursor);
            p = or_pop(c);
        }
    }
    else
        p = or_pop(c);

    c->active[0] = p;
    c->num_active = 1;

    ac_cursor_t **heap = c->heap;
    uint32_t current = p->current;
    while(c->num_heap && heap[1]->current == current) {
        c->active[c->num_active] = or_pop(c);
        c->num_active++;
    }

    c->cursor.current = current;
    return current;
}

static
uint32_t advance_or_init(or_cursor_t *c) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    uint32_t num = c->num_cursors;

    c->heap = (ac_cursor_t **)ac_pool_alloc(c->cursor.pool, sizeof(ac_cursor_t *) * (num+1) * 2);
    c->active = c->heap + (num+1);
    c->num_heap = 0;
    c->num_active = 0;

    for( uint32_t k=0; k<c->num_cursors; k++ ) {
        if(c->cursors[k]->advance(c->cursors[k])) {
            // ac_cursor_reset(c->cursors[k]);
            or_push(c, c->cursors[k]);
        }
    }
    
    if(!c->num_heap)
        return ac_cursor_empty(&c->cursor);

    c->cursor.advance = (ac_cursor_advance_cb)advance_or;
    c->cursor.advance_to = (ac_cursor_advance_to_cb)advance_or_to;

    return advance_or(c);
}

static
uint32_t advance_or_to_init(or_cursor_t *c, uint32_t id) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    uint32_t r=advance_or_init(c);
    if(r >= id)
        return r;
    if(!r)
         return ac_cursor_empty(&c->cursor);

    return c->cursor.advance_to(c, id);
}    

static
void init_or(ac_pool_t *pool, or_cursor_t *r) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    uint32_t num_cursors = 2;
    r->cursor.pool = pool;
    r->cursor.type = OR_CURSOR;
    r->cursor.advance = (ac_cursor_advance_cb)advance_or_init;
    r->cursor.advance_to = (ac_cursor_advance_to_cb)advance_or_to_init;
    r->cursor.add = (ac_cursor_add_cb)or_add;
    r->cursors = (ac_cursor_t **)ac_pool_alloc(pool, sizeof(ac_cursor_t *) * num_cursors);
    r->num_cursors = 0;
    r->cursor_size = num_cursors;
}

ac_cursor_t *ac_cursor_init_or(ac_pool_t *pool) {
    // printf( "%s, %u\n", __FUNCTION__, __LINE__ );

    or_cursor_t *r = (or_cursor_t *)ac_pool_calloc(pool, sizeof(or_cursor_t));
    init_or(pool, r);
    return (ac_cursor_t *)r;
}

struct not_cursor_s;
typedef struct not_cursor_s not_cursor_t;

struct not_cursor_s {
    ac_cursor_t cursor;
    ac_cursor_t *pos;
    ac_cursor_t *neg;
};

static
uint32_t advance_pos(not_cursor_t *c) {
    uint32_t id=c->pos->advance(c->pos);
    c->cursor.current=id;
    if(id)
        return id;
    return ac_cursor_empty(&c->cursor);
}

static
uint32_t advance_pos_to(not_cursor_t *c, uint32_t _id) {
    if (_id <= c->cursor.current)
        return c->cursor.current;

    uint32_t id=c->pos->advance_to(c->pos, _id);
    c->cursor.current=id;
    if(id)
        return id;
    return ac_cursor_empty(&c->cursor);
}

static
uint32_t advance_not(not_cursor_t *c)
{
    while(true) {
        uint32_t id=c->pos->advance(c->pos);
        if(!id)
            return ac_cursor_empty(&c->cursor);
        uint32_t id2=c->neg->advance_to(c->neg, id);
        if(id==id2)
            continue;
        c->cursor.current = id;
        if(id2)
            return id;
        c->cursor.advance_to = (ac_cursor_advance_to_cb)advance_pos_to;
        c->cursor.advance = (ac_cursor_advance_cb)advance_pos;
        return id;        
    }
}

static
uint32_t advance_not_to(not_cursor_t *c, uint32_t _id)
{
    if (_id <= c->cursor.current)
        return c->cursor.current;

    uint32_t id=c->pos->advance_to(c->pos, _id);
    if(!id)
        return ac_cursor_empty(&c->cursor);
    uint32_t id2=c->neg->advance_to(c->neg, id);
    if(id==id2)
        return advance_not(c);

    c->cursor.current = id;
    if(id2)
        return id;
    c->cursor.advance_to = (ac_cursor_advance_to_cb)advance_pos_to;
    c->cursor.advance = (ac_cursor_advance_cb)advance_pos;
    return id;
}

ac_cursor_t *ac_cursor_init_not(ac_pool_t *pool,
                                ac_cursor_t *pos,
                                ac_cursor_t *neg ) {
    if(!pos)
        return ac_cursor_init_empty(pool);
    else if(!neg)
        return pos;
    not_cursor_t *r = (not_cursor_t *)ac_pool_calloc(pool, sizeof(not_cursor_t));
    r->cursor.pool = pool;
    r->cursor.type = NOT_CURSOR;
    r->cursor.advance = (ac_cursor_advance_cb)advance_not;
    r->cursor.advance_to = (ac_cursor_advance_to_cb)advance_not_to;
    r->pos = pos;
    r->neg = neg;
    return (ac_cursor_t *)r;
}

struct and_cursor_s;
typedef struct and_cursor_s and_cursor_t;

struct and_cursor_s {
    ac_cursor_t cursor;
    ac_cursor_t **cursors;
    uint32_t num_cursors;
    uint32_t cursor_size;
};

static
void and_add( and_cursor_t *dest, ac_cursor_t *src ) {
    if(dest->num_cursors >= dest->cursor_size) {
        uint32_t num_cursors = (dest->cursor_size+1)*2;
        ac_cursor_t **cursors = 
            (ac_cursor_t **)ac_pool_alloc(dest->cursor.pool, sizeof(ac_cursor_t *) * num_cursors);
        if(dest->num_cursors)
            memcpy(cursors, dest->cursors, dest->num_cursors * sizeof(ac_cursor_t *));
        dest->cursor_size = num_cursors;
        dest->cursors = cursors;
    }
    dest->cursors[dest->num_cursors] = src;
    dest->num_cursors++;
}

static
uint32_t advance_and(and_cursor_t *c)
{
    uint32_t i=1;
    uint32_t id = c->cursors[0]->advance(c->cursors[0]);
    while(id && i < c->num_cursors) {
        uint32_t id2 = c->cursors[i]->advance_to(c->cursors[i], id);
        if(id==id2)
            i++;
        else {
            i=0;
            id=id2;
        }
    }
    if(!id)
        return ac_cursor_empty(&c->cursor);
    c->cursor.current = id;
    return id;
}

static
uint32_t advance_and_to(and_cursor_t *c, uint32_t id)
{
    if (id <= c->cursor.current)
        return c->cursor.current;

    uint32_t i=0;
    while(id && i < c->num_cursors) {
        uint32_t id2 = c->cursors[i]->advance_to(c->cursors[i], id);
        if(id==id2)
            i++;
        else {
            i=0;
            id=id2;
        }
    }
    if(!id)
        return ac_cursor_empty(&c->cursor);

    c->cursor.current = id;
    return id;
}

ac_cursor_t *ac_cursor_init_and(ac_pool_t *pool) {
    uint32_t num_cursors = 2;
    and_cursor_t *r = (and_cursor_t *)ac_pool_calloc(pool, sizeof(and_cursor_t));
    r->cursor.pool = pool;
    r->cursor.type = AND_CURSOR;
    r->cursor.advance = (ac_cursor_advance_cb)advance_and;
    r->cursor.advance_to = (ac_cursor_advance_to_cb)advance_and_to;
    r->cursor.add = (ac_cursor_add_cb)and_add;
    r->cursors = (ac_cursor_t **)ac_pool_alloc(pool, sizeof(ac_cursor_t *) * num_cursors);
    r->num_cursors = 0;
    r->cursor_size = num_cursors;
    return (ac_cursor_t *)r;
}

uint32_t ac_cursor_empty(ac_cursor_t *c) {
    c->advance_to = advance_empty_to;
    c->advance = advance_empty;
    c->current = 0;
    return 0;
}

static
uint32_t post_reset_advance(ac_cursor_t *c) {
    c->advance = c->_advance;
    return c->current;
}

void ac_cursor_reset(ac_cursor_t *c) {
    c->_advance = c->advance;
    c->advance = (ac_cursor_advance_cb)post_reset_advance;
}

struct range_cursor_s;
typedef struct range_cursor_s range_cursor_t;

struct range_cursor_s {
    ac_cursor_t cursor;
    uint32_t current;
    uint32_t end;
};

static
uint32_t advance_range(range_cursor_t *c)
{
    uint32_t id = c->current;
    if(id >= c->end)
        return ac_cursor_empty(&c->cursor);
    c->cursor.current = id;
    c->current++;
    return id;
}

static
uint32_t advance_range_to(range_cursor_t *c, uint32_t id)
{
    if(id <= c->cursor.current)
        return c->cursor.current;

    if(id >= c->end)
        return ac_cursor_empty(&c->cursor);
    c->cursor.current = id;
    c->current = id+1;
    return id;
}

ac_cursor_t *ac_cursor_range(ac_pool_t *pool, uint32_t start, uint32_t end) {
    range_cursor_t *r = (range_cursor_t *)ac_pool_calloc(pool, sizeof(range_cursor_t));
    r->current = start+1;
    r->end = end;
    r->cursor.current = start;
    r->cursor._advance = (ac_cursor_advance_cb)advance_range;
    r->cursor.advance_to = (ac_cursor_advance_to_cb)advance_range_to;
    r->cursor.advance = (ac_cursor_advance_cb)post_reset_advance;
    r->cursor.type = NORMAL_CURSOR;
    return &(r->cursor);
}


ac_cursor_t *ac_cursor_init_id(ac_pool_t *pool, uint32_t id) {
    ac_cursor_t *r = (ac_cursor_t *)ac_pool_calloc(pool, sizeof(ac_cursor_t));
    r->current = id;
    r->_advance = advance_empty;
    r->advance_to = advance_empty_to;
    r->advance = (ac_cursor_advance_cb)post_reset_advance;
    r->type = NORMAL_CURSOR;
    return r;
}

ac_cursor_t ** ac_cursor_subs(ac_cursor_t *c, uint32_t *num_sub ) {
    if(c->type == AND_CURSOR || c->type == PHRASE_CURSOR) {
        and_cursor_t *r = (and_cursor_t *)c;
        *num_sub = r->num_cursors;
        return r->cursors;
    }
    else if(c->type == OR_CURSOR) {
        or_cursor_t *r = (or_cursor_t *)c;
        *num_sub = r->num_active;
        return r->active;
    }
    else if(c->type == NOT_CURSOR) {
        not_cursor_t *r = (not_cursor_t *)c;
        *num_sub = 1;
        return &(r->pos);
    }
    *num_sub = 0;
    return NULL;
}

static
ac_cursor_t *_ac_cursor_open(ac_pool_t *pool, ac_cursor_custom_cb cb, ac_token_t *t, void *arg) {
    if(t->child) {
        if(t->type == AC_TOKEN_OPEN_PAREN || t->type == AC_TOKEN_DQUOTE) {
            ac_cursor_t *resp = ac_cursor_init_and(pool);
            if(t->type == AC_TOKEN_DQUOTE)
                resp->type = PHRASE_CURSOR;
            ac_token_t *n = t->child;
            while(n) {
                ac_cursor_t *c = _ac_cursor_open(pool, cb, n, arg);
                if(c && c->type != EMPTY_CURSOR)
                    resp->add(resp, c);
                else
                    return NULL;
                n = n->next;
            }
            return resp;
        }
        else if(t->type == AC_TOKEN_OR) {
            ac_cursor_t *resp = ac_cursor_init_or(pool);
            ac_token_t *n = t->child;
            while(n) {
                ac_cursor_t *c = _ac_cursor_open(pool, cb, n, arg);
                if(c)
                    resp->add(resp, c);
                n = n->next;
            }
            return resp;
        }
        else if(t->type == AC_TOKEN_NOT) {
            if(t->child && t->child->next) {
                return ac_cursor_init_not(pool,
                                          _ac_cursor_open(pool, cb, t->child->next, arg),
                                          _ac_cursor_open(pool, cb, t->child, arg));
            }
        }
        return NULL;
    }
    else
        return cb(pool, t, arg);
}

ac_cursor_t *ac_cursor_open(ac_pool_t *pool, ac_cursor_custom_cb cb, ac_token_t *t, void *arg) {
    if(!t)
        return ac_cursor_init_empty(pool);
        
    ac_cursor_t *c = _ac_cursor_open(pool, cb, t, arg);
    if(!c)
        return ac_cursor_init_empty(pool);
    return c;
}

uint32_t ac_cursor_first(ac_pool_t *pool, ac_cursor_custom_cb cb, ac_token_t *t, void *arg) {
    ac_cursor_t *c = ac_cursor_open(pool, cb, t, arg);
    if(!c)
        return 0;
    return c->advance(c);
}
