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

/*
  ac_conv is a collection of conversion functions to convert strings (const
  char *) to various types.
*/

#ifndef _ac_conv_H
#define _ac_conv_H

#include "another-c-library/ac_common.h"

#include <inttypes.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* returns a 64 bit hash of data */
uint64_t ac_hash64(const void *data, size_t len);

/* returns a 64 bit representation of md5 */
uint64_t ac_md5(const void *s, size_t len);
uint64_t ac_md5_str(const char *s);


/* returns value if value is not NULL, otherwise default_value */
const char *ac_str(const char *value, const char *default_value);

/* returns bool if value is not NULL and valid, otherwise default_value */
bool ac_bool(const char *value, bool default_value);

/* returns int if value is not NULL and valid, otherwise default_value */
int ac_int(const char *value, int default_value);

/* returns long if value is not NULL and valid, otherwise default_value */
long ac_long(const char *value, long default_value);

/* returns double if value is not NULL and valid, otherwise default_value */
double ac_double(const char *value, double default_value);

/* returns int32_t if value is not NULL and valid, otherwise default_value */
int32_t ac_int32_t(const char *value, int32_t default_value);

/* returns uint32_t if value is not NULL and valid, otherwise default_value */
uint32_t ac_uint32_t(const char *value, uint32_t default_value);

/* returns int64_t if value is not NULL and valid, otherwise default_value */
int64_t ac_int64_t(const char *value, int64_t default_value);

/* returns uint64_t if value is not NULL and valid, otherwise default_value */
uint64_t ac_uint64_t(const char *value, uint64_t default_value);

/* fills dest with gmt time in format of YYYY-MM-DD hh:mm:ss and must be at
 * least 20 bytes (19+0 terminator).  Returns a pointer to dest. */
char *ac_date_time(char *dest, time_t ts);

/* fills dest with gmt time in format of YYYY-MM-DD and must be at
 * least 11 bytes (10+0 terminator).  Returns a pointer to dest. */
char *ac_date(char *dest, time_t ts);

/* converts a yyyy-mm-dd format to a time_t */
time_t ac_date_as_time_t(const char *value, time_t default_value);

/* converts a hh:mm:ss format to a time_t */
time_t ac_time_of_day_as_time_t(const char *value, time_t default_value);

/* converts a yyyy-mm-dd?hh:mm:ss format to a time_t */
time_t ac_date_time_as_time_t(const char *value, time_t default_value);

#ifdef __cplusplus
}
#endif

#endif
