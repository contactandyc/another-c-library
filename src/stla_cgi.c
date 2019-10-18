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

#include "stla_cgi.h"

#include "stla_conv.h"
#include "stla_map.h"

#include <stdio.h>
#include <string.h>

static inline int to_hex(int v) {
  if (v >= '0' && v <= '9')
    return v - '0';
  else if (v >= 'a' && v <= 'f')
    return v - 'a' + 10;
  else if (v >= 'A' && v <= 'F')
    return v - 'A' + 10;
  return -1;
}

char *stla_cgi_decode(stla_pool_t *pool, char *s) {
  char *res = (char *)stla_pool_alloc(pool, strlen(s) + 1);
  char *p = s;
  char *wp = res;
  while (*p != 0) {
    if (*p == '%') {
      int v1 = to_hex(p[1]);
      int v2 = to_hex(p[2]);
      if (v1 >= 0 && v2 >= 0) {
        v1 <<= 4;
        v1 += v2;
        if (v1 != 1 && v1 < 32)
          v1 = 32;
        if (v1 <= 255)
          *wp++ = v1;
        p += 3;
        continue;
      }
    } else if (*p == '+') {
      *wp++ = ' ';
      p++;
      continue;
    }
    *wp++ = *p++;
  }
  *wp = 0;
  wp = res;
  p = res;
  while (*p != 0) {
    if (*p == '&') {
      p++;
      if (p[0] == 'a') {
        if (p[1] == 'p' && p[2] == 'o' && p[3] == 's' && p[4] == ';') {
          *wp++ = '\'';
          p += 5;
        } else if (p[1] == 'm' && p[2] == 'p' && p[3] == ';') {
          *wp++ = '&';
          p += 4;
        } else
          *wp++ = '&';
      } else if (p[0] == 'g' && p[1] == 't' && p[2] == ';') {
        *wp++ = '>';
        p += 3;
      } else if (p[0] == 'l' && p[1] == 't' && p[2] == ';') {
        *wp++ = '<';
        p += 3;
      } else if (p[0] == 'q' && p[1] == 'u' && p[2] == 'o' && p[3] == 't' &&
                 p[4] == ';') {
        *wp++ = '\"';
        p += 5;
      } else
        *wp++ = '&';
    }
  }
  return res;
}

typedef struct value_s {
  char *value;
  char *decoded;
  struct value_s *next;
} value_t;

typedef struct {
  stla_map_t map;
  value_t *head, *tail;
} cgi_node_t;

struct stla_cgi_s {
  stla_pool_t *pool;
  stla_map_t *root;
};

static inline const char *get_key(const cgi_node_t *el) {
  return (char *)(el + 1);
}

static inline int compare_key(const cgi_node_t *a, const cgi_node_t *b) {
  return strcasecmp(get_key(a), get_key(b));
}

static inline int compare_key_for_find(const char *key,
                                       const cgi_node_t *value) {
  return strcasecmp(key, get_key(value));
}

stla_map_find_m(cgi_find, char, cgi_node_t, compare_key_for_find);
stla_map_insert_m(cgi_insert, cgi_node_t, compare_key);

const char *stla_cgi_query(stla_cgi_t *h) { return (char *)(h + 1); }

char **stla_cgi_strs(stla_cgi_t *h, const char *key) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return NULL;

  size_t num = 1;
  value_t *v = n->head;
  while (v) {
    num++;
    v = v->next;
  }
  char **res = (char **)stla_pool_alloc(h->pool, sizeof(char *) * num);
  char **rp = res;
  v = n->head;
  while (v) {
    *rp = v->decoded;
    rp++;
    v = v->next;
  }
  *rp = NULL;
  return res;
}

const char *stla_cgi_str(stla_cgi_t *h, const char *key,
                         const char *default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_str(n->head->decoded, default_value);
}

bool stla_cgi_bool(stla_cgi_t *h, const char *key, bool default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_bool(n->head->decoded, default_value);
}

int stla_cgi_int(stla_cgi_t *h, const char *key, int default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_int(n->head->decoded, default_value);
}

long stla_cgi_long(stla_cgi_t *h, const char *key, long default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_long(n->head->decoded, default_value);
}

double stla_cgi_double(stla_cgi_t *h, const char *key, double default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_double(n->head->decoded, default_value);
}

int32_t stla_cgi_int32_t(stla_cgi_t *h, const char *key,
                         int32_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_int32_t(n->head->decoded, default_value);
}

uint32_t stla_cgi_uint32_t(stla_cgi_t *h, const char *key,
                           uint32_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_uint32_t(n->head->decoded, default_value);
}

int64_t stla_cgi_int64_t(stla_cgi_t *h, const char *key,
                         int64_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_int64_t(n->head->decoded, default_value);
}

uint64_t stla_cgi_uint64_t(stla_cgi_t *h, const char *key,
                           uint64_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return stla_uint64_t(n->head->decoded, default_value);
}

static void add_key_value(stla_cgi_t *h, char *kv) {
  if (*kv == 0)
    return;
  if (*kv == '=')
    return;
  char *p = kv;
  while (*p && *p != '=')
    p++;
  char *value = NULL, *decoded = NULL;
  if (*p == '=') {
    *p = 0;
    p++;
    value = p;
    decoded = stla_cgi_decode(h->pool, p);
  }
  cgi_node_t *n = cgi_find(kv, h->root);
  if (!n) {
    n = (cgi_node_t *)stla_pool_alloc(h->pool,
                                      sizeof(cgi_node_t) + strlen(kv) + 1);
    strcpy((char *)(n + 1), kv);
    n->head = n->tail = NULL;
    cgi_insert(n, &h->root);
  }
  if (value) {
    value_t *v = (value_t *)stla_pool_alloc(h->pool, sizeof(value_t));
    v->value = value;
    v->decoded = decoded;
    v->next = NULL;
    if (!n->head)
      n->head = n->tail = v;
    else {
      n->tail->next = v;
      n->tail = v;
    }
  }
}

stla_cgi_t *stla_cgi_init(stla_pool_t *pool, const char *q) {
  stla_cgi_t *h = (stla_cgi_t *)stla_pool_alloc(pool, sizeof(stla_cgi_t) +
                                                          (strlen(q) * 2) + 2);
  h->pool = pool;
  strcpy((char *)(h + 1), q);

  char *p = (char *)(h + 1);
  char *p2 = strchr(p, '?');
  if (p2)
    p = p2 + 1;

  while (*p) {
    char *sp = p;
    while (*p && *p != '&')
      p++;
    if (*p == '&') {
      *p = 0;
      p++;
    }
    add_key_value(h, sp);
  }
  return h;
}
