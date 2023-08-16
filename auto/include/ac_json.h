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
#include "ac_pool.h"

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define AC_JSON_DEBUG
//#define AC_JSON_FILL_TEST
//#define AC_JSON_SORT_TEST

struct ac_json_s;
typedef struct ac_json_s ac_json_t;
struct ac_jsona_s;
typedef struct ac_jsona_s ac_jsona_t;
struct ac_jsono_s;
typedef struct ac_jsono_s ac_jsono_t;

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

/* null, bool_false (to not conflict with keyword false), and zero are all
   grouped together on purpose as they all respresent false like values.
   number, decimal, and bool_true are grouped because they all respresent
   true like values.  The string and binary type may also represent true and
   false values (if one were considering t=true, f=false for example). */
typedef enum {
  error = 0,
  object = 1,
  array = 2,
  binary = 3,
  null = 4,
  string = 5,
  bool_false = 6,
  zero = 7,
  number = 8,
  decimal = 9,
  bool_true = 10
} ac_json_type_t;

/* Depending upon the task, it may be handy to know what type the json is.
   Internally, all json is stored as strings and converted on demand. */
static inline ac_json_type_t ac_json_type(ac_json_t *j);

/* Dump the json to a file or to a buffer */
void ac_json_dump(FILE *out, ac_json_t *a);
void ac_json_dump_to_buffer(ac_buffer_t *bh, ac_json_t *a);

/* Decode encoded json text */
char *ac_json_decode(ac_pool_t *pool, char *s, size_t length);

/* Decode encoded json text and return the length of the decoded string.  This
   allows for binary data to be encoded. */
char *ac_json_decode2(size_t *rlen, ac_pool_t *pool, char *s, size_t length);

/* Encode json text */
char *ac_json_encode(ac_pool_t *pool, char *s, size_t length);

/* returns NULL if object, array, or error */
static inline char *ac_jsond(ac_pool_t *pool, ac_json_t *j);
static inline char *ac_jsonv(ac_json_t *j);
static inline char *ac_jsonb(ac_json_t *j, size_t *length);

static inline ac_json_t *ac_jsono(ac_pool_t *pool);
static inline ac_json_t *ac_jsona(ac_pool_t *pool);

static inline ac_json_t *ac_json_binary(ac_pool_t *pool, char *s,
                                        size_t length);
static inline ac_json_t *ac_json_string(ac_pool_t *pool, const char *s,
                                        size_t length);
static inline ac_json_t *ac_json_str(ac_pool_t *pool, const char *s);
static inline ac_json_t *ac_json_encode_string(ac_pool_t *pool,
                                               const char *s, size_t length);
static inline ac_json_t *ac_json_encode_str(ac_pool_t *pool, const char *s);
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

static inline ac_json_t *ac_jsono_path(ac_pool_t *pool, ac_json_t *j, const char *path);
static inline char *ac_jsono_pathv(ac_pool_t *pool, ac_json_t *j, const char *path);
static inline char *ac_jsono_pathd(ac_pool_t *pool, ac_json_t *j, const char *path);


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

static inline ac_json_t *ac_jsono_scan(ac_json_t *j, const char *key);
static inline ac_json_t *ac_jsono_scanr(ac_json_t *j, const char *key);

static inline int ac_jsono_scan_int(ac_json_t *j, const char *key, int default_value);
static inline int32_t ac_jsono_scan_int32(ac_json_t *j, const char *key, int32_t default_value);
static inline uint32_t ac_jsono_scan_uint32(ac_json_t *j, const char *key, uint32_t default_value);
static inline int64_t ac_jsono_scan_int64(ac_json_t *j, const char *key, int64_t default_value);
static inline uint64_t ac_jsono_scan_uint64(ac_json_t *j, const char *key, uint64_t default_value);
static inline char *ac_jsono_scan_str(ac_json_t *j, const char *key, const char *default_value);
static inline char *ac_jsono_scan_strd(ac_pool_t *pool, ac_json_t *j,
                                       const char *key,
                                       const char *default_value);

/* use _get if json is meant to be read only and _find if not.
  The ac_jsono_get method is faster than the ac_jsono_find
  method as it creates a sorted array vs a red black tree (or map).  The
  find/insert methods are useful if you need to lookup keys and insert.

  ac_jsono_get/get_node/find will not find items which are appended.
*/
static inline ac_json_t *ac_jsono_get(ac_json_t *j, const char *key);
static inline ac_jsono_t *ac_jsono_get_node(ac_json_t *j, const char *key);

/* in this case, don't use _get (only _find) */
static inline void ac_jsono_erase(ac_jsono_t *n);
static inline ac_jsono_t *ac_jsono_insert(ac_json_t *j, const char *key,
                                          ac_json_t *item, bool copy_key);
static inline ac_jsono_t *ac_jsono_find(ac_json_t *j, const char *key);

#define AC_JSON_PARSE_STRING(j, resp, name) resp->name = (char *)ac_jsond(pool, ac_jsono_scan(j, #name)); if(!resp->name) { resp->name = (char *)""; }
#define AC_JSON_PARSE_STRING_TEST(j, resp, name) resp->name = (char *)ac_jsond(pool, ac_jsono_scan(j, #name)); if(!resp->name) { return NULL; }
#define AC_JSON_PARSE_UINT32(j, resp, name) resp->name = ac_uint32_t(ac_jsonv(ac_jsono_scan(j, #name)), 0)
#define AC_JSON_PARSE_DOUBLE(j, resp, name) resp->name = ac_double(ac_jsonv(ac_jsono_scan(j, #name)), 0.0)

#include "ac-json/ac_json.h"

#ifdef __cplusplus
}
#endif

#endif
