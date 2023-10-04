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

#include "the-macro-library/macro_map.h"
#include "the-macro-library/macro_bsearch.h"

#define AC_JSON_ERROR 0
#define AC_JSON_VALID 1
#define AC_JSON_OBJECT 1
#define AC_JSON_ARRAY 2
#define AC_JSON_BINARY 3
#define AC_JSON_NULL 4
#define AC_JSON_STRING 5
#define AC_JSON_FALSE 6
#define AC_JSON_ZERO 7
#define AC_JSON_NUMBER 8
#define AC_JSON_DECIMAL 9
#define AC_JSON_TRUE 10

struct ac_json_s {
  uint32_t type;
  uint32_t length;
  ac_json_t *parent;
  char *value;
};

static inline ac_json_type_t ac_json_type(ac_json_t *j) {
  return (ac_json_type_t)(j->type);
}

struct ac_jsona_s {
  ac_json_t *value;
  ac_jsona_t *next;
  ac_jsona_t *previous;
};

struct ac_jsono_s {
  macro_map_t map;
  char *key;
  ac_json_t *value;
  ac_jsono_t *next;
  ac_jsono_t *previous;
};

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

static inline ac_json_t *ac_json_encode_string(ac_pool_t *pool, const char *s, size_t length) {
  if (!s)
    return NULL;

  char *v = ac_json_encode(pool, (char *)s, length);

  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_STRING;
  j->value = (char *)v;
  j->length = strlen(v);
  return j;
}

static inline ac_json_t *ac_json_encode_str(ac_pool_t *pool, const char *s) {
  if (!s)
    return NULL;

  char *v = ac_json_encode(pool, (char *)s, strlen(s));

  ac_json_t *j = (ac_json_t *)ac_pool_alloc(pool, sizeof(ac_json_t));
  j->parent = NULL;
  j->type = AC_JSON_STRING;
  j->value = (char *)v;
  j->length = strlen(v);
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
  snprintf(j->value, 22, "%ld", n);
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

static inline char *ac_jsond(ac_pool_t *pool, ac_json_t *j) {
  if (!j)
    return NULL;
  else if (j->type == AC_JSON_STRING)
    return ac_json_decode(pool, j->value, j->length);
  else if (j->type > AC_JSON_STRING)
    return j->value;
  else
    return NULL;
}

static inline char *ac_jsonb(ac_json_t *j, size_t *length) {
  if (j && j->type >= AC_JSON_BINARY) {
    *length = j->length;
    return j->value;
  } else
    return NULL;
}

static inline char *ac_jsonv(ac_json_t *j) {
  if (j && j->type >= AC_JSON_STRING)
    return j->value;
  else
    return NULL;
}

static inline int ac_jsono_compare(const char *key, const ac_jsono_t **o) {
  return strcmp(key, (*o)->key);
}

static inline int ac_jsono_compare2(const char *key, const ac_jsono_t *o) {
  return strcmp(key, o->key);
}

static inline int ac_jsono_insert_compare(const ac_jsono_t *a,
                                          const ac_jsono_t *b) {
  return strcmp(a->key, b->key);
}

static inline
macro_map_find_kv(__ac_json_find, char, ac_jsono_t,
                  ac_jsono_compare2);

static inline macro_map_insert(__ac_json_insert, ac_jsono_t,
                                  ac_jsono_insert_compare);

static inline macro_bsearch_first_kv(__ac_json_search, char, ac_jsono_t *,
                                  ac_jsono_compare);

struct ac_json_error_s;
typedef struct ac_json_error_s ac_json_error_t;

struct _ac_jsono_s;
typedef struct _ac_jsono_s _ac_jsono_t;

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

struct _ac_jsono_s {
  uint32_t type;
  uint32_t num_entries;
  ac_json_t *parent;
  macro_map_t *root;
  size_t num_sorted_entries;
  ac_jsono_t *head;
  ac_jsono_t *tail;
  ac_pool_t *pool;
};

typedef struct {
  uint32_t type;
  uint32_t num_entries;
  ac_json_t *parent;
  ac_jsona_t **array;
  ac_jsona_t *head;
  ac_jsona_t *tail;
  ac_pool_t *pool;
} _ac_jsona_t;

static inline ac_json_t *ac_jsono(ac_pool_t *pool) {
  _ac_jsono_t *obj = (_ac_jsono_t *)ac_pool_calloc(pool, sizeof(_ac_jsono_t));
  obj->type = AC_JSON_OBJECT;
  obj->pool = pool;
  return (ac_json_t *)obj;
}

static inline ac_json_t *ac_jsona(ac_pool_t *pool) {
  _ac_jsona_t *a = (_ac_jsona_t *)ac_pool_calloc(pool, sizeof(_ac_jsona_t));
  a->type = AC_JSON_ARRAY;
  a->pool = pool;
  return (ac_json_t *)a;
}

static inline void _ac_jsona_fill(_ac_jsona_t *arr) {
  arr->array = (ac_jsona_t **)ac_pool_alloc(arr->pool, sizeof(ac_jsona_t *) *
                                                           arr->num_entries);
  ac_jsona_t **awp = arr->array;
  ac_jsona_t *n = arr->head;
  while (n) {
    *awp++ = n;
    n = n->next;
  }
  arr->num_entries = awp - arr->array;
}

static inline ac_json_t *ac_jsona_nth(ac_json_t *j, int nth) {
  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  if (nth >= arr->num_entries)
    return NULL;
  if (!arr->array)
    _ac_jsona_fill(arr);
  return arr->array[nth]->value;
}

static inline ac_jsona_t *ac_jsona_nth_node(ac_json_t *j, int nth) {
  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  if (nth >= arr->num_entries)
    return NULL;
  if (!arr->array)
    _ac_jsona_fill(arr);
  return arr->array[nth];
}

static inline ac_json_t *ac_jsona_scan(ac_json_t *j, int nth) {
  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  if (nth >= arr->num_entries)
    return NULL;
  if ((nth << 1) > arr->num_entries) {
    nth = arr->num_entries - nth;
    nth--;
    ac_jsona_t *n = arr->tail;
    while (nth) {
      nth--;
      n = n->previous;
    }
    return n->value;
  } else {
    ac_jsona_t *n = arr->head;
    while (nth) {
      nth--;
      n = n->next;
    }
    return n->value;
  }
}

static inline void ac_jsona_append(ac_json_t *j, ac_json_t *item) {
  if (!item)
    return;

  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  ac_jsona_t *n = (ac_jsona_t *)ac_pool_alloc(arr->pool, sizeof(*n));
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

static inline int ac_jsona_count(ac_json_t *j) {
  if(!j) return 0;
  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  return arr->num_entries;
}

static inline ac_jsona_t *ac_jsona_first(ac_json_t *j) {
  if(!j) return NULL;
  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  return arr->head;
}

static inline ac_jsona_t *ac_jsona_last(ac_json_t *j) {
  if(!j) return NULL;
  _ac_jsona_t *arr = (_ac_jsona_t *)j;
  return arr->tail;
}

static inline ac_jsona_t *ac_jsona_next(ac_jsona_t *j) { return j->next; }

static inline ac_jsona_t *ac_jsona_previous(ac_jsona_t *j) {
  return j->previous;
}

static inline void ac_jsona_erase(ac_jsona_t *n) {
  _ac_jsona_t *arr = (_ac_jsona_t *)(n->value->parent);
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

static inline int ac_jsono_count(ac_json_t *j) {
  if(!j)
    return 0;
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  return o->num_entries;
}

static inline ac_jsono_t *ac_jsono_first(ac_json_t *j) {
  if(!j)
    return NULL;
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  return o->head;
}

static inline ac_jsono_t *ac_jsono_last(ac_json_t *j) {
  if(!j)
    return NULL;
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  return o->tail;
}

static inline ac_jsono_t *ac_jsono_next(ac_jsono_t *j) { return j->next; }

static inline ac_jsono_t *ac_jsono_previous(ac_jsono_t *j) {
  return j->previous;
}

static inline void ac_jsono_erase(ac_jsono_t *n) {
  _ac_jsono_t *o = (_ac_jsono_t *)(n->value->parent);
  o->num_entries--;
  if (o->root) {
    if (o->num_sorted_entries) {
      o->root = NULL;
      o->num_sorted_entries = 0;
    } else
      macro_map_erase(&o->root, (macro_map_t *)n);
  }
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

void _ac_jsono_fill(_ac_jsono_t *o);

static inline ac_jsono_t *ac_jsono_get_node(ac_json_t *j, const char *key) {
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  if (!o->root) {
    if (o->head)
      _ac_jsono_fill(o);
    else
      return NULL;
  }
  ac_jsono_t **res = __ac_json_search((char *)key, (const ac_jsono_t **)o->root,
                                      o->num_sorted_entries);
  return *res;
}

static inline ac_json_t *ac_jsono_get(ac_json_t *j, const char *key) {
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  if (!o->root) {
    if (o->head)
      _ac_jsono_fill(o);
    else
      return NULL;
  }
  ac_jsono_t **res = __ac_json_search(key, (const ac_jsono_t **)o->root,
                                      o->num_sorted_entries);
  if (res) {
    ac_jsono_t *r = *res;
    if (r)
      return r->value;
  }
  return NULL;
}

static inline ac_json_t *ac_jsono_scanr(ac_json_t *j, const char *key) {
  if (!j || j->type != AC_JSON_OBJECT)
    return NULL;
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  ac_jsono_t *r = o->tail;
  while (r) {
    if (!strcmp(r->key, key))
      return r->value;
    r = r->previous;
  }
  return NULL;
}

static inline ac_json_t *ac_jsono_scan(ac_json_t *j, const char *key) {
  if (!j || j->type != AC_JSON_OBJECT)
    return NULL;
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  ac_jsono_t *r = o->head;
  while (r) {
    if (!strcmp(r->key, key))
      return r->value;
    r = r->next;
  }
  return NULL;
}

static inline int parse_int(const char *str, int default_value) {
    if (!str) return default_value;

    int num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;

            /*
            if (*str < '0' || *str > '9') {
                // Not a valid digit
                return default_value;
            }
            // Shift the current number by 10 (to make space for the new digit)
            // and add the new digit to the number
            num = num * 10 + (*str - '0');
            str++;
            */
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
            /*
            if (*str < '0' || *str > '9') {
                // Not a valid digit
                return default_value;
            }
            // Shift the current number by 10 (to make space for the new digit)
            // and add the new digit to the number
            num = num * 10 + (*str - '0');
            str++;
            */
        }
    }
    return num;
}


static inline int ac_jsono_scan_int(ac_json_t *j, const char *key, int default_value) {
    return parse_int(ac_jsonv(ac_jsono_scan(j, key)), default_value);
}

static inline int32_t parse_int32(const char *str, int32_t default_value) {
    if (!str) return default_value;

    int32_t num = 0;

    int ch = *str;
    if(*str == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline int32_t ac_jsono_scan_int32(ac_json_t *j, const char *key, int32_t default_value) {
    return parse_int32(ac_jsonv(ac_jsono_scan(j, key)), default_value);
}

static inline uint32_t parse_uint32(const char *str, uint32_t default_value) {
    if (!str) return default_value;

    uint32_t num = 0;

    int ch = *str;
    while (ch) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            str++;
            ch = *str;
            continue;
        }
        // Not a valid digit
        return default_value;
    }

    return num;
}


static inline uint32_t ac_jsono_scan_uint32(ac_json_t *j, const char *key, uint32_t default_value) {
    return parse_uint32(ac_jsonv(ac_jsono_scan(j, key)), default_value);
}

static inline int64_t parse_int64(const char *str, int64_t default_value) {
    if (!str) return default_value;

    int64_t num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline int64_t ac_jsono_scan_int64(ac_json_t *j, const char *key, int64_t default_value) {
    return parse_int64(ac_jsonv(ac_jsono_scan(j, key)), default_value);
}

static inline uint64_t parse_uint64(const char *str, uint64_t default_value) {
    if (!str) return default_value;

    uint64_t num = 0;

    int ch = *str;
    while (ch) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            str++;
            ch = *str;
            continue;
        }
        // Not a valid digit
        return default_value;
    }

    return num;
}

static inline uint64_t ac_jsono_scan_uint64(ac_json_t *j, const char *key, uint64_t default_value) {
    return parse_uint64(ac_jsonv(ac_jsono_scan(j, key)), default_value);
}

static inline char *ac_jsono_scan_str(ac_json_t *j, const char *key, const char *default_value) {
    char *value = ac_jsonv(ac_jsono_scan(j, key));
    return value ? value : (char *)default_value;
}

static inline char *ac_jsono_scan_strd(ac_pool_t *pool, ac_json_t *j,
                                             const char *key,
                                             const char *default_value) {
    char *value = ac_jsond(pool, ac_jsono_scan(j, key));
    return value ? value : (char *)default_value;
}


static inline void _ac_jsono_fill_tree(_ac_jsono_t *o) {
  ac_jsono_t *r = o->head;
  o->root = NULL;
  o->num_sorted_entries = 0;
  while (r) {
    __ac_json_insert(&(o->root), r);
    r = r->next;
  }
}

static inline ac_jsono_t *ac_jsono_find(ac_json_t *j, const char *key) {
  _ac_jsono_t *o = (_ac_jsono_t *)j;
  if (!o->root || o->num_sorted_entries) {
    if (o->head)
      _ac_jsono_fill_tree(o);
    else
      return NULL;
  }
  return __ac_json_find(o->root, key);
}

static inline ac_jsono_t *ac_jsono_insert(ac_json_t *j, const char *key,
                                          ac_json_t *item, bool copy_key) {
  if (!item)
    return NULL;
  ac_jsono_t *res = ac_jsono_find(j, key);
  if (res) {
    item->parent = j;
    res->value = item;
  } else {
    ac_jsono_append(j, key, item, copy_key);
    _ac_jsono_t *o = (_ac_jsono_t *)j;
    __ac_json_insert(&(o->root), o->tail);
  }
  return res;
}

static inline void ac_jsono_append(ac_json_t *j, const char *key,
                                   ac_json_t *item, bool copy_key) {
  if (!item)
    return;

  _ac_jsono_t *o = (_ac_jsono_t *)j;
  ac_jsono_t *on;
  if (copy_key) {
    on = (ac_jsono_t *)ac_pool_calloc(o->pool,
                                      sizeof(ac_jsono_t) + strlen(key) + 1);
    on->key = (char *)(on + 1);
    strcpy(on->key, key);
  } else {
    on = (ac_jsono_t *)ac_pool_calloc(o->pool, sizeof(ac_jsono_t));
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

static inline ac_json_t *ac_jsono_path(ac_pool_t *pool, ac_json_t *j, const char *path) {
  size_t num_paths = 0;
  char **paths = ac_pool_split2(pool, &num_paths, '.', path);
  for( size_t i=0; i<num_paths; i++ ) {
    if(ac_json_is_array(j)) {
      char *value = strchr(paths[i], '=');
      if(value) {
        *value = 0;
        value++;
        ac_json_t *next = NULL;
        ac_jsona_t *iter = ac_jsona_first(j);
        while(iter) {
          char *v = ac_jsonv(ac_jsono_scan(iter->value, paths[i]));
          if(v && !strcmp(v, value)) {
            next = iter->value;
            break;
          }
          iter = ac_jsona_next(iter);
        }
        j = next;
      }
      else {
        size_t num = 0;
        if(sscanf(paths[i], "%lu", &num) != 1)
          return NULL;
        j = ac_jsona_scan(j, num);
      }
    }
    else
      j = ac_jsono_scan(j, paths[i]);

    if(!j)
      return NULL;
  }
  return j;
}

static inline char *ac_jsono_pathv(ac_pool_t *pool, ac_json_t *j, const char *path) {
  j = ac_jsono_path(pool, j, path);
  return ac_jsonv(j);
}

static inline char *ac_jsono_pathd(ac_pool_t *pool, ac_json_t *j, const char *path) {
  j = ac_jsono_path(pool, j, path);
  return ac_jsond(pool, j);
}
