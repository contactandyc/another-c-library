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

#ifndef _ac_sort_H
#define _ac_sort_H

#ifdef __cplusplus
extern "C" {
#endif

/*
  In general, you will only need to use the sort macros.  The print and test
  macros are meant for testing.
*/

/*
  Sort macros
  =====================================================================

  ac_sort_m(name, compare, datatype)
    expects: int compare(const datatype *a, const datatype *b);
    returns: void name(type *base, size_t num_elements);

  ac_sort_arg_m(name, compare, datatype)
    expects: int compare(const datatype *a, const datatype *b, void *arg);
    returns: void name(type *base, size_t num_elements, void *arg);

  ac_sort_compare_m(name, datatype)
    returns: void name(type *base, size_t num_elements,
                       int compare(const datatype *a, const datatype *b));

  ac_sort_compare_arg_m(name, datatype)
    returns: void name(type *base, size_t num_elements,
                       int compare(const datatype *a,
                                 const datatype *b, void *arg),
                       void *arg);
*/

/*
  Print macros (used to print sorted set for debugging mostly)
  =====================================================================

  ac_sort_print_m(name, datatype)
    returns: void name(const char *func_line, datatype *base,
                       ssize_t num_elements,
                       void (*print_element)(const datatype *el));

  ac_sort_print_arg_m(name, datatype)
    returns: void name(const char *func_line, datatype *base,
                       ssize_t num_elements,
                       void (*print_element)(const datatype *el, void *arg),
                       void *arg);
*/

/*
  Test macros (used to test that a list is in sorted order)
  =====================================================================
  ac_sort_test_m(name, compare, datatype)
    expects: int compare(const datatype *a, const datatype *b);
    returns: void name(const char *func_line,
                       datatype *base, ssize_t num_elements,
                       void (*print_element)(const datatype *el));

  ac_sort_test_arg_m(name, compare, datatype)
    expects: int compare(const datatype *a, const datatype *b, void *arg);
    returns: void name(const char *func_line,
                       datatype *base, ssize_t num_elements,
                       void (*print_element)(const datatype *el, void *arg),
                       void *arg);

  ac_sort_test_compare_m(name, datatype)
    returns: void name(const char *func_line,
                       datatype *base, ssize_t num_elements,
                       bool (*compare)(const datatype *a,
                                    const datatype *b),
                       void (*print_element)(const datatype *el));

  ac_sort_test_compare_arg_m(name, datatype)
    returns: void name(const char *func_line,
                       datatype *base, ssize_t num_elements,
                       bool (*compare)(const datatype *a,
                                    const datatype *b,
                                    void *arg),
                       void (*print_element)(const datatype *el, void *arg),
                       void *arg);
*/

#include "another-c-library/ac-core/ac_sort.h"

#ifdef __cplusplus
}
#endif

#endif
