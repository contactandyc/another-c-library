/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "stla_pool.h"
#include "stla_allocator.h"
#include <stdlib.h>

size_t stla_pool_size(stla_pool_t *h) {
  return h->size + (h->current->endp - h->curp);
}

size_t stla_pool_used(stla_pool_t *h) { return h->used; }

void stla_pool_set_minimum_growth_size(stla_pool_t *h, size_t size) {
  if(size == 0) abort();  /* this doesn't make sense */
  h->minimum_growth_size = size;
}

#ifdef _STLA_DEBUG_MEMORY_
static void dump_pool(FILE *out, const char *caller, void *p, size_t length) {
  stla_pool_t *pool = (stla_pool_t *)p;
  fprintf( out, "%s size: %lu, max_size: %lu, initial_size: %lu used: %lu ",
           caller, pool->cur_size, pool->max_size, pool->initial_size, pool->used );
}

stla_pool_t *_stla_pool_init(size_t initial_size, const char *caller) {
#else
stla_pool_t *_stla_pool_init(size_t initial_size) {
#endif
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

  stla_pool_t *h;
#ifdef _STLA_DEBUG_MEMORY_
  h = (stla_pool_t *)_stla_malloc_d(NULL, caller,
                                    block_size + sizeof(stla_pool_t) +
                                    sizeof(stla_pool_node_t), true);
  h->dump.dump = dump_pool;
  h->initial_size = initial_size;
  h->cur_size = 0;
  h->max_size = 0;
#else
  h = (stla_pool_t *)stla_malloc(block_size + sizeof(stla_pool_t) +
                                 sizeof(stla_pool_node_t));
#endif
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

void stla_pool_clear(stla_pool_t *h) {
  /* remove the extra blocks (the ones where prev != NULL) */
  stla_pool_node_t *prev = h->current->prev;
  while (prev) {
    stla_free(h->current);
    h->current = prev;
    prev = prev->prev;
  }

  /* reset curp to the beginning */
  h->curp = (char *)(h->current + 1);

  /* reset size and used */
  h->size = 0;
#ifdef _STLA_DEBUG_MEMORY_
  h->cur_size = 0;
#endif
  h->used = (h->current->endp - h->curp) + sizeof(stla_pool_t) +
            sizeof(stla_pool_node_t);
}

void stla_pool_destroy(stla_pool_t *h) {
  /* pool_clear frees all of the memory from all of the extra nodes and only
    leaves the main block and main node allocated */
  stla_pool_clear(h);
  /* free the main block and the main node */
  stla_free(h);
}

void *_stla_pool_alloc_grow(stla_pool_t *h, size_t len) {
  size_t block_size = len;
  if (block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  stla_pool_node_t *block =
      (stla_pool_node_t *)stla_malloc(sizeof(stla_pool_node_t) + block_size);
  if (!block)
    abort();
  if(h->current->prev)
    h->size += (h->current->endp - h->curp);
  h->used += sizeof(stla_pool_node_t) + block_size;
  block->prev = h->current;
  h->current = block;
  char *r = (char *)(block + 1);
  block->endp = r + block_size;
  h->curp = r + len;
#ifdef _STLA_DEBUG_MEMORY_
  h->cur_size += len;
  if(h->cur_size > h->max_size)
    h->max_size = h->cur_size;
#endif
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
#ifdef _STLA_DEBUG_MEMORY_
    pool->cur_size += (n+1);
    if(pool->cur_size > pool->max_size)
      pool->max_size = pool->cur_size;
#endif
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
