/* IMPLEMENTATION FOLLOWS - API is above this line */

/* Because this object is called very frequently, some of the functionality is
  inlined. Inlining the structure can be helpful for other objects, particularly
  if they want to be able to take advantage of the remaining memory in a block
  in the way that pool_strdupf does. */

/* When MEMORY_TRACKING is enabled, the pools that are allocated will be tracked
  and logged to help you track down memory leaks.  By default it is turned off.
*/
#ifdef MEMORY_TRACKING
#define stla_pool_init(size)                                                   \
  _stla_pool_tracked_init(size, __FILE__, __FUNCTION__, __LINE__)
#define stla_pool_clear(h)                                                     \
  _stla_pool_tracked_clear(h, __FILE__, __FUNCTION__, __LINE__)
#define stla_pool_destroy(h)                                                   \
  _stla_pool_tracked_destroy(h, __FILE__, __FUNCTION__, __LINE__)
#else
#define stla_pool_init(size) _stla_pool_init(size)
#define stla_pool_clear(h) _stla_pool_clear(h)
#define stla_pool_destroy(h) _stla_pool_destroy(h)
#endif

/* tracked versions for debugging */
stla_pool_t *_stla_pool_tracked_init(size_t size, const char *filename,
                                     const char *function, int line);
void _stla_pool_tracked_clear(stla_pool_t *h, const char *filename,
                              const char *function, int line);
void _stla_pool_tracked_destroy(stla_pool_t *h, const char *filename,
                                const char *function, int line);

/* used internally */
void *_stla_pool_alloc_grow(stla_pool_t *h, size_t len);

typedef struct stla_pool_node_s {
  /* The stla_pool_node_s includes a block of memory just after it.  endp
    points to the end of that block of memory.
   */
  char *endp;

  /* this will be NULL if it is the first block. */
  struct stla_pool_node_s *prev;
} stla_pool_node_t;

struct stla_pool_s {
  /* A pointer to the current block associated with the pool.  If there is more
    than one block, the blocks are linked together via the prev pointer into
    a singly linked list. */
  stla_pool_node_t *current;

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

static inline void *stla_pool_ualloc(stla_pool_t *h, size_t len) {
  char *r = h->curp;
  if (r + len < h->current->endp) {
    h->curp = r + len;
    return r;
  }
  return _stla_pool_alloc_grow(h, len);
}

static inline void *stla_pool_alloc(stla_pool_t *h, size_t len) {
  char *r =
      h->curp + ((sizeof(size_t) - ((size_t)(h->curp) & (sizeof(size_t) - 1))) &
                 (sizeof(size_t) - 1));
  if (r + len < h->current->endp) {
    h->curp = r + len;
    return r;
  }
  return _stla_pool_alloc_grow(h, len);
}

static inline void *stla_pool_calloc(stla_pool_t *h, size_t len) {
  /* calloc will simply call the pool_alloc function and then zero the memory.
   */
  void *dest = stla_pool_alloc(h, len);
  memset(dest, 0, len);
  return dest;
}

static inline char *stla_pool_strdup(stla_pool_t *h, const char *p) {
  /* strdup will simply allocate enough bytes to hold the duplicated string,
    copy the string, and return the newly allocated string. */
  size_t len = strlen(p) + 1;
  char *dest = (char *)stla_pool_ualloc(h, len);
  memcpy(dest, p, len);
  return dest;
}

static inline char *stla_pool_dup(stla_pool_t *h, const void *data, size_t len) {
  /* dup will simply allocate enough bytes to hold the duplicated data,
    copy the data, and return the newly allocated memory which contains a copy
    of data. Because the data could need aligned, we will use stla_pool_alloc
    instead of stla_pool_ualloc */
  char *dest = (char *)stla_pool_alloc(h, len);
  memcpy(dest, data, len);
  return dest;
}

static inline char *stla_pool_strdupf(stla_pool_t *pool, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char *r = stla_pool_strdupvf(pool, fmt, args);
  va_end(args);
  return r;
}
