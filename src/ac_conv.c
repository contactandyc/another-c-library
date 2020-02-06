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

#include "ac_conv.h"
#include "lz4/xxhash.h"

#include <stdio.h>

uint64_t ac_hash64(const void *s, size_t len) {
  return (uint64_t)XXH64(s, len, 0);
}

char *ac_date_time(char *dest, time_t ts) {
  struct tm t;
  gmtime_r(&ts, &t);
  sprintf(dest, "%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1,
          t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
  return dest;
}

char *ac_date(char *dest, time_t ts) {
  struct tm t;
  gmtime_r(&ts, &t);
  sprintf(dest, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
  return dest;
}

time_t ac_date_as_time_t(const char *value, time_t default_value) {
  if (value[0] >= '0' && value[0] <= '9' && value[1] >= '0' &&
      value[1] <= '9' && value[2] >= '0' && value[2] <= '9' &&
      value[3] >= '0' && value[3] <= '9' && value[4] == '-' &&
      value[5] >= '0' && value[5] <= '9' && value[6] >= '0' &&
      value[6] <= '9' && value[7] == '-' && value[8] >= '0' &&
      value[8] <= '9' && value[9] >= '0' && value[9] <= '9') {
    uint32_t year = ((value[0] - '0') * 1000) + ((value[1] - '0') * 100) +
                    ((value[2] - '0') * 10) + (value[3] - '0');
    uint32_t month = ((value[5] - '0') * 10) + (value[6] - '0');
    uint32_t day = ((value[8] - '0') * 10) + (value[9] - '0');
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = year - 1900;
    t.tm_mon = month;
    t.tm_mday = day;
    return mktime(&t);
  }
  return default_value;
}

const char *ac_str(const char *value, const char *default_value) {
  if (value)
    return value;
  return default_value;
}

bool ac_bool(const char *value, bool default_value) {
  if (!value)
    return default_value;

  if (default_value) {
    if (*value == '0' || *value == 'f' || *value == 'F')
      return false;
    return true;
  } else {
    if (*value == '1' || *value == 't' || *value == 'T')
      return true;
    return false;
  }
}

int ac_int(const char *value, int default_value) {
  if (!value)
    return default_value;

  int res = 0;
  if (sscanf(value, "%d", &res) == 1)
    return res;
  return default_value;
}

long ac_long(const char *value, long default_value) {
  if (!value)
    return default_value;

  long res = 0;
  if (sscanf(value, "%ld", &res) == 1)
    return res;
  return default_value;
}

double ac_double(const char *value, double default_value) {
  if (!value)
    return default_value;

  double res = 0;
  if (sscanf(value, "%lf", &res) == 1)
    return res;
  return default_value;
}

int32_t ac_int32_t(const char *value, int32_t default_value) {
  if (!value)
    return default_value;

  int32_t res = 0;
  if (sscanf(value, "%d", &res) == 1)
    return res;
  return default_value;
}

uint32_t ac_uint32_t(const char *value, uint32_t default_value) {
  if (!value)
    return default_value;

  uint32_t res = 0;
  if (sscanf(value, "%u", &res) == 1)
    return res;
  return default_value;
}

int64_t ac_int64_t(const char *value, int64_t default_value) {
  if (!value)
    return default_value;

  int64_t res = 0;
  if (sscanf(value, "%lld", &res) == 1)
    return res;
  return default_value;
}

uint64_t ac_uint64_t(const char *value, uint64_t default_value) {
  if (!value)
    return default_value;

  uint64_t res = 0;
  if (sscanf(value, "%llu", &res) == 1)
    return res;
  return default_value;
}
