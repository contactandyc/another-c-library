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

#include "accgi.h"

#include "acconv.h"
#include "acmap.h"

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

char *accgi_decode(acpool_t *pool, char *s) {
  char *res = (char *)acpool_alloc(pool, strlen(s) + 1);
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
  acmap_t map;
  value_t *head, *tail;
} cgi_node_t;

struct accgi_s {
  acpool_t *pool;
  acmap_t *root;
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

acmap_find_m(cgi_find, char, cgi_node_t, compare_key_for_find);
acmap_insert_m(cgi_insert, cgi_node_t, compare_key);

const char *accgi_query(accgi_t *h) { return (char *)(h + 1); }

char **accgi_strs(accgi_t *h, const char *key) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return NULL;

  size_t num = 1;
  value_t *v = n->head;
  while (v) {
    num++;
    v = v->next;
  }
  char **res = (char **)acpool_alloc(h->pool, sizeof(char *) * num);
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

const char *accgi_str(accgi_t *h, const char *key,
                         const char *default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acstr(n->head->decoded, default_value);
}

bool accgi_bool(accgi_t *h, const char *key, bool default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acbool(n->head->decoded, default_value);
}

int accgi_int(accgi_t *h, const char *key, int default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acint(n->head->decoded, default_value);
}

long accgi_long(accgi_t *h, const char *key, long default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return aclong(n->head->decoded, default_value);
}

double accgi_double(accgi_t *h, const char *key, double default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acdouble(n->head->decoded, default_value);
}

int32_t accgi_int32_t(accgi_t *h, const char *key,
                         int32_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acint32_t(n->head->decoded, default_value);
}

uint32_t accgi_uint32_t(accgi_t *h, const char *key,
                           uint32_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acuint32_t(n->head->decoded, default_value);
}

int64_t accgi_int64_t(accgi_t *h, const char *key,
                         int64_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acint64_t(n->head->decoded, default_value);
}

uint64_t accgi_uint64_t(accgi_t *h, const char *key,
                           uint64_t default_value) {
  cgi_node_t *n = cgi_find(key, h->root);
  if (!n || !n->head)
    return default_value;

  return acuint64_t(n->head->decoded, default_value);
}

static void add_key_value(accgi_t *h, char *kv) {
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
    decoded = accgi_decode(h->pool, p);
  }
  cgi_node_t *n = cgi_find(kv, h->root);
  if (!n) {
    n = (cgi_node_t *)acpool_alloc(h->pool,
                                      sizeof(cgi_node_t) + strlen(kv) + 1);
    strcpy((char *)(n + 1), kv);
    n->head = n->tail = NULL;
    cgi_insert(n, &h->root);
  }
  if (value) {
    value_t *v = (value_t *)acpool_alloc(h->pool, sizeof(value_t));
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

accgi_t *accgi_init(acpool_t *pool, const char *q) {
  accgi_t *h = (accgi_t *)acpool_alloc(pool, sizeof(accgi_t) +
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
