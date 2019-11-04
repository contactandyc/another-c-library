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

#include <math.h>

#include "ac_buffer.h"
#include "ac_conv.h"
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
  start--;
  char *p = ac_pool_alloc(pool, length + 2);
  memcpy(p, inp + start, length - 2);
  p[length - 2] = '.';
  p[length - 1] = inp[start + length - 2];
  p[length] = inp[start + length - 1];
  p[length + 1] = 0;
  char *ep = p + length - 2;

  int negative = 0;
  if (*p == '-') {
    negative = 1;
    p++;
  }
  while (p < ep && *p == '0')
    p++;

  if (p >= ep || *p < '0' || *p > '9')
    return p - 1;

  char *result = p;
  if (negative) {
    p[-1] = '-';
    result = p - 1;
  }
  while (p < ep && *p >= '0' && *p <= '9')
    p++;

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
  return ac_json_encode(pool, p, ep - p);
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

char *get_display_price(ac_pool_t *pool, ac_json_t *root,
                        double *calculated_value, const char *for_x,
                        const char *split_price, const char *single_price) {
  *calculated_value = 0.00;

  int split = ac_int(ac_json_value(ac_json_object_get_value(root, for_x)), 0);
  if (split) {
    char *sp = ac_json_value(ac_json_object_get_value(root, split_price));
    if (!sp || !strcmp(sp, "0.00"))
      return NULL;

    double price = floor(ceil(ac_double(sp, 0.00) * 20000) / 2);
    *calculated_value = price / (split * 10000.0);
    return ac_pool_strdupf(pool, "%d for $%s", split, sp);
  } else {
    char *sp = ac_json_value(ac_json_object_get_value(root, single_price));
    if (!sp || !strcmp(sp, "0.00"))
      return NULL;
    return ac_pool_strdupf(pool, "$%s", sp);
  }
}

int main(int argc, char *argv[]) {
  ac_pool_t *pool = ac_pool_init(1000);
  ac_buffer_t *bh = ac_buffer_init(1000);
  char str[1000];
  FILE *in = stdin;
  if (argc > 1) {
    in = fopen(argv[1], "rb");
    if (!in)
      return -1;
  }
  while (fgets(str, 999, in)) {
    ac_pool_clear(pool);
    ac_buffer_clear(bh);
    ac_json_t *root = ac_json_object(pool);
    ac_json_object_append(
        root, "Product ID",
        ac_json_number_string(pool, _parse_number(pool, str, 1, 8)), false);
    ac_json_object_append(root, "Product Description",
                          ac_json_str(pool, _parse_string(pool, str, 10, 58)),
                          false);
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
    ac_json_object_append(root, "Product Size",
                          ac_json_str(pool, _parse_string(pool, str, 134, 9)),
                          false);

    char *flags = str + 123;
    for (int i = 0; i < 8; i++) {
      if (flags[i] == 'Y')
        ac_json_object_append(root, ac_pool_strdupf(pool, "F%d", i + 1),
                              ac_json_number_string(pool, "1"), false);
    }

    ac_json_t *product_record = ac_json_object(pool);

    ac_json_object_append(
        product_record, "Product ID",
        ac_json_number_string(
            pool, ac_json_value(ac_json_object_get_value(root, "Product ID"))),
        false);
    ac_json_object_append(
        product_record, "Product Description",
        ac_json_str(pool, ac_str(ac_json_value(ac_json_object_get_value(
                                     root, "Product Description")),
                                 "")),
        false);
    double regular_calculated_price = 0.0, promotional_calculated_price = 0.0;
    ac_json_object_append(
        product_record, "Regular Display Price",
        ac_json_str(pool,
                    get_display_price(pool, root, &regular_calculated_price,
                                      "Regular For X", "Regular Split Price",
                                      "Regular Singular Price")),
        false);
    if (regular_calculated_price != 0.00) {
      ac_json_object_append(
          product_record, "Regular Calculator Price",
          ac_json_str(
              pool, ac_pool_strdupf(pool, "$%0.4f", regular_calculated_price)),
          false);
    }
    ac_json_object_append(
        product_record, "Promotional Display Price",
        ac_json_str(pool, get_display_price(
                              pool, root, &promotional_calculated_price,
                              "Promotional For X", "Promotional Split Price",
                              "Promotional Singular Price")),
        false);
    if (promotional_calculated_price != 0.00) {
      ac_json_object_append(
          product_record, "Promotional Calculator Price",
          ac_json_str(pool, ac_pool_strdupf(pool, "$%0.4f",
                                            promotional_calculated_price)),
          false);
    }
    if (ac_int(ac_json_value(ac_json_object_get_value(root, "F3")), 0)) {
      ac_json_object_append(product_record, "Unit of Measure",
                            ac_json_str(pool, "Pound"), false);
    } else {
      ac_json_object_append(product_record, "Unit of Measure",
                            ac_json_str(pool, (char *)"Each"), false);
    }
    ac_json_object_append(
        product_record, "Product Size",
        ac_json_str(pool, ac_str(ac_json_value(ac_json_object_get_value(
                                     root, "Product Size")),
                                 "")),
        false);
    ac_json_object_append(product_record, "Tax Rate",
                          ac_json_str(pool, "7.75%"), false);
    // ac_json_dump_to_buffer(bh, root);
    // ac_buffer_appendc(bh, '\n');
    ac_json_dump_to_buffer(bh, product_record);
    ac_buffer_appendc(bh, '\n');
    printf("%s", ac_buffer_data(bh));
  }
  if (argc > 1)
    fclose(in);
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
  return 0;
}
