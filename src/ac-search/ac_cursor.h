#ifndef _ac_cursor_h
#define _ac_cursor_h

#include <inttypes.h>
#include "another-c-library/ac_pool.h"
#include "another-c-library/ac_token.h"

struct ac_cursor_s;
typedef struct ac_cursor_s ac_cursor_t;
/*
    Callback function for custom cursors.  This function is called for each token.
*/
typedef ac_cursor_t *(*ac_cursor_custom_cb)(ac_pool_t *pool, ac_token_t *token, void *arg);

/* The ac_token object has a ac_token_parse_expression function which parses a query
   into the ac_token_t object.  Once this cursor is constructed, call cursor->advance()
   to get each id which matches the given query. */
ac_cursor_t *ac_cursor_open(ac_pool_t *pool, ac_cursor_custom_cb cb, ac_token_t *t, void *arg);

/*
    Create a cursor which will return all ids in the range [start, end)
*/
ac_cursor_t *ac_cursor_range(ac_pool_t *pool, uint32_t start, uint32_t end);

uint32_t ac_cursor_first(ac_pool_t *pool, ac_cursor_custom_cb cb, ac_token_t *t, void *arg);

typedef uint32_t (*ac_cursor_advance_cb)( void * c );
typedef uint32_t (*ac_cursor_advance_to_cb)( void * c, uint32_t id );
typedef void (*ac_cursor_add_cb)( ac_cursor_t *dest, ac_cursor_t *src );

enum ac_cursor_type { EMPTY_CURSOR = 0, AND_CURSOR = 1, PHRASE_CURSOR = 2, OR_CURSOR = 3, NOT_CURSOR = 4, NORMAL_CURSOR=5 };

struct ac_cursor_s {
    ac_pool_t *pool;
    ac_cursor_advance_cb advance;
    ac_cursor_advance_to_cb advance_to;
    ac_cursor_advance_cb _advance;

    ac_cursor_add_cb add;

    enum ac_cursor_type type;

    uint32_t current;
};

/* convert a query to an empty one */
uint32_t ac_cursor_empty(ac_cursor_t *c);

ac_cursor_t *ac_cursor_init_empty(ac_pool_t *pool);
ac_cursor_t *ac_cursor_init_id(ac_pool_t *pool, uint32_t id);
ac_cursor_t *ac_cursor_init_or(ac_pool_t *pool);
ac_cursor_t *ac_cursor_init_and(ac_pool_t *pool);
ac_cursor_t *ac_cursor_init_not(ac_pool_t *pool, ac_cursor_t *pos, ac_cursor_t *neg);

ac_cursor_t ** ac_cursor_subs(ac_cursor_t *c, uint32_t *num_sub );

void ac_cursor_reset(ac_cursor_t *c);


#endif