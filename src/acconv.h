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

#ifndef _acconv_H
#define _acconv_H

#include "accommon.h"

#include <inttypes.h>

/* returns value if value is not NULL, otherwise default_value */
const char *acstr(const char *value, const char *default_value);

/* returns bool if value is not NULL and valid, otherwise default_value */
bool acbool(const char *value, bool default_value);

/* returns int if value is not NULL and valid, otherwise default_value */
int acint(const char *value, int default_value);

/* returns long if value is not NULL and valid, otherwise default_value */
long aclong(const char *value, long default_value);

/* returns double if value is not NULL and valid, otherwise default_value */
double acdouble(const char *value, double default_value);

/* returns int32_t if value is not NULL and valid, otherwise default_value */
int32_t acint32_t(const char *value, int32_t default_value);

/* returns uint32_t if value is not NULL and valid, otherwise default_value */
uint32_t acuint32_t(const char *value, uint32_t default_value);

/* returns int64_t if value is not NULL and valid, otherwise default_value */
int64_t acint64_t(const char *value, int64_t default_value);

/* returns uint64_t if value is not NULL and valid, otherwise default_value */
uint64_t acuint64_t(const char *value, uint64_t default_value);

#endif
