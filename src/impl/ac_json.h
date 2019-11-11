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

#include "../ac_search.h"
#include "../ac_sort.h"

#define AC_JSON_ERROR 0
#define AC_JSON_VALID 1
#define AC_JSON_OBJECT 1
#define AC_JSON_ARRAY 2
#define AC_JSON_BINARY 3
#define AC_JSON_STRING 4
#define AC_JSON_NULL 5
#define AC_JSON_FALSE 6
#define AC_JSON_ZERO 7
#define AC_JSON_NUMBER 8
#define AC_JSON_DECIMAL 9
#define AC_JSON_TRUE 10

static inline ac_json_t *ac_json_binary(ac_pool_t *pool, char *s,
                                        size_t length) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_BINARY;
  j->value = s;
  j->length = length;
  return j;
}

static inline ac_json_t *ac_json_string(ac_pool_t *pool, const char *s,
                                        size_t length) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_STRING;
  j->value = (char *)s;
  j->length = length;
  return j;
}

static inline ac_json_t *ac_json_str(ac_pool_t *pool, const char *s) {
  if (!s)
    return NULL;
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_STRING;
  j->value = (char *)s;
  j->length = strlen(s);
  return j;
}

static inline ac_json_t *ac_json_true(ac_pool_t *pool) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_TRUE;
  j->value = (char *)"true";
  j->length = 4;
  return j;
}

static inline ac_json_t *ac_json_false(ac_pool_t *pool) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_FALSE;
  j->value = (char *)"false";
  j->length = 5;
  return j;
}

static inline ac_json_t *ac_json_null(ac_pool_t *pool) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_NULL;
  j->value = (char *)"null";
  j->length = 4;
  return j;
}

static inline ac_json_t *ac_json_zero(ac_pool_t *pool) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_ZERO;
  j->value = (char *)"0";
  j->length = 1;
  return j;
}

static inline ac_json_t *ac_json_number(ac_pool_t *pool, ssize_t n) {
  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t) + 22);
  j->parent = NULL;
  j->value = (char *)(j + 1);
  j->type = AC_JSON_NUMBER;
  sprintf(j->value, "%ld", n);
  j->length = strlen(j->value);
  return j;
}

static inline ac_json_t *ac_json_number_string(ac_pool_t *pool, char *s) {
  size_t length = strlen(s);
  ac_json_t *j =
      (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t) + length + 1);
  j->parent = NULL;
  j->value = (char *)(j + 1);
  j->type = AC_JSON_NUMBER;
  memcpy(j->value, s, length + 1);
  j->length = length;
  return j;
}

static inline ac_json_t *ac_json_decimal_string(ac_pool_t *pool, char *s) {
  size_t length = strlen(s);
  ac_json_t *j =
      (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t) + length + 1);
  j->parent = NULL;
  j->value = (char *)(j + 1);
  j->type = AC_JSON_DECIMAL;
  memcpy(j->value, s, length + 1);
  j->length = length;
  return j;
}

static inline char *ac_json_decoded(ac_pool_t *pool, ac_json_t *j) {
  if (!j)
    return NULL;
  else if (j->type == AC_JSON_STRING)
    return ac_json_decode(pool, j->value, j->length);
  else if (j->type > AC_JSON_STRING)
    return j->value;
  else
    return NULL;
}

static inline char *ac_json_value(ac_json_t *j) {
  if (j && j->type >= AC_JSON_STRING)
    return j->value;
  else
    return NULL;
}

static inline int ac_json_object_compare(char *key, ac_json_object_node_t **o) {
  return strcmp(key, (*o)->key);
}

static inline int ac_json_object_compare2(const char *key,
                                          const ac_json_object_node_t *o) {
  return strcmp(key, o->key);
}

static inline int ac_json_object_insert_compare(ac_json_object_node_t *a,
                                                ac_json_object_node_t *b) {
  return strcmp(a->key, b->key);
}

static inline ac_map_find_m(__ac_json_find, char, ac_json_object_node_t,
                            ac_json_object_compare2);

static inline ac_map_insert_m(__ac_json_insert, ac_json_object_node_t,
                              ac_json_object_insert_compare);

static inline ac_search_least_m(__ac_json_search, char, ac_json_object_node_t *,
                                ac_json_object_compare);

struct ac_json_error_s;
typedef struct ac_json_error_s ac_json_error_t;

struct ac_json_object_s;
typedef struct ac_json_object_s ac_json_object_t;

struct ac_json_error_s {
  uint32_t type;
  uint16_t line; // for debug lines see AC_JSON_DEBUG block (defaults to zero)
  uint16_t line2;
  char *error_at;
  char *source;
  ac_pool_t *pool;
};

static inline bool ac_json_is_error(ac_json_t *j) {
  return j->type == AC_JSON_ERROR;
}

static inline bool ac_json_is_object(ac_json_t *j) {
  return j->type == AC_JSON_OBJECT;
}

static inline bool ac_json_is_array(ac_json_t *j) {
  return j->type == AC_JSON_ARRAY;
}

struct ac_json_object_s {
  uint32_t type;
  uint32_t num_entries;
  ac_json_t *parent;
  ac_map_t *root;
  ac_json_object_node_t **array;
  ac_json_object_node_t *head;
  ac_json_object_node_t *tail;
  ac_pool_t *pool;
};

typedef struct {
  uint32_t type;
  uint32_t num_entries;
  ac_json_t *parent;
  ac_json_array_node_t **array;
  ac_json_array_node_t *head;
  ac_json_array_node_t *tail;
  ac_pool_t *pool;
} ac_json_array_t;

static inline ac_json_t *ac_json_object(ac_pool_t *pool) {
  ac_json_object_t *obj =
      (ac_json_object_t *)ac_pool_calloc(pool, sizeof(ac_json_object_t));
  obj->type = AC_JSON_OBJECT;
  obj->pool = pool;
  return (ac_json_t *)obj;
}

static inline ac_json_t *ac_json_array(ac_pool_t *pool) {
  ac_json_array_t *a =
      (ac_json_array_t *)ac_pool_calloc(pool, sizeof(ac_json_array_t));
  a->type = AC_JSON_ARRAY;
  a->pool = pool;
  return (ac_json_t *)a;
}

static inline void _ac_json_array_fill(ac_json_array_t *arr) {
  arr->array = (ac_json_array_node_t **)ac_pool_alloc(
      arr->pool, sizeof(ac_json_array_node_t *) * arr->num_entries);
  ac_json_array_node_t **awp = arr->array;
  ac_json_array_node_t *n = arr->head;
  while (n) {
    *awp++ = n;
    n = n->next;
  }
  arr->num_entries = awp - arr->array;
}

static inline ac_json_t *ac_json_array_nth(ac_json_t *j, int nth) {
  ac_json_array_t *arr = (ac_json_array_t *)j;
  if (nth >= arr->num_entries)
    return NULL;
  if (!arr->array)
    _ac_json_array_fill(arr);
  return arr->array[nth]->value;
}

static inline ac_json_array_node_t *ac_json_array_nth_node(ac_json_t *j,
                                                           int nth) {
  ac_json_array_t *arr = (ac_json_array_t *)j;
  if (nth >= arr->num_entries)
    return NULL;
  if (!arr->array)
    _ac_json_array_fill(arr);
  return arr->array[nth];
}

static inline ac_json_t *ac_json_array_scan(ac_json_t *j, int nth) {
  ac_json_array_t *arr = (ac_json_array_t *)j;
  if (nth >= arr->num_entries)
    return NULL;
  ac_json_array_node_t *n = arr->head;
  while (nth) {
    nth--;
    n = n->next;
  }
  return n->value;
}

static inline void ac_json_array_append(ac_json_t *j, ac_json_t *item) {
  if (!item)
    return;

  ac_json_array_t *arr = (ac_json_array_t *)j;
  ac_json_array_node_t *n =
      (ac_json_array_node_t *)ac_pool_alloc(arr->pool, sizeof(*n));
  item->parent = j;
  n->value = item;
  n->next = NULL;
  arr->num_entries++;
  arr->array = NULL;
  if (arr->head) {
    n->previous = arr->tail;
    arr->tail->next = n;
    arr->tail = n;
  } else
    arr->head = arr->tail = n;
}

static inline int ac_json_array_count(ac_json_t *j) {
  ac_json_array_t *arr = (ac_json_array_t *)j;
  return arr->num_entries;
}

static inline ac_json_array_node_t *ac_json_array_first(ac_json_t *j) {
  ac_json_array_t *arr = (ac_json_array_t *)j;
  return arr->head;
}

static inline ac_json_array_node_t *ac_json_array_last(ac_json_t *j) {
  ac_json_array_t *arr = (ac_json_array_t *)j;
  return arr->tail;
}

static inline ac_json_array_node_t *
ac_json_array_next(ac_json_array_node_t *j) {
  return j->next;
}

static inline ac_json_array_node_t *
ac_json_array_previous(ac_json_array_node_t *j) {
  return j->previous;
}

static inline void ac_json_array_erase(ac_json_array_node_t *n) {
  ac_json_array_t *arr = (ac_json_array_t *)(n->value->parent);
  arr->num_entries--;
  if (n->previous) {
    n->previous->next = n->next;
    if (n->next) {
      n->next->previous = n->previous;
      arr->array = NULL;
    } else
      arr->tail = n->previous;
  } else {
    arr->head = n->next;
    if (n->next) {
      n->next->previous = NULL;
      if (arr->array)
        arr->array++;
    } else {
      arr->head = arr->tail = NULL;
      arr->array = NULL;
    }
  }
}

static inline int ac_json_object_count(ac_json_t *j) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  return o->num_entries;
}

static inline ac_json_object_node_t *ac_json_object_first(ac_json_t *j) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  return o->head;
}

static inline ac_json_object_node_t *ac_json_object_last(ac_json_t *j) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  return o->tail;
}

static inline ac_json_object_node_t *
ac_json_object_next(ac_json_object_node_t *j) {
  return j->next;
}

static inline ac_json_object_node_t *
ac_json_object_previous(ac_json_object_node_t *j) {
  return j->previous;
}

static inline void ac_json_object_erase(ac_json_object_node_t *n) {
  ac_json_object_t *o = (ac_json_object_t *)(n->value->parent);
  o->num_entries--;
  ac_map_erase((ac_map_t *)n, &o->root);
  if (n->previous) {
    n->previous->next = n->next;
    if (n->next) {
      n->next->previous = n->previous;
    } else
      o->tail = n->previous;
  } else {
    o->head = n->next;
    if (n->next)
      n->next->previous = NULL;
    else
      o->head = o->tail = NULL;
  }
}

void _ac_json_object_fill(ac_json_object_t *o);

static inline ac_json_object_node_t *ac_json_object_get(ac_json_t *j,
                                                        const char *key) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  if (!o->root) {
    if (o->head)
      _ac_json_object_fill(o);
    else
      return NULL;
  }
  ac_json_object_node_t **res = __ac_json_search(
      (char *)key, (ac_json_object_node_t **)o->root, o->num_entries);
  return *res;
}

static inline ac_json_t *ac_json_object_get_value(ac_json_t *j,
                                                  const char *key) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  if (!o->root) {
    if (o->head)
      _ac_json_object_fill(o);
    else
      return NULL;
  }
  ac_json_object_node_t **res = __ac_json_search(
      (char *)key, (ac_json_object_node_t **)o->root, o->num_entries);
  if (res) {
    ac_json_object_node_t *r = *res;
    if (r)
      return r->value;
  }
  return NULL;
}

static inline ac_json_object_node_t *ac_json_object_scan(ac_json_t *j,
                                                         const char *key) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  ac_json_object_node_t *r = o->head;
  while (r) {
    if (!strcmp(r->key, key))
      return r;
    r = r->next;
  }
  return NULL;
}

static inline void _ac_json_object_fill_tree(ac_json_object_t *o) {
  ac_json_object_node_t *r = o->head;
  while (r) {
    __ac_json_insert(r, &(o->root));
    r = r->next;
  }
}

static inline ac_json_object_node_t *ac_json_object_find(ac_json_t *j,
                                                         const char *key) {
  ac_json_object_t *o = (ac_json_object_t *)j;
  if (!o->root) {
    if (o->head)
      _ac_json_object_fill_tree(o);
    else
      return NULL;
  }
  return __ac_json_find(key, o->root);
}

static inline ac_json_object_node_t *ac_json_object_insert(ac_json_t *j,
                                                           const char *key,
                                                           ac_json_t *item,
                                                           bool copy_key) {
  if (!item)
    return NULL;
  ac_json_object_node_t *res = ac_json_object_find(j, key);
  if (res) {
    item->parent = j;
    res->value = item;
  } else {
    ac_json_object_append(j, key, item, copy_key);
    ac_json_object_t *o = (ac_json_object_t *)j;
    __ac_json_insert(res, &(o->root));
  }
  return res;
}

static inline void ac_json_object_append(ac_json_t *j, const char *key,
                                         ac_json_t *item, bool copy_key) {
  if (!item)
    return;

  ac_json_object_t *o = (ac_json_object_t *)j;
  ac_json_object_node_t *on;
  if (copy_key) {
    on = (ac_json_object_node_t *)ac_pool_calloc(
        o->pool, sizeof(ac_json_object_node_t) + strlen(key) + 1);
    on->key = (char *)(on + 1);
    strcpy(on->key, key);
  } else {
    on = (ac_json_object_node_t *)ac_pool_calloc(o->pool,
                                                 sizeof(ac_json_object_node_t));
    on->key = (char *)key;
  }
  on->value = item;
  item->parent = j;

  o->num_entries++;
  if (!o->head)
    o->head = o->tail = on;
  else {
    on->previous = o->tail;
    o->tail->next = on;
    o->tail = on;
  }
}