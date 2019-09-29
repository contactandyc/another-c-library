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

#ifndef _stla_cgi_H
#define _stla_cgi_H

#include "stla_pool.h"

struct stla_cgi_s;
typedef struct stla_cgi_s stla_cgi_t;

/* initialize a cgi object using a pool (no destroy method exists) */
stla_cgi_t *stla_cgi_init(stla_pool_t *pool, const char *q);

/* get the original cgi query passed to init */
const char *stla_cgi_query(stla_cgi_t *h);

/* get a NULL terminated array of decoded values from key */
char **stla_cgi_strs(stla_cgi_t *h, const char *key);

/* get the first decoded string from key as a string */
const char *stla_cgi_str(stla_cgi_t *h, const char *key,
                         const char *default_value);

/* get the first decoded string from key as a bool */
bool stla_cgi_bool(stla_cgi_t *h, const char *key, bool default_value);

/* get the first decoded string from key as an int */
int stla_cgi_int(stla_cgi_t *h, const char *key, int default_value);

/* get the first decoded string from key as an long */
long stla_cgi_long(stla_cgi_t *h, const char *key, long default_value);

/* get the first decoded string from key as an double */
double stla_cgi_double(stla_cgi_t *h, const char *key, double default_value);

/* returns int32_t if value is not NULL and valid, otherwise default_value */
int32_t stla_cgi_int32_t(stla_cgi_t *h, const char *key, int32_t default_value);

/* returns uint32_t if value is not NULL and valid, otherwise default_value */
uint32_t stla_cgi_uint32_t(stla_cgi_t *h, const char *key,
                           uint32_t default_value);

/* returns int64_t if value is not NULL and valid, otherwise default_value */
int64_t stla_cgi_int64_t(stla_cgi_t *h, const char *key, int64_t default_value);

/* returns uint64_t if value is not NULL and valid, otherwise default_value */
uint64_t stla_cgi_uint64_t(stla_cgi_t *h, const char *key,
                           uint64_t default_value);

/* decode cgi text */
char *stla_cgi_decode(stla_pool_t *pool, char *s);

#endif
