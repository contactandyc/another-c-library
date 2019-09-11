#include "stla_pool.h"
#include <stdlib.h>

size_t stla_pool_size(stla_pool_t *h) {
  return h->size + (h->current->endp - h->curp);
}

size_t stla_pool_used(stla_pool_t *h) { return h->used; }

void stla_pool_set_minimum_growth_size(stla_pool_t *h, size_t size) {
  if(size == 0) abort();  /* this doesn't make sense */
  h->minimum_growth_size = size;
}


stla_pool_t *_stla_pool_init(size_t initial_size) {
  if (initial_size == 0)
    abort(); /* this doesn't make any sense */
  /* round initial_size up to be properly aligned */
  initial_size += ((sizeof(size_t) - (initial_size & (sizeof(size_t) - 1))) &
                   (sizeof(size_t) - 1));

  /* Allocate the stla_pool_t structure, the first node, and the memory in one
    call.  This keeps the memory in close proximity which is better for the CPU
    cache.  It also makes it so that when we destroy the handle, we only need to
    make one call to free for the handle, the first block, and the initial size.
  */

  /* If the initial_size is an even multiple of 4096, then reduce the block size
    so that the actual memory allocated via the system malloc is 4096 bytes. */
  size_t block_size = initial_size;
  if ((block_size & 4096) == 0)
    block_size -= (sizeof(stla_pool_t) + sizeof(stla_pool_node_t));

  stla_pool_t *h = (stla_pool_t *)malloc(block_size + sizeof(stla_pool_t) +
                                         sizeof(stla_pool_node_t));
  if (!h) /* what else might we do? */
    abort();
  h->used = initial_size + sizeof(stla_pool_t) + sizeof(stla_pool_node_t);
  h->size = 0;
  h->current = (stla_pool_node_t *)(h + 1);
  h->curp = (char *)(h->current + 1);
  h->current->endp = h->curp + block_size;
  h->current->prev = NULL;

  stla_pool_set_minimum_growth_size(h, initial_size);
  return h;
}

void _stla_pool_clear(stla_pool_t *h) {
  /* remove the extra blocks (the ones where prev != NULL) */
  stla_pool_node_t *prev = h->current->prev;
  while (prev) {
    free(h->current);
    h->current = prev;
    prev = prev->prev;
  }
  /* reset curp to the beginning */
  h->curp = (char *)(h->current + 1);
  /* reset size and used */
  h->size = 0;
  h->used = (h->current->endp - h->curp) + sizeof(stla_pool_t) +
            sizeof(stla_pool_node_t);
}

void _stla_pool_destroy(stla_pool_t *h) {
  /* pool_clear frees all of the memory from all of the extra nodes and only
    leaves the main block and main node allocated */
  stla_pool_clear(h);
  /* free the main block and the main node */
  free(h);
}

void *_stla_pool_alloc_grow(stla_pool_t *h, size_t len) {
  size_t block_size = len;
  if (block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  stla_pool_node_t *block =
      (stla_pool_node_t *)malloc(sizeof(stla_pool_node_t) + block_size);
  if (!block)
    abort();
  block->prev = h->current;
  h->current = block;
  char *r = (char *)(block + 1);
  block->endp = r + block_size;
  h->curp = r + len;
  return r;
}

char *stla_pool_strdupvf(stla_pool_t *pool, const char *fmt, va_list args) {
  va_list args_copy;
  va_copy(args_copy, args);
  size_t leftover = pool->current->endp - pool->curp;
  char *r = pool->curp;
  int n = vsnprintf(r, leftover, fmt, args_copy);
  if (n < 0)
    abort();
  va_end(args_copy);
  if (n < leftover) {
    pool->curp += n + 1;
    return r;
  }
  r = (char *)stla_pool_ualloc(pool, n + 1);
  va_copy(args_copy, args);
  int n2 = vsnprintf(r, n + 1, fmt, args_copy);
  if (n != n2)
    abort(); // should never happen!
  va_end(args_copy);
  return r;
}

stla_pool_t *_stla_pool_tracked_init(size_t size, const char *filename,
                                     const char *function, int line) {
  /* we will implement tracking later */
  return _stla_pool_init(size);
}

void _stla_pool_tracked_clear(stla_pool_t *h, const char *filename,
                              const char *function, int line) {
  /* we will implement tracking later */
  _stla_pool_clear(h);
}

void _stla_pool_tracked_destroy(stla_pool_t *h, const char *filename,
                                const char *function, int line) {
  /* we will implement tracking later */
  _stla_pool_destroy(h);
}
