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

/* IMPLEMENTATION FOLLOWS - API is above this line */

/* Because this object is called very frequently, some of the functionality is
  inlined. Inlining the structure can be helpful for other objects, particularly
  if they want to be able to take advantage of the remaining memory in a block
  in the way that pool_strdupf does. */

/* used internally */
void *_acpool_alloc_grow(acpool_t *h, size_t len);

typedef struct acpool_node_s {
  /* The acpool_node_s includes a block of memory just after it.  endp
    points to the end of that block of memory.
   */
  char *endp;

  /* this will be NULL if it is the first block. */
  struct acpool_node_s *prev;
} acpool_node_t;

struct acpool_s {
#ifdef _ACDEBUG_MEMORY_
  acallocator_dump_t dump;
  /* The size of the initial block requires a second variable to be
     thread-safe. */
  size_t initial_size;
  /* The cur_size is needed because the acpool_size function isn't
     thread-safe. */
  size_t cur_size;
  /* Everytime the pool get's cleared, cur_size is reset */
  size_t max_size;
#endif

  /* A pointer to the current block associated with the pool.  If there is more
    than one block, the blocks are linked together via the prev pointer into
    a singly linked list. */
  acpool_node_t *current;

  /* A pointer into the current node where memory is available. */
  char *curp;

  /* This is used as an alternate size for new blocks beyond the initial
    block.  It will be initially set to the length of the first block and can
    later be modified. */
  size_t minimum_growth_size;

  /* the size doesn't consider the bytes that are used in the current block */
  size_t size;

  /* the total number of bytes allocated by the pool object */
  size_t used;
};

static inline void *acpool_ualloc(acpool_t *h, size_t len) {
  char *r = h->curp;
  if (r + len < h->current->endp) {
    h->curp = r + len;
#ifdef _ACDEBUG_MEMORY_
    h->cur_size += len;
    if (h->cur_size > h->max_size)
      h->max_size = h->cur_size;
#endif
    return r;
  }
  return _acpool_alloc_grow(h, len);
}

static inline void *acpool_alloc(acpool_t *h, size_t len) {
  char *r =
      h->curp + ((sizeof(size_t) - ((size_t)(h->curp) & (sizeof(size_t) - 1))) &
                 (sizeof(size_t) - 1));
  if (r + len < h->current->endp) {
    h->curp = r + len;
#ifdef _ACDEBUG_MEMORY_
    h->cur_size += len;
    if (h->cur_size > h->max_size)
      h->max_size = h->cur_size;
#endif
    return r;
  }
  return _acpool_alloc_grow(h, len);
}

static inline void *acpool_calloc(acpool_t *h, size_t len) {
  /* calloc will simply call the pool_alloc function and then zero the memory.
   */
  void *dest = acpool_alloc(h, len);
  if (len)
    memset(dest, 0, len);
  return dest;
}

static inline void *acpool_udup(acpool_t *h, const void *data,
                                   size_t len) {
  /* dup will simply allocate enough bytes to hold the duplicated data,
    copy the data, and return the newly allocated memory which contains a copy
    of data. Because the data could need aligned, we will use acpool_alloc
    instead of acpool_ualloc */
  char *dest = (char *)acpool_ualloc(h, len);
  if (len)
    memcpy(dest, data, len);
  return dest;
}

static inline char *acpool_strdup(acpool_t *h, const char *p) {
  /* strdup will simply allocate enough bytes to hold the duplicated string,
    copy the string, and return the newly allocated string. */
  size_t len = strlen(p) + 1;
  return (char *)acpool_udup(h, p, len);
}

static inline char *acpool_strndup(acpool_t *h, const char *p,
                                      size_t length) {
  /* strdup will simply allocate enough bytes to hold the duplicated string,
    copy the string, and return the newly allocated string. */
  size_t len = strlen(p);
  if (len > length)
    len = length;
  return (char *)acpool_udup(h, p, len + 1);
}

static inline void *acpool_dup(acpool_t *h, const void *data,
                                  size_t len) {
  /* dup will simply allocate enough bytes to hold the duplicated data,
    copy the data, and return the newly allocated memory which contains a copy
    of data. Because the data could need aligned, we will use acpool_alloc
    instead of acpool_ualloc */
  char *dest = (char *)acpool_alloc(h, len);
  if (len)
    memcpy(dest, data, len);
  return dest;
}

static inline char *acpool_strdupf(acpool_t *pool, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = acpool_strdupvf(pool, fmt, args);
  va_end(args);
  return r;
}