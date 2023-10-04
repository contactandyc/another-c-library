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

#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_pool.h"
#include <stdlib.h>

size_t ac_pool_size(ac_pool_t *h) {
  return h->size + (h->current->endp - h->curp);
}

size_t ac_pool_used(ac_pool_t *h) { return h->used; }

void ac_pool_set_minimum_growth_size(ac_pool_t *h, size_t size) {
  if (size == 0)
    abort(); /* this doesn't make sense */
  h->minimum_growth_size = size;
}

#ifdef _AC_MEMORY_CHECK_
static void dump_pool(FILE *out, const char *caller, void *p, size_t length) {
  ac_pool_t *pool = (ac_pool_t *)p;
  fprintf(out, "%s size: %lu, max_size: %lu, initial_size: %lu used: %lu ",
          caller, pool->cur_size, pool->max_size, pool->initial_size,
          pool->used);
}

ac_pool_t *_ac_pool_init(size_t initial_size, const char *caller) {
#else
ac_pool_t *_ac_pool_init(size_t initial_size) {
#endif
  if (initial_size == 0)
    abort(); /* this doesn't make any sense */
  /* round initial_size up to be properly aligned */
  initial_size += ((sizeof(size_t) - (initial_size & (sizeof(size_t) - 1))) &
                   (sizeof(size_t) - 1));

  /* Allocate the ac_pool_t structure, the first node, and the memory in one
   call.  This keeps the memory in close proximity which is better for the CPU
   cache.  It also makes it so that when we destroy the handle, we only need to
   make one call to free for the handle, the first block, and the initial size.
  */

  /* If the initial_size is an even multiple of 4096, then reduce the block size
   so that the actual memory allocated via the system malloc is 4096 bytes. */
  size_t block_size = initial_size;
  if ((block_size & 4096) == 0)
    block_size -= (sizeof(ac_pool_t) + sizeof(ac_pool_node_t));

  ac_pool_t *h;
#ifdef _AC_MEMORY_CHECK_
  h = (ac_pool_t *)_ac_malloc_d(
      NULL, caller, block_size + sizeof(ac_pool_t) + sizeof(ac_pool_node_t),
      true);
  memset(h, 0, sizeof(ac_pool_t) + sizeof(ac_pool_node_t));
  h->dump.dump = dump_pool;
  h->initial_size = initial_size;
  h->cur_size = 0;
  h->max_size = 0;
#else
  h = (ac_pool_t *)ac_malloc(block_size + sizeof(ac_pool_t) +
                             sizeof(ac_pool_node_t));
  memset(h, 0, sizeof(ac_pool_t) + sizeof(ac_pool_node_t));
#endif
  if (!h) /* what else might we do? */
    abort();
  h->used = initial_size + sizeof(ac_pool_t) + sizeof(ac_pool_node_t);
  h->size = 0;
  h->pool = NULL;
  h->current = (ac_pool_node_t *)(h + 1);
  h->curp = (char *)(h->current + 1);
  h->current->endp = h->curp + block_size;
  h->current->prev = NULL;

  ac_pool_set_minimum_growth_size(h, initial_size);
  return h;
}



ac_pool_t *ac_pool_pool_init(ac_pool_t *pool, size_t initial_size) {
  if (initial_size == 0)
    abort(); /* this doesn't make any sense */
  /* round initial_size up to be properly aligned */
  initial_size += ((sizeof(size_t) - (initial_size & (sizeof(size_t) - 1))) &
                   (sizeof(size_t) - 1));
  size_t block_size = initial_size;

  ac_pool_t *h;
  h = (ac_pool_t *)ac_pool_alloc(pool, block_size + sizeof(ac_pool_t) +
                                 sizeof(ac_pool_node_t));
  if (!h) /* what else might we do? */
    abort();
  h->used = initial_size + sizeof(ac_pool_t) + sizeof(ac_pool_node_t);
  h->size = 0;
  h->current = (ac_pool_node_t *)(h + 1);
  h->curp = (char *)(h->current + 1);
  h->current->endp = h->curp + block_size;
  h->current->prev = NULL;
  h->pool = pool;
  ac_pool_set_minimum_growth_size(h, initial_size);
  return h;
}


void ac_pool_clear(ac_pool_t *h) {
  /* remove the extra blocks (the ones where prev != NULL) */
  ac_pool_node_t *prev = h->current->prev;
  while (prev) {
    if(!h->pool)
      ac_free(h->current);
    h->current = prev;
    prev = prev->prev;
  }

  /* reset curp to the beginning */
  h->curp = (char *)(h->current + 1);

  /* reset size and used */
  h->size = 0;
#ifdef _AC_MEMORY_CHECK_
  h->cur_size = 0;
#endif
  h->used =
      (h->current->endp - h->curp) + sizeof(ac_pool_t) + sizeof(ac_pool_node_t);
}

void ac_pool_destroy(ac_pool_t *h) {
  /* pool_clear frees all of the memory from all of the extra nodes and only
    leaves the main block and main node allocated */
  ac_pool_clear(h);
  /* free the main block and the main node */
  if(!h->pool)
    ac_free(h);
}

void *_ac_pool_alloc_grow(ac_pool_t *h, size_t len) {
  size_t block_size = len;
  if (block_size < h->minimum_growth_size)
    block_size = h->minimum_growth_size;
  ac_pool_node_t *block;
  if(!h->pool)
    block = (ac_pool_node_t *)ac_malloc(sizeof(ac_pool_node_t) + block_size);
  else
    block = (ac_pool_node_t *)ac_pool_alloc(h->pool, sizeof(ac_pool_node_t) + block_size);
  if (!block)
    abort();
  if (h->current->prev)
    h->size += (h->current->endp - h->curp);
  h->used += sizeof(ac_pool_node_t) + block_size;
  block->prev = h->current;
  h->current = block;
  char *r = (char *)(block + 1);
  block->endp = r + block_size;
  h->curp = r + len;
#ifdef _AC_MEMORY_CHECK_
  h->cur_size += len;
  if (h->cur_size > h->max_size)
    h->max_size = h->cur_size;
#endif
  return r;
}

char *ac_pool_strdupvf(ac_pool_t *pool, const char *fmt, va_list args) {
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
#ifdef _AC_MEMORY_CHECK_
    pool->cur_size += (n + 1);
    if (pool->cur_size > pool->max_size)
      pool->max_size = pool->cur_size;
#endif
    return r;
  }
  r = (char *)ac_pool_ualloc(pool, n + 1);
  va_copy(args_copy, args);
  int n2 = vsnprintf(r, n + 1, fmt, args_copy);
  if (n != n2)
    abort(); // should never happen!
  va_end(args_copy);
  return r;
}

static size_t count_bytes_in_array(char **a, size_t *n) {
  size_t len = sizeof(char *);
  size_t num = 1;
  while (*a) {
    len += strlen(*a) + sizeof(char *) + 1;
    num++;
    a++;
  }
  return len;
}

char **ac_pool_strdupa(ac_pool_t *pool, char **a) {
  if (!a)
    return NULL;

  size_t n = 0;
  size_t len = count_bytes_in_array(a, &n);
  char **r = (char **)ac_pool_alloc(pool, len);
  char *m = (char *)(r + n);
  char **rp = r;
  while (*a) {
    *rp++ = m;
    char *s = *a;
    while (*s)
      *m++ = *s++;
    *m++ = 0;
    a++;
  }
  *rp = NULL;
  return r;
}

static size_t count_bytes_in_arrayn(char **a, size_t num) {
  size_t len = (sizeof(char *) * (num + 1));
  for (size_t i = 0; i < num; i++) {
    len += strlen(*a) + 1;
    a++;
  }
  return len;
}

char **ac_pool_strdupan(ac_pool_t *pool, char **a, size_t n) {
  if (!a)
    return NULL;

  size_t len = count_bytes_in_arrayn(a, n);
  char **r = (char **)ac_pool_alloc(pool, len);
  char *m = (char *)(r + n + 1);
  char **rp = r;
  char **ae = a + n;
  while (a < ae) {
    *rp++ = m;
    char *s = *a;
    while (*s)
      *m++ = *s++;
    *m++ = 0;
    a++;
  }
  *rp = NULL;
  return r;
}

char **ac_pool_strdupa2(ac_pool_t *pool, char **a) {
  if (!a)
    return NULL;

  char **p = a;
  while (*p)
    p++;

  p++;
  return (char **)ac_pool_dup(pool, a, (p - a) * sizeof(char *));
}

static inline void setup_tokenize(uint64_t *bits, const char *delim) {
  bits[0] = bits[1] = 0;
  const char *p = delim;
  while (true) {
    if (*p > 0) {
      if (*p < 64)
        bits[0] |= (1 << (*p));
      else
        bits[1] |= (1 << ((*p) - 64));
    } else if (*p == 0)
      break;

    p++;
  }
}

static inline char *skip_delimiter(uint64_t *bits, char *p) {
  while (*p > 0) {
    if (*p < 64) {
      if (bits[0] & (1 << (*p)))
        p++;
      else
        return p;
    } else {
      if (bits[1] & (1 << ((*p) - 64)))
        p++;
      else
        return p;
    }
  }
  return p;
}

char **_ac_pool_tokenize(ac_pool_t *h, size_t *num_splits, const char *delim,
                         char *s) {
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  uint64_t bits[2];
  setup_tokenize(bits, delim);
  char *p = skip_delimiter(bits, s);
  s = p;
  if (*p == 0) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }

  size_t num = 1;
  while (true) {
    if (*p > 0) {
      if (*p < 64) {
        if (bits[0] & (1 << (*p))) {
          num++;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0) {
            num--;
            break;
          }
        } else
          p++;
      } else {
        if (bits[1] & (1 << ((*p) - 64))) {
          num++;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0) {
            num--;
            break;
          }
        } else
          p++;
      }
    } else if (*p == 0)
      break;
    else
      p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  p = s;
  wr++;
  while (true) {
    if (*p > 0) {
      if (*p < 64) {
        if (bits[0] & (1 << (*p))) {
          *p = 0;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0)
            break;
          *wr = p;
          wr++;
        } else
          p++;
      } else {
        if (bits[1] & (1 << ((*p) - 64))) {
          *p = 0;
          p = skip_delimiter(bits, p + 1);
          if (*p == 0)
            break;
          *wr = p;
          wr++;
        } else
          p++;
      }
    } else if (*p == 0)
      break;
    else
      p++;
  }

  *wr = NULL;
  return r;
}

char *ac_pool_strip_html(ac_pool_t *h, const char *s) {
    char *r = ac_pool_strdup(h, s);
    char *wp = r;
    char *sp = r;
    while(*sp == ' ')
        sp++;
    while(*sp) {
        if(*sp == '<' && strchr(sp+1, '>')) {
            sp++;
            while(*sp && *sp != '>') {
                if(*sp == '\"') {
                    sp++;
                    while(*sp && *sp != '\"') {
                        if(*sp == '\\')
                            sp++;
                        sp++;
                    }
                    if(*sp == '\"')
                        sp++;
                }
                else if(*sp == '\'') {
                    sp++;
                    while(*sp && *sp != '\'') {
                        if(*sp == '\\')
                            sp++;
                        sp++;
                    }
                    if(*sp == '\'')
                        sp++;
                }
                else
                    sp++;
             }
             if(*sp == '>')
                sp++;
        }
        else {
            if(*sp == ' ' && wp > r && wp[-1] == ' ')
                sp++;
            else
                *wp++ = *sp++;
        }
    }
    while(wp > r && wp[-1] == ' ')
        wp--;
    *wp = 0;
    return r;
}

char **ac_pool_tokenize(ac_pool_t *h, size_t *num_splits, const char *delim,
                        const char *s) {
  return _ac_pool_tokenize(h, num_splits, delim,
                           s ? ac_pool_strdup(h, s) : NULL);
}

char *ac_pool_csv_encode(ac_pool_t *h, char delim, const char *s) {
    char *p = (char *)s;
    while(*p) {
        if(*p == delim || *p == '\n' || *p == '\"')
            break;
        p++;
    }
    if(*p == 0)
        return (char *)s;

    int num = 0;
    p = (char *)s;
    while(*p) {
        if(*p == '\"')
            num++;
        p++;
    }
    char *res = (char *)ac_pool_alloc(h, (p-s)+3+num);
    char *wp = res;
    *wp++ = '\"';
    p = (char *)s;
    while(*p) {
        if(*p == '\"')
            *wp++ = '\"';
        *wp++ = *p++;
    }
    *wp++ = '\"';
    *wp = 0;
    return res;
}

char *ac_pool_csv_encodef(ac_pool_t *h, char delim, const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return ac_pool_csv_encode(h, delim, r);
}

char **_ac_pool_split_csv(ac_pool_t *h, size_t *num_splits, char delim,
                          char *s) {
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  char *p = s;
  size_t num = 1;
  while (*p != 0) {
    if (*p == '\"') {
      p++;
      encoded_quote:
      while(*p != 0 && *p != '\"')
        p++;
      if(*p != 0 && p[1] == '\"') {
        p += 2;
        goto encoded_quote;
      }
    }
    else if (*p == delim)
      num++;
    p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  char *wp = s;
  wr++;
    // ,,, => "","","",""

  while (*s != 0) {
    if (*s == '\"') {
      s++;
      encoded_quote2:
      while(*s != 0 && *s != '\"') {
        *wp++ = *s;
        s++;
      }
      if(*s != 0 && s[1] == '\"') {
        *wp++ = *s;
        s += 2;
        goto encoded_quote2;
      }
      s++;
    }
    else if (*s == delim) {
      *wp = 0;
      s++;
      *wr = s;
      wp = s;
      wr++;
    } else
      *wp++ = *s++;
  }
  *wp = 0;
  *wr = NULL;
  return r;
}

char **ac_pool_split_csv(ac_pool_t *h, size_t *num_splits, char delim,
                         const char *p) {
  return _ac_pool_split_csv(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_pool_split_csvf(ac_pool_t *h, size_t *num_splits, char delim,
                          const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_pool_split_csv(h, num_splits, delim, r);
}

char **_ac_pool_split_csv2(ac_pool_t *h, size_t *num_splits, char delim, char *s) {
  size_t num_res = 0;
  char **res = _ac_pool_split_csv(h, &num_res, delim, s);
  char **wp = res;
  char **p = res;
  char **ep = p+num_res;
  while(p < ep) {
    if(*p[0] != 0)
        *wp++ = *p;
    p++;
  }
  *num_splits = wp-res;
  *wp++ = NULL;
  return res;
}

char **ac_pool_split_csv2(ac_pool_t *h, size_t *num_splits, char delim,
                          const char *p) {
  return _ac_pool_split_csv2(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_pool_split_csv2f(ac_pool_t *h, size_t *num_splits, char delim,
                           const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_pool_split_csv2(h, num_splits, delim, r);
}


char **_ac_pool_split(ac_pool_t *h, size_t *num_splits, char delim, char *s) {
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  char *p = s;
  size_t num = 1;
  while (*p != 0) {
    if (*p == delim)
      num++;
    p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  wr++;
    // ,,, => "","","",""

  while (*s != 0) {
    if (*s == delim) {
      *s = 0;
      s++;
      *wr = s;
      wr++;
    } else
      s++;
  }
  *wr = NULL;
  return r;
}

char **ac_pool_split(ac_pool_t *h, size_t *num_splits, char delim,
                     const char *p) {
  return _ac_pool_split(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_pool_splitf(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_pool_split(h, num_splits, delim, r);
}

char **_ac_pool_split2(ac_pool_t *h, size_t *num_splits, char delim, char *s) {
  size_t num_res = 0;
  char **res = _ac_pool_split(h, &num_res, delim, s);
  char **wp = res;
  char **p = res;
  char **ep = p+num_res;
  while(p < ep) {
    if(*p[0] != 0)
        *wp++ = *p;
    p++;
  }
  *num_splits = wp-res;
  *wp++ = NULL;
  return res;
}

char **ac_pool_split2(ac_pool_t *h, size_t *num_splits, char delim,
                      const char *p) {
  return _ac_pool_split2(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_pool_split2f(ac_pool_t *h, size_t *num_splits, char delim,
                       const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_pool_split2(h, num_splits, delim, r);
}


char **_ac_pool_split_str(ac_pool_t *h, size_t *num_splits, const char *delim, char *s) {
  size_t delim_len = strlen(delim);
  if(delim_len == 0) {
    *num_splits = 1;
    char **r = (char **)ac_pool_alloc(h, sizeof(char *)*2);
    r[0] = s;
    r[1] = NULL;
    return r;
  }
  else if(delim_len == 1)
    return _ac_pool_split(h, num_splits, delim[0], s);

  delim_len--;
  static char *nil = NULL;
  if (!s) {
    if (num_splits)
      *num_splits = 0;
    return &nil;
  }
  char *p = s;
  size_t num = 1;
  while (*p != 0) {
    if (*p == *delim && !strncmp(delim+1, p+1, delim_len)) {
      p += delim_len;
      num++;
    }
    p++;
  }
  if (num_splits)
    *num_splits = num;
  char **r = (char **)ac_pool_alloc(h, sizeof(char *) * (num + 1));
  char **wr = r;
  *wr = s;
  wr++;
  while (*s != 0) {
    if (*s == *delim && !strncmp(delim+1, s+1, delim_len)) {
      *s = 0;
      s += delim_len+1;
      *wr = s;
      wr++;
    } else
      s++;
  }
  *wr = NULL;
  return r;
}

char **ac_pool_split_str(ac_pool_t *h, size_t *num_splits, const char *delim,
                         const char *p) {
  return _ac_pool_split_str(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_pool_split_strf(ac_pool_t *h, size_t *num_splits, const char *delim,
                      const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_pool_split_str(h, num_splits, delim, r);
}

char **_ac_pool_split_str2(ac_pool_t *h, size_t *num_splits, const char *delim, char *s) {
  size_t num_res = 0;
  char **res = _ac_pool_split_str(h, &num_res, delim, s);
  char **wp = res;
  char **p = res;
  char **ep = p+num_res;
  while(p < ep) {
    if(*p[0] != 0)
        *wp++ = *p;
    p++;
  }
  *num_splits = wp-res;
  *wp++ = NULL;
  return res;
}

char **ac_pool_split_str2(ac_pool_t *h, size_t *num_splits, const char *delim,
                      const char *p) {
  return _ac_pool_split_str2(h, num_splits, delim, p ? ac_pool_strdup(h, p) : NULL);
}

char **ac_pool_split_str2f(ac_pool_t *h, size_t *num_splits, const char *delim,
                       const char *p, ...) {
  va_list args;
  va_start(args, p);
  char *r = ac_pool_strdupvf(h, p, args);
  va_end(args);
  return _ac_pool_split_str2(h, num_splits, delim, r);
}

