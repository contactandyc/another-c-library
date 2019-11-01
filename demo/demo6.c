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

#include "ac_json.h"
#include "ac_pool.h"

static char *_parse_number(ac_pool_t *pool, char *inp, size_t start,
                           size_t length) {
  char *p = ac_pool_strndup(pool, inp + start - 1, length);
  char *ep = p + length;

  int negative = 0;
  if (*p == '-') {
    negative = 1;
    p++;
  }
  while (p < ep && *p == '0')
    p++;

  if (p >= ep || *p < '0' || *p > '9') {
    return p - 1;
  }

  char *result = p;
  if (negative) {
    p[-1] = '-';
    result = p - 1;
  }
  while (p < ep && *p >= '0' && *p <= '9')
    p++;
  *p = 0;
  return result;
}

static char *_parse_currency(ac_pool_t *pool, char *inp, size_t start,
                             size_t length) {
  char *p = ac_pool_strndup(pool, inp + start - 1, length + 1);
  char *ep = p + length - 2;

  int negative = 0;
  if (*p == '-') {
    negative = 1;
    p++;
  }
  while (p < ep && *p == '0')
    p++;

  if (p >= ep || *p < '0' || *p > '9') {
    p[3] = 0;
    p[2] = p[1];
    p[1] = p[0];
    p[0] = '.';
    return p - 1;
  }

  char *result = p;
  if (negative) {
    p[-1] = '-';
    result = p - 1;
  }
  while (p < ep && *p >= '0' && *p <= '9')
    p++;

  p[3] = 0;
  p[2] = p[1];
  p[1] = p[0];
  p[0] = '.';
  return result;
}

static char *_parse_string(ac_pool_t *pool, char *inp, size_t start,
                           size_t length) {
  char *p = ac_pool_strndup(pool, inp + start - 1, length);
  char *ep = p + length;

  while (p < ep && *p == ' ')
    p++;
  while (ep > p && ep[-1] == ' ')
    ep--;
  *ep = 0;
  return p;
}

/*
        1	8	Product ID	Number
        10	68	Product Description	String
        70	77	Regular Singular Price	Currency
        79	86	Promotional Singular Price	Currency
        88	95	Regular Split Price	Currency
        97	104	Promotional Split Price	Currency
        106	113	Regular For X	Number
        115	122	Promotional For X	Number
        124	132	Flags	Flags
        134	142	Product Size	String
  */

int main(int argc, char *argv[]) {
  ac_pool_t *pool = ac_pool_init(1000);
  char str[1000];
  while (fgets(str, 999, stdin)) {
    ac_pool_clear(pool);
    ac_json_t *root = ac_json_object(pool);
    ac_json_object_append(
        root, "Product ID",
        ac_json_number_string(pool, _parse_number(pool, str, 1, 8)), false);
    ac_json_object_append(
        root, "Product Description",
        ac_json_string(pool, _parse_string(pool, str, 10, 58), false), false);
    ac_json_object_append(
        root, "Regular Singular Price",
        ac_json_decimal_string(pool, _parse_currency(pool, str, 70, 8)), false);
    ac_json_object_append(
        root, "Promotional Singular Price",
        ac_json_decimal_string(pool, _parse_currency(pool, str, 79, 8)), false);
    ac_json_object_append(
        root, "Regular Split Price",
        ac_json_decimal_string(pool, _parse_currency(pool, str, 88, 8)), false);
    ac_json_object_append(
        root, "Promotional Split Price",
        ac_json_decimal_string(pool, _parse_currency(pool, str, 97, 8)), false);
    ac_json_object_append(
        root, "Regular For X",
        ac_json_number_string(pool, _parse_number(pool, str, 106, 8)), false);
    ac_json_object_append(
        root, "Promotional For X",
        ac_json_number_string(pool, _parse_number(pool, str, 115, 8)), false);
    ac_json_object_append(
        root, "Product Size",
        ac_json_string(pool, _parse_string(pool, str, 134, 9), false), false);

    char *flags = str + 123;
    for (int i = 0; i < 8; i++) {
      if (flags[i] == 'Y')
        ac_json_object_append(root, ac_pool_strdupf(pool, "F%d", i + 1),
                              ac_json_true(pool), false);
    }
    ac_json_dump(stdout, root);
    printf("\n");
  }
  ac_pool_destroy(pool);
  return 0;
}
