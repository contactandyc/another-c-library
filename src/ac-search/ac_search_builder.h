#ifndef _ac_search_builder_h
#define _ac_search_builder_h

#include <inttypes.h>
#include <stddef.h>
#include "ac_pool.h"
#include "ac_s.h"
#include "ac_cursor.h"
#include "ac_number_range.h"

struct ac_search_builder_s;
typedef struct ac_search_builder_s ac_search_builder_t;

/*
    TODO: optimize term_idx size
*/

ac_search_builder_t *ac_search_builder_init(const char *filename, size_t buffer_size);
void ac_search_builder_global(ac_search_builder_t *h, uint32_t id, const void *d, uint32_t len);

void ac_search_builder_term(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, const char *term );
void ac_search_builder_termf(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, const char *term, ... );

// adds the term and wildcard starting at sp position in string
void ac_search_builder_wterm(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, size_t sp, const char *term );
void ac_search_builder_wtermf(ac_search_builder_t *h, uint32_t id, int32_t value, uint16_t pos, size_t sp, const char *term, ... );

void ac_search_builder_destroy(ac_search_builder_t *h);


typedef struct {
    uint16_t pos;
    int32_t value;
} ac_search_builder_image_term_pos_t;

typedef struct {
    uint8_t *gp;
    uint8_t *egp;
    uint32_t gid;
    uint32_t id;
    int32_t value;
    uint8_t *p;
    uint8_t *ep;
    uint8_t *wp;

    // term_pos filled by func
    uint32_t max_term_size;
    ac_search_builder_image_term_pos_t *term_pos;
    uint32_t num_term_pos;

    char **term;
    char **eterm;
} ac_search_builder_image_term_t;

struct ac_search_builder_image_s;
typedef struct ac_search_builder_image_s ac_search_builder_image_t;

ac_search_builder_image_t *ac_search_builder_image_init(const char *filename);

// length is found in the 4 bytes prior to the returned pointer if non null
const void * ac_search_builder_image_global(ac_search_builder_image_t *h, uint32_t gid);

bool ac_search_builder_image_term(ac_search_builder_image_t *img, ac_pool_t *pool, ac_search_builder_image_term_t *r, const char *term);

/* this is useful if using the term/eterm as it skips the binary search, used for wildcard search */
void ac_search_builder_fill_term(ac_search_builder_image_t *img, ac_pool_t *pool, ac_search_builder_image_term_t *r, char **termp);

bool ac_search_builder_image_advance(ac_search_builder_image_term_t *t);

bool ac_search_builder_image_advance_to(ac_search_builder_image_term_t *t, uint32_t id);

typedef struct {
    ac_cursor_t cursor;
    ac_search_builder_image_term_t term;
} ac_search_builder_cursor2_t;

ac_cursor_t *ac_search_builder_init_cursor(ac_search_builder_image_t *img, ac_pool_t *pool, const char *term);

typedef struct {
    ac_s_cursor_t cursor;
    ac_search_builder_image_term_t term;
    ac_number_range_t range;
} ac_search_builder_cursor_t;

ac_s_cursor_t *ac_search_builder_init_sb2_cursor(ac_search_builder_image_t *img, ac_pool_t *pool, const char *term, ac_number_range_t *nr);

void ac_search_builder_image_destroy(ac_search_builder_image_t *h);

#endif