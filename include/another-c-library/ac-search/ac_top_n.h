#ifndef _ac_top_n_h
#define _ac_top_n_h

#include "another-c-library/ac_pool.h"

typedef struct {
    uint32_t id;
    double weight;
} ac_top_n_item_t;

struct ac_top_n_s;
typedef struct ac_top_n_s ac_top_n_t;

ac_top_n_t *ac_top_n_init(ac_pool_t *pool, uint32_t size);

void ac_top_n_reset(ac_top_n_t *h);
void ac_top_n_add(ac_top_n_t *h, uint32_t id, double weight);

ac_top_n_item_t *ac_top_n_finish(ac_top_n_item_t **ep, ac_top_n_t *h);

#endif