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

#include <stdio.h>
#include <stdlib.h>

#define ac_sort_print_def(name, type)                                          \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el));

#define ac_sort_print_arg_def(name, type)                                      \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el, void *arg), void *arg);

#define ac_sort_print_m(name, type)                                            \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el)) {                                \
    if (func_line)                                                             \
      printf("%s ", func_line);                                                \
    printf("%ld elements\n", num_elements);                                    \
    type *ep = base + num_elements;                                            \
    while (base < ep) {                                                        \
      print_element(base);                                                     \
      base++;                                                                  \
    }                                                                          \
    printf("\n");                                                              \
  }

#define ac_sort_print_arg_m(name, type)                                        \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el, void *arg), void *arg) {          \
    if (func_line)                                                             \
      printf("%s ", func_line);                                                \
    printf("%ld elements\n", num_elements);                                    \
    type *ep = base + num_elements;                                            \
    while (base < ep) {                                                        \
      print_element(base, arg);                                                \
      base++;                                                                  \
    }                                                                          \
    printf("\n");                                                              \
  }

#define ac_sort_test_def(name, type)                                           \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el));

#define ac_sort_test_arg_def(name, type)                                       \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el, void *arg), void *arg);

#define ac_sort_test_compare_def(name, type)                                   \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            int (*compare)(const type *a, const type *b),                      \
            void (*print_element)(type * el));

#define ac_sort_test_compare_arg_def(name, type)                               \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            int (*compare)(const type *a, const type *b, void *arg),           \
            void (*print_element)(type * el, void *arg), void *arg);

#define ac_sort_test_m(name, compare, type)                                    \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el)) {                                \
    type *ep = base + num_elements;                                            \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      n = compare((const type *)base - 1, (const type *)base);                 \
      if (n > 0) {                                                             \
        if (func_line)                                                         \
          printf("%s ", func_line);                                            \
        print_element(base - 1);                                               \
        printf(" is greater than ");                                           \
        print_element(base);                                                   \
        printf("\n");                                                          \
        abort();                                                               \
      }                                                                        \
      base++;                                                                  \
    }                                                                          \
  }

#define ac_sort_test_arg_m(name, compare, type)                                \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            void (*print_element)(type * el, void *arg), void *arg) {          \
    type *ep = base + num_elements;                                            \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      n = compare((const type *)base - 1, (const type *)base, arg);            \
      if (n > 0) {                                                             \
        if (func_line)                                                         \
          printf("%s ", func_line);                                            \
        print_element(base - 1, arg);                                          \
        printf(" is greater than ");                                           \
        print_element(base, arg);                                              \
        printf("\n");                                                          \
        abort();                                                               \
      }                                                                        \
      base++;                                                                  \
    }                                                                          \
  }

#define ac_sort_test_compare_m(name, type)                                     \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            int (*compare)(const type *a, const type *b),                      \
            void (*print_element)(type * el)) {                                \
    type *ep = base + num_elements;                                            \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      n = compare((const type *)base - 1, (const type *)base);                 \
      if (n > 0) {                                                             \
        if (func_line)                                                         \
          printf("%s ", func_line);                                            \
        print_element(base - 1);                                               \
        printf(" is greater than ");                                           \
        print_element(base);                                                   \
        printf("\n");                                                          \
        abort();                                                               \
      }                                                                        \
      base++;                                                                  \
    }                                                                          \
  }

#define ac_sort_test_compare_arg_m(name, type)                                 \
  void name(char *func_line, type *base, ssize_t num_elements,                 \
            int (*compare)(const type *a, const type *b, void *arg),           \
            void (*print_element)(type * el, void *arg), void *arg) {          \
    type *ep = base + num_elements;                                            \
    base++;                                                                    \
    int n;                                                                     \
    while (base < ep) {                                                        \
      n = compare((const type *)base - 1, (const type *)base, arg);            \
      if (n > 0) {                                                             \
        if (func_line)                                                         \
          printf("%s ", func_line);                                            \
        print_element(base - 1, arg);                                          \
        printf(" is greater than ");                                           \
        print_element(base, arg);                                              \
        printf("\n");                                                          \
        abort();                                                               \
      }                                                                        \
      base++;                                                                  \
    }                                                                          \
  }
