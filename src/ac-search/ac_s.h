#ifndef _ac_s_H
#define _ac_s_H

#include <inttypes.h>
#include "ac_pool.h"
#include "ac_token.h"

struct ac_s_s;
typedef struct ac_s_s ac_s_t;

/* structure used to advance over the results of a search query */
struct ac_s_cursor_t;
typedef struct ac_s_cursor_t ac_s_cursor_t;

typedef ac_s_cursor_t *(*ac_s_custom_cursor_cb)(ac_pool_t *pool, ac_s_t *h, ac_token_t *token, void *arg);
ac_s_t *ac_s_init(ac_s_custom_cursor_cb cb);

void ac_s_destroy(ac_s_t *h);

/* insert data for a term/id pair (use ac_sb to build the data) */
void ac_s_insert(ac_s_t *h, const char *term, uint32_t id, const void *data, uint32_t len);

/* erase a term/id pair along with its data */
void ac_s_erase(ac_s_t *h, const char *term, uint32_t id);

/* find the data associated with a term/id pair */
uint8_t *ac_s_find(ac_s_t *h, uint32_t *len, const char *term, uint32_t id);

/* The ac_token object has a ac_token_parse_expression function which parses a query 
   into the ac_token_t object.  Once this cursor is constructed, call cursor->advance()
   to get each id which matches the given query. */
ac_s_cursor_t *ac_s_open_cursor(ac_pool_t *pool, ac_s_t *search, ac_token_t *t, void *arg);

/* advanced: for manually constructed queries */
typedef uint32_t (*ac_s_advance_cb)( void * c );
typedef uint32_t (*ac_s_advance_to_cb)( void * c, uint32_t id );
typedef void (*ac_s_add_to_cursor_cb)( ac_s_cursor_t *dest, ac_s_cursor_t *src );

struct ac_s_cursor_t {
    ac_pool_t *pool;
    ac_s_advance_cb advance;
    ac_s_advance_to_cb advance_to;
    ac_s_advance_cb _advance;

    ac_s_add_to_cursor_cb add_to_cursor;

    int type;

    uint32_t current;

    uint16_t *wp;
    uint16_t *ewp;
};

uint32_t ac_s_empty_cursor(ac_s_cursor_t *c);

ac_s_cursor_t *ac_s_init_term_cursor(ac_pool_t *pool, ac_s_t *h, ac_token_t *token, void *arg);
ac_s_cursor_t *ac_s_init_empty_cursor(ac_pool_t *pool);
ac_s_cursor_t *ac_s_init_or_cursor(ac_pool_t *pool);

/* supports word positions */
ac_s_cursor_t *ac_s_init_or_term_cursor(ac_pool_t *pool);

ac_s_cursor_t *ac_s_init_and_cursor(ac_pool_t *pool);
ac_s_cursor_t *ac_s_init_phrase_cursor(ac_pool_t *pool);

ac_s_cursor_t *ac_s_filter_min_position(ac_s_cursor_t *c, uint16_t min_position);
ac_s_cursor_t *ac_s_filter_max_position(ac_s_cursor_t *c, uint16_t max_position);

ac_s_cursor_t ** ac_s_sub_cursors(ac_s_cursor_t *c, uint32_t *num_sub );

void ac_s_reset(ac_s_cursor_t *c);

#endif