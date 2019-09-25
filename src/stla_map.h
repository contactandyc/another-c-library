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
#ifndef _stla_map_H
#define _stla_map_H

/* Still a bit incomplete */

#include "stla_buffer.h"
#include "stla_common.h"
#include "stla_pool.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct stla_map_node_s {
  size_t parent_color;
  struct stla_map_node_s *left;
  struct stla_map_node_s *right;
} stla_map_node_t __attribute__((aligned(2)));
/* The alignment is needed because the color uses the lowest bit */

/* iteration */
stla_map_node_t *stla_map_first(stla_map_node_t *n);
stla_map_node_t *stla_map_last(stla_map_node_t *n);
stla_map_node_t *stla_map_next(stla_map_node_t *n);
stla_map_node_t *stla_map_previous(stla_map_node_t *n);
stla_map_node_t *stla_map_postorder_first(stla_map_node_t *n);
stla_map_node_t *stla_map_postorder_next(stla_map_node_t *n);

/*
  stla_map_copy_node_f is a callback meant to be used with stla_map_copy to
  copy one map to another given a root node.  The tag will typically be an
  allocator such as the pool.
*/
typedef stla_map_node_t *(*stla_map_copy_node_f)(stla_map_node_t *n, void *tag);

stla_map_node_t *stla_map_copy(stla_map_node_t *root, stla_map_copy_node_f copy,
                               void *tag);

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char *(*print_node_to_string_f)(stla_pool_t *pool, stla_map_node_t *n);

bool stla_map_valid_to_buffer(stla_buffer_t *bh, stla_pool_t *pool,
                              stla_map_node_t *root,
                              print_node_to_string_f print_node);

bool stla_map_valid(stla_pool_t *pool, stla_map_node_t *root,
                    print_node_to_string_f print_node);

#define STLA_MAP_DONT_PRINT_RED 1
#define STLA_MAP_DONT_PRINT_BLACK_HEIGHT 2

void stla_map_print_to_buffer(stla_buffer_t *bh, stla_pool_t *pool,
                              stla_map_node_t *node,
                              print_node_to_string_f print_node, int flags);

void stla_map_print(stla_pool_t *pool, stla_map_node_t *node,
                    print_node_to_string_f print_node, int flags);

bool stla_map_erase(stla_map_node_t *node, stla_map_node_t **root);

void stla_map_fix_insert(stla_map_node_t *node, stla_map_node_t *parent,
                         stla_map_node_t **root);

/*
  Finding and insertion cannot be made easily generic due to the need to access
  the key and value members of the structure.  Finding is a pretty trivial
  operation in binary search trees.  I've implemented a number of macros to
  create functions since most use cases will be the same.

  The macros have the following parameters (not all are used in all functions).

  name     - the name of the function you wish to create
  keytype  - the type of the key (only used in find functions)
  datatype - the type of the structure which utilizes the stla_map_node_t
     structure.
  mapname  - the name of the field in your structure which references the
     stla_map_node_t structure.  This is only used in macros with a 2 in the
     name such as stla_map_find2_m.
  compare  - the find function will expect the signature to look like
     int compare(keytype key, datatype *d);
       or
     int compare(keytype key, datatype *d, void *arg);
       if there is _arg_ in the macro name such as stla_map_find_arg_m.

     The insert function will expect the signature of the compare function to be
     int compare(datatype *d1, datatype *d2);
       or
     int compare(datatype *d1, datatype *d2);
       if there is _arg_ in the macro name such as stla_map_insert_arg_m.

  The macros have the following naming convention.
     stla_map_find prefix for find functions
     stla_map_least prefix to find the least instance of a key
     stla_map_greatest prefix to find the greatest instance of a key
     stla_map_lower_bound to find the least instance of a key or the
       next item if it doesn't exist
     stla_map_upper_bound to find the instance just after a key
     stla_map_range to find a range from a start to an end.  The returned value
       is the lower bound and the upper bound (through the first parameter).
       The start and the end keys may be the same.

     stla_map_insert prefix for all map insert functions where items must be
       unique
     stla_multimap_insert prefix for all map insert functions where items can
       be repeated

     _m suffix for all macros to create functions.
     _arg_m suffix for all macros where the compare function has an arg.
     _compare_m suffix for all macros which expect user to provide compare
       function to resulting function.
     _compare_arg_m suffix for all macros which expect user to provide compare
       function with an extra arg field to resulting function.
     2_m, 2_arg_m, 2_compare_m, 2_compare_arg_m suffix for all macros
       which don't have stla_map_node_t structure defined as the first field.

  The find macros are listed below (they are defined in impl/stla_map.h)

  stla_map_find_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_find2_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_find_arg_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_find2_arg_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_find_compare_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_find2_compare_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_find_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_find2_compare_arg_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_least_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_least2_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_least_arg_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_least2_arg_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_least_compare_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_least2_compare_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_least_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_least2_compare_arg_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_greatest_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_greatest2_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_greatest_arg_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_greatest2_arg_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_greatest_compare_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_greatest2_compare_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_greatest_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_greatest2_compare_arg_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_lower_bound_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_lower_bound2_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_lower_bound_arg_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_lower_bound2_arg_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_lower_bound_compare_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_lower_bound2_compare_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_lower_bound_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_lower_bound2_compare_arg_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_upper_bound_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_upper_bound2_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(const keytype *key, const stla_map_node_t *root);

  stla_map_upper_bound_arg_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_upper_bound2_arg_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(const keytype *key,
                            const stla_map_node_t *root,
                            void *arg);

  stla_map_upper_bound_compare_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_upper_bound2_compare_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_upper_bound_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_upper_bound2_compare_arg_m(name, keytype, datatype, mapname)
    returns: datatype *name(const keytype *key, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_range_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root);

  stla_map_range2_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value);
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root);

  stla_map_range_arg_m(name, keytype, datatype, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root,
                            void *arg);

  stla_map_range2_arg_m(name, keytype, datatype, mapname, compare)
    expects: int compare(const keytype *key, const datatype *value, void *arg);
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root,
                            void *arg);

  stla_map_range_compare_m(name, keytype, datatype)
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_range2_compare_m(name, keytype, datatype, mapname)
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value));

  stla_map_range_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  stla_map_range2_compare_arg_m(name, keytype, datatype)
    returns: datatype *name(datatype **upper_bound, const keytype *start,
                            const keytype *end, const stla_map_node_t *root,
                            int compare(const keytype *key,
                                        const datatype *value,
                                        void *arg),
                            void *arg);

  The insert macros are listed below (they are defined in impl/stla_map.h)

  stla_map_insert_m(name, datatype, compare)
    expects: int compare(const datatype *node_to_insert, const datatype *value);
    returns: datatype *name(datatype *node_to_insert, stla_map_node_t **root);

  stla_map_insert2_m(name, datatype, mapname, compare)
    expects: int compare(const datatype *node_to_insert, const datatype *value);
    returns: datatype *name(datatype *node_to_insert, stla_map_node_t **root);

  stla_map_insert_arg_m(name, datatype, compare)
    expects: int compare(const datatype *node_to_insert,
                         const datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            stla_map_node_t **root,
                            void *arg);

  stla_map_insert2_arg_m(name, datatype, mapname, compare)
    expects: int compare(const datatype *node_to_insert,
                         const datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            stla_map_node_t **root,
                            void *arg);

  stla_multimap_insert_m(name, datatype, compare)
    expects: int compare(const datatype *node_to_insert, const datatype *value);
    returns: datatype *name(datatype *node_to_insert, stla_map_node_t **root);

  stla_multimap_insert2_m(name, datatype, mapname, compare)
    expects: int compare(const datatype *node_to_insert, const datatype *value);
    returns: datatype *name(datatype *node_to_insert, stla_map_node_t **root);

  stla_multimap_insert_arg_m(name, datatype, compare)
    expects: int compare(const datatype *node_to_insert,
                         const datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            stla_map_node_t **root,
                            void *arg);

  stla_multimap_insert2_arg_m(name, datatype, mapname, compare)
    expects: int compare(const datatype *node_to_insert,
                         const datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            stla_map_node_t **root,
                            void *arg);
*/

#include "impl/stla_map.h"

#endif
