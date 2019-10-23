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

#ifndef _ac_cgi_H
#define _ac_cgi_H

#include "ac_pool.h"

#ifdef __cplusplus
extern "C" {
#endif


struct ac_cgi_s;
typedef struct ac_cgi_s ac_cgi_t;

/* initialize a cgi object using a pool (no destroy method exists) */
ac_cgi_t *ac_cgi_init(ac_pool_t *pool, const char *q);

/* get the original cgi query passed to init */
const char *ac_cgi_query(ac_cgi_t *h);

/* get a NULL terminated array of decoded values from key */
char **ac_cgi_strs(ac_cgi_t *h, const char *key);

/* get the first decoded string from key as a string */
const char *ac_cgi_str(ac_cgi_t *h, const char *key,
                         const char *default_value);

/* get the first decoded string from key as a bool */
bool ac_cgi_bool(ac_cgi_t *h, const char *key, bool default_value);

/* get the first decoded string from key as an int */
int ac_cgi_int(ac_cgi_t *h, const char *key, int default_value);

/* get the first decoded string from key as an long */
long ac_cgi_long(ac_cgi_t *h, const char *key, long default_value);

/* get the first decoded string from key as an double */
double ac_cgi_double(ac_cgi_t *h, const char *key, double default_value);

/* returns int32_t if value is not NULL and valid, otherwise default_value */
int32_t ac_cgi_int32_t(ac_cgi_t *h, const char *key, int32_t default_value);

/* returns uint32_t if value is not NULL and valid, otherwise default_value */
uint32_t ac_cgi_uint32_t(ac_cgi_t *h, const char *key,
                           uint32_t default_value);

/* returns int64_t if value is not NULL and valid, otherwise default_value */
int64_t ac_cgi_int64_t(ac_cgi_t *h, const char *key, int64_t default_value);

/* returns uint64_t if value is not NULL and valid, otherwise default_value */
uint64_t ac_cgi_uint64_t(ac_cgi_t *h, const char *key,
                           uint64_t default_value);

/* decode cgi text */
char *ac_cgi_decode(ac_pool_t *pool, char *s);

#ifdef __cplusplus
}
#endif

#endif
