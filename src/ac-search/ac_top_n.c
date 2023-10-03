#include "another-c-library/ac_top_n.h"
#include "another-c-library/ac_sort.h"

static inline int compare_ac_top_n_item(const ac_top_n_item_t *a, const ac_top_n_item_t *b) {
  if(a->weight != b->weight)
    return (a->weight < b->weight) ? 1 : -1; // descending
  if(a->id != b->id)
    return (a->id < b->id) ? -1 : 1;
  return 0;
}
static inline ac_sort_m(sort_ac_top_n_item, ac_top_n_item_t, compare_ac_top_n_item);

struct ac_top_n_s {
    ac_top_n_item_t *iw;
    ac_top_n_item_t *split;
    ac_top_n_item_t *p;
    ac_top_n_item_t *ep;
};

ac_top_n_t *ac_top_n_init(ac_pool_t *pool, uint32_t size) {
    size = size * 2;
    ac_top_n_t *t = (ac_top_n_t *)ac_pool_calloc(pool, sizeof(*t));
    t->iw = (ac_top_n_item_t *)ac_pool_alloc(pool, sizeof(ac_top_n_item_t) * size);
    t->p = t->iw;
    t->split = t->ep = t->iw + size;
    return t;
}

void ac_top_n_clear(ac_top_n_t *top) {
    top->split = top->ep;
    top->p = top->iw;
}

void ac_top_n_add(ac_top_n_t *top, uint32_t id, double weight) {
    if(top->p < top->split) {
        ac_top_n_item_t *iw = top->p;
        iw->id = id;
        iw->weight = weight;
        top->p++;
    }
    else {
        if(top->p < top->ep) {
            if(weight > top->split->weight) {
                ac_top_n_item_t *iw = top->p;
                iw->id = id;
                iw->weight = weight;
                top->p++;
            }
        }
        else {
            if(top->split == top->ep) {
                // sort everything
                sort_ac_top_n_item(top->iw, top->ep-top->iw);
                top->split = top->iw + ((top->ep-top->iw) / 2);
            }
            else {
                // an optimization could be to sort the second half and merge
                // for now just sort everything
                sort_ac_top_n_item(top->iw, top->ep-top->iw);
            }
            top->p = top->split;
        }
    }
}

ac_top_n_item_t *ac_top_n_finish(ac_top_n_item_t **ep, ac_top_n_t *top) {
    sort_ac_top_n_item(top->iw, top->p-top->iw);
    *ep = top->p;
    return top->iw;
}

