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

#ifndef _accgi_H
#define _accgi_H

#include "acpool.h"

struct accgi_s;
typedef struct accgi_s accgi_t;

/* initialize a cgi object using a pool (no destroy method exists) */
accgi_t *accgi_init(acpool_t *pool, const char *q);

/* get the original cgi query passed to init */
const char *accgi_query(accgi_t *h);

/* get a NULL terminated array of decoded values from key */
char **accgi_strs(accgi_t *h, const char *key);

/* get the first decoded string from key as a string */
const char *accgi_str(accgi_t *h, const char *key,
                         const char *default_value);

/* get the first decoded string from key as a bool */
bool accgi_bool(accgi_t *h, const char *key, bool default_value);

/* get the first decoded string from key as an int */
int accgi_int(accgi_t *h, const char *key, int default_value);

/* get the first decoded string from key as an long */
long accgi_long(accgi_t *h, const char *key, long default_value);

/* get the first decoded string from key as an double */
double accgi_double(accgi_t *h, const char *key, double default_value);

/* returns int32_t if value is not NULL and valid, otherwise default_value */
int32_t accgi_int32_t(accgi_t *h, const char *key, int32_t default_value);

/* returns uint32_t if value is not NULL and valid, otherwise default_value */
uint32_t accgi_uint32_t(accgi_t *h, const char *key,
                           uint32_t default_value);

/* returns int64_t if value is not NULL and valid, otherwise default_value */
int64_t accgi_int64_t(accgi_t *h, const char *key, int64_t default_value);

/* returns uint64_t if value is not NULL and valid, otherwise default_value */
uint64_t accgi_uint64_t(accgi_t *h, const char *key,
                           uint64_t default_value);

/* decode cgi text */
char *accgi_decode(acpool_t *pool, char *s);

#endif
