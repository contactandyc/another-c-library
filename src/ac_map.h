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
#ifndef _ac_map_H
#define _ac_map_H

/* Still a bit incomplete */

#include "ac_buffer.h"
#include "ac_common.h"
#include "ac_pool.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ac_map_s {
  size_t parent_color;
  struct ac_map_s *left;
  struct ac_map_s *right;
} ac_map_t __attribute__((aligned(2)));
/* The alignment is needed because the color uses the lowest bit */

/* iteration */
ac_map_t *ac_map_first(ac_map_t *n);
ac_map_t *ac_map_last(ac_map_t *n);
ac_map_t *ac_map_next(ac_map_t *n);
ac_map_t *ac_map_previous(ac_map_t *n);
ac_map_t *ac_map_postorder_first(ac_map_t *n);
ac_map_t *ac_map_postorder_next(ac_map_t *n);

/*
  ac_map_copy_node_f is a callback meant to be used with ac_map_copy to
  copy one map to another given a root node.  The tag will typically be an
  allocator such as the pool.
*/
typedef ac_map_t *(*ac_map_copy_node_f)(ac_map_t *n, void *tag);

ac_map_t *ac_map_copy(ac_map_t *root, ac_map_copy_node_f copy, void *tag);

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char *(*print_node_to_string_f)(ac_pool_t *pool, ac_map_t *n);

bool ac_map_valid_to_buffer(ac_buffer_t *bh, ac_pool_t *pool, ac_map_t *root,
                            print_node_to_string_f print_node);

bool ac_map_valid(ac_pool_t *pool, ac_map_t *root,
                  print_node_to_string_f print_node);

#define AC_MAP_DONT_PRINT_RED 1
#define AC_MAP_DONT_PRINT_BLACK_HEIGHT 2

void ac_map_print_to_buffer(ac_buffer_t *bh, ac_pool_t *pool, ac_map_t *node,
                            print_node_to_string_f print_node, int flags);

void ac_map_print(ac_pool_t *pool, ac_map_t *node,
                  print_node_to_string_f print_node, int flags);

bool ac_map_erase(ac_map_t *node, ac_map_t **root);

void ac_map_fix_insert(ac_map_t *node, ac_map_t *parent, ac_map_t **root);

/*
  Finding and insertion cannot be made easily generic due to the need to access
  the key and value members of the structure.  Finding is a pretty trivial
  operation in binary search trees.  I've implemented a number of macros to
  create functions since most use cases will be the same.

  The macros have the following parameters (not all are used in all functions).

  name     - the name of the function you wish to create
  keytype  - the type of the key (only used in find functions)
  datatype - the type of the structure which utilizes the ac_map_t
     structure.
  mapname  - the name of the field in your structure which references the
     ac_map_t structure.  This is only used in macros with a 2 in the
     name such as ac_map_find2_m.
  compare  - the find function will expect the signature to look like
     int compare(keytype key, datatype *d);
       or
     int compare(keytype key, datatype *d, void *arg);
       if there is _arg_ in the macro name such as ac_map_find_arg_m.

     The insert function will expect the signature of the compare function to be
     int compare(datatype *d1, datatype *d2);
       or
     int compare(datatype *d1, datatype *d2);
       if there is _arg_ in the macro name such as ac_map_insert_arg_m.

  The macros have the following naming convention.
     ac_map_find prefix for find functions
     ac_map_least prefix to find the least instance of a key
     ac_map_greatest prefix to find the greatest instance of a key
     ac_map_lower_bound to find the least instance of a key or the
       next item if it doesn't exist
     ac_map_upper_bound to find the instance just after a key
     ac_map_range to find a range from a start to an end.  The returned value
       is the lower bound and the upper bound (through the first parameter).
       The start and the end keys may be the same.

     ac_map_insert prefix for all map insert functions where items must be
       unique
     ac_multimap_insert prefix for all map insert functions where items can
       be repeated

     _m suffix for all macros to create functions.
     _arg_m suffix for all macros where the compare function has an arg.
     _compare_m suffix for all macros which expect user to provide compare
       function to resulting function.
     _compare_arg_m suffix for all macros which expect user to provide compare
       function with an extra arg field to resulting function.
     2_m, 2_arg_m, 2_compare_m, 2_compare_arg_m suffix for all macros
       which don't have ac_map_t structure defined as the first field.

  The find macros are listed below (they are defined in impl/ac_map.h)

  ac_map_find_m(name, keytype, datatype, compare)
  ac_map_least_m(name, keytype, datatype, compare)
  ac_map_greatest_m(name, keytype, datatype, compare)
  ac_map_lower_bound_m(name, keytype, datatype, compare)
  ac_map_upper_bound_m(name, keytype, datatype, compare)

    expects: int compare( keytype *key, datatype *value);
    returns: datatype *name( keytype *key, ac_map_t *root);

  ac_map_find2_m(name, keytype, datatype, mapname, compare)
  ac_map_least2_m(name, keytype, datatype, mapname, compare)
  ac_map_greatest2_m(name, keytype, datatype, mapname, compare)
  ac_map_lower_bound2_m(name, keytype, datatype, mapname, compare)
  ac_map_upper_bound2_m(name, keytype, datatype, mapname, compare)

    expects: int compare( keytype *key, datatype *value);
    returns: datatype *name( keytype *key, ac_map_t *root);

  ac_map_find_arg_m(name, keytype, datatype, compare)
  ac_map_least_arg_m(name, keytype, datatype, compare)
  ac_map_greatest_arg_m(name, keytype, datatype, compare)
  ac_map_lower_bound_arg_m(name, keytype, datatype, compare)
  ac_map_upper_bound_arg_m(name, keytype, datatype, compare)

    expects: int compare( keytype *key, datatype *value, void *arg);
    returns: datatype *name(keytype *key, ac_map_t *root, void *arg);

  ac_map_find2_arg_m(name, keytype, datatype, mapname, compare)
  ac_map_least2_arg_m(name, keytype, datatype, mapname, compare)
  ac_map_greatest2_arg_m(name, keytype, datatype, mapname, compare)
  ac_map_lower_bound2_arg_m(name, keytype, datatype, mapname, compare)
  ac_map_upper_bound2_arg_m(name, keytype, datatype, mapname, compare)

    expects: int compare( keytype *key, datatype *value, void *arg);
    returns: datatype *name(keytype *key, ac_map_t *root, void *arg);

  ac_map_find_compare_m(name, keytype, datatype)
  ac_map_least_compare_m(name, keytype, datatype)
  ac_map_greatest_compare_m(name, keytype, datatype)
  ac_map_lower_bound_compare_m(name, keytype, datatype)
  ac_map_upper_bound_compare_m(name, keytype, datatype)

    returns: datatype *name(keytype *key,
                            ac_map_t *root,
                            int compare(keytype *key,
                                        datatype *value));

  ac_map_find2_compare_m(name, keytype, datatype, mapname)
  ac_map_least2_compare_m(name, keytype, datatype, mapname)
  ac_map_greatest2_compare_m(name, keytype, datatype, mapname)
  ac_map_lower_bound2_compare_m(name, keytype, datatype, mapname)
  ac_map_upper_bound2_compare_m(name, keytype, datatype, mapname)

    returns: datatype *name(keytype *key,
                            ac_map_t *root,
                            int compare(keytype *key,
                                        datatype *value));

  ac_map_find_compare_arg_m(name, keytype, datatype)
  ac_map_least_compare_arg_m(name, keytype, datatype)
  ac_map_greatest_compare_arg_m(name, keytype, datatype)
  ac_map_lower_bound_compare_arg_m(name, keytype, datatype)
  ac_map_upper_bound_compare_arg_m(name, keytype, datatype)

    returns: datatype *name(keytype *key,
                            ac_map_t *root,
                            int compare(keytype *key,
                                        datatype *value,
                                        void *arg),
                            void *arg);

  ac_map_find2_compare_arg_m(name, keytype, datatype, mapname)
  ac_map_least2_compare_arg_m(name, keytype, datatype, mapname)
  ac_map_greatest2_compare_arg_m(name, keytype, datatype, mapname)
  ac_map_lower_bound2_compare_arg_m(name, keytype, datatype, mapname)
  ac_map_upper_bound2_compare_arg_m(name, keytype, datatype, mapname)

    returns: datatype *name(keytype *key,
                            ac_map_t *root,
                            int compare(keytype *key,
                                        datatype *value,
                                        void *arg),
                            void *arg);

  The insert macros are listed below (they are defined in impl/ac_map.h)

  ac_map_insert_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, ac_map_t **root);

  ac_map_insert2_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, ac_map_t **root);

  ac_map_insert_arg_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            ac_map_t **root,
                            void *arg);

  ac_map_insert2_arg_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            ac_map_t **root,
                            void *arg);

  ac_multimap_insert_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, ac_map_t **root);

  ac_multimap_insert2_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, ac_map_t **root);

  ac_multimap_insert_arg_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            ac_map_t **root,
                            void *arg);

  ac_multimap_insert2_arg_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            ac_map_t **root,
                            void *arg);
*/

#include "impl/ac_map.h"

#ifdef __cplusplus
}
#endif

#endif
