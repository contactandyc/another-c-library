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

#ifndef _stla_conv_H
#define _stla_conv_H

#include "stla_common.h"

#include <inttypes.h>

/* returns value if value is not NULL, otherwise default_value */
const char *stla_str(const char *value, const char *default_value);

/* returns bool if value is not NULL and valid, otherwise default_value */
bool stla_bool(const char *value, bool default_value);

/* returns int if value is not NULL and valid, otherwise default_value */
int stla_int(const char *value, int default_value);

/* returns long if value is not NULL and valid, otherwise default_value */
long stla_long(const char *value, long default_value);

/* returns double if value is not NULL and valid, otherwise default_value */
double stla_double(const char *value, double default_value);

/* returns int32_t if value is not NULL and valid, otherwise default_value */
int32_t stla_int32_t(const char *value, int32_t default_value);

/* returns uint32_t if value is not NULL and valid, otherwise default_value */
uint32_t stla_uint32_t(const char *value, uint32_t default_value);

/* returns int64_t if value is not NULL and valid, otherwise default_value */
int64_t stla_int64_t(const char *value, int64_t default_value);

/* returns uint64_t if value is not NULL and valid, otherwise default_value */
uint64_t stla_uint64_t(const char *value, uint64_t default_value);

#endif
