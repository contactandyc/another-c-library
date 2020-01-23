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

#ifndef _ac_json_H
#define _ac_json_H

#include "ac_buffer.h"
#include "ac_map.h"
#include "ac_pool.h"

#include <inttypes.h>

//#define AC_JSON_DEBUG
//#define AC_JSON_FILL_TEST
//#define AC_JSON_SORT_TEST

struct ac_json_s;
typedef struct ac_json_s ac_json_t;
struct ac_jsona_s;
typedef struct ac_jsona_s ac_jsona_t;
struct ac_jsono_s;
typedef struct ac_jsono_s ac_jsono_t;

struct ac_json_s {
  uint32_t type;
  uint32_t length;
  ac_json_t *parent;
  char *value;
};

typedef struct ac_jsona_s {
  ac_json_t *value;
  ac_jsona_t *next;
  ac_jsona_t *previous;
} ac_jsona_t;

typedef struct ac_jsono_s {
  ac_map_t map;
  char *key;
  ac_json_t *value;
  ac_jsono_t *next;
  ac_jsono_t *previous;
} ac_jsono_t;

/* This is the core function for parsing json.  This parser is not fully
 * compliant in that keys are expected to not include encodings (or if they do,
 * then you must encode the keys in the same way to match). */
ac_json_t *ac_json_parse(ac_pool_t *pool, char *p, char *ep);

/* If the parse fails, the value returned will be marked such that
 * ac_json_error returns true.  If this happens, you can dump it to the screen
 * or to a buffer.
 */
static inline bool ac_json_is_error(ac_json_t *j);
void ac_json_dump_error(FILE *out, ac_json_t *j);
void ac_json_dump_error_to_buffer(ac_buffer_t *bh, ac_json_t *j);

/* Dump the json to a file or to a buffer */
void ac_json_dump(FILE *out, ac_json_t *a);
void ac_json_dump_to_buffer(ac_buffer_t *bh, ac_json_t *a);

/* Decode encoded json text */
char *ac_json_decode(ac_pool_t *pool, char *s, size_t length);

/* Encode json text */
char *ac_json_encode(ac_pool_t *pool, char *s, size_t length);

/* returns NULL if object, array, or error */
static inline char *ac_json_decoded(ac_pool_t *pool, ac_json_t *j);
static inline char *ac_json_value(ac_json_t *j);

static inline ac_json_t *ac_jsono(ac_pool_t *pool);
static inline ac_json_t *ac_jsona(ac_pool_t *pool);

static inline ac_json_t *ac_json_binary(ac_pool_t *pool, char *s,
                                        size_t length);
static inline ac_json_t *ac_json_string(ac_pool_t *pool, const char *s,
                                        size_t length);
static inline ac_json_t *ac_json_str(ac_pool_t *pool, const char *s);
static inline ac_json_t *ac_json_true(ac_pool_t *pool);
static inline ac_json_t *ac_json_false(ac_pool_t *pool);
static inline ac_json_t *ac_json_null(ac_pool_t *pool);
static inline ac_json_t *ac_json_zero(ac_pool_t *pool);

static inline ac_json_t *ac_json_number(ac_pool_t *pool, ssize_t n);
static inline ac_json_t *ac_json_number_string(ac_pool_t *pool, char *s);
static inline ac_json_t *ac_json_decimal_string(ac_pool_t *pool, char *s);

/* Determine if current json object is an object, array, or something else */
static inline bool ac_json_is_object(ac_json_t *j);
static inline bool ac_json_is_array(ac_json_t *j);

static inline int ac_jsona_count(ac_json_t *j);
static inline ac_json_t *ac_jsona_scan(ac_json_t *j, int nth);

static inline ac_jsona_t *ac_jsona_first(ac_json_t *j);
static inline ac_jsona_t *ac_jsona_last(ac_json_t *j);
static inline ac_jsona_t *ac_jsona_next(ac_jsona_t *j);
static inline ac_jsona_t *ac_jsona_previous(ac_jsona_t *j);

/* These functions cause an internal direct access table to be created.  This
 * would be more efficient if accessing many different array elements.
 * ac_jsona_erase and ac_jsona_append will destroy the direct
 * access table.  Care should be taken when calling append frequently and nth
 * or nth_node. */
static inline ac_json_t *ac_jsona_nth(ac_json_t *j, int nth);
static inline ac_jsona_t *ac_jsona_nth_node(ac_json_t *j, int nth);
static inline void ac_jsona_erase(ac_jsona_t *n);
static inline void ac_jsona_append(ac_json_t *j, ac_json_t *item);

static inline int ac_jsono_count(ac_json_t *j);
static inline ac_jsono_t *ac_jsono_first(ac_json_t *j);
static inline ac_jsono_t *ac_jsono_last(ac_json_t *j);
static inline ac_jsono_t *ac_jsono_next(ac_jsono_t *j);
static inline ac_jsono_t *ac_jsono_previous(ac_jsono_t *j);

/* append doesn't lookup key prior to inserting, so it should be used with
 * caution.  It is more efficient because it doesn't need to lookup key or
 * maintain a tree. */
static inline void ac_jsono_append(ac_json_t *j, const char *key,
                                   ac_json_t *item, bool copy_key);

static inline ac_jsono_t *ac_jsono_scan(ac_json_t *j, const char *key);

/* use _get if json is meant to be read only and _find if not.
  The ac_jsono_get method is faster than the ac_jsono_find
  method as it creates a sorted array vs a red black tree (or map).  The
  find/insert methods are useful if you need to lookup keys and insert.
*/
static inline ac_jsono_t *ac_jsono_get(ac_json_t *j, const char *key);
static inline ac_json_t *ac_jsono_get_value(ac_json_t *j, const char *key);

/* in this case, don't use _get (only _find) */
static inline void ac_jsono_erase(ac_jsono_t *n);
static inline ac_jsono_t *ac_jsono_insert(ac_json_t *j, const char *key,
                                          ac_json_t *item, bool copy_key);
static inline ac_jsono_t *ac_jsono_find(ac_json_t *j, const char *key);

#include "impl/ac_json.h"

#endif
