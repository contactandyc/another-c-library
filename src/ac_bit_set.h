#ifndef _ac_bit_set_h
#define _ac_bit_set_h

#include "ac_cursor.h"
#include "ac_pool.h"

struct ac_bit_set_s;
typedef struct ac_bit_set_s ac_bit_set_t;

ac_bit_set_t * ac_bit_set_init(ac_pool_t *pool, uint32_t num_items);
bool ac_bit_set(ac_bit_set_t *h, uint32_t id);

void ac_bit_set_set(ac_bit_set_t *h, uint32_t id);
void ac_bit_set_boolean(ac_bit_set_t *h, uint32_t id, bool v);
void ac_bit_set_unset(ac_bit_set_t *h, uint32_t id);

void ac_bit_set_and(ac_bit_set_t *dest, ac_bit_set_t *to_and);
void ac_bit_set_complement(ac_bit_set_t *h);
ac_bit_set_t *ac_bit_set_copy(ac_pool_t *pool, ac_bit_set_t *src);
uint32_t ac_bit_set_count(ac_bit_set_t *h);
uint32_t ac_bit_set_count_and_zero(ac_bit_set_t *h);
// get first set bit
uint32_t ac_bit_set_first(ac_bit_set_t *bs);
void ac_bit_set_false(ac_bit_set_t *h);
void ac_bit_set_not(ac_bit_set_t *dest, ac_bit_set_t *to_not);
void ac_bit_set_or(ac_bit_set_t *dest, ac_bit_set_t *to_or);
void ac_bit_set_true(ac_bit_set_t *h);

ac_cursor_t *ac_bit_set_open_cursor(ac_pool_t *pool, ac_bit_set_t *h);

#endif