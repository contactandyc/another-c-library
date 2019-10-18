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
#ifndef _acmap_H
#define _acmap_H

/* Still a bit incomplete */

#include "acbuffer.h"
#include "accommon.h"
#include "acpool.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct acmap_s {
  size_t parent_color;
  struct acmap_s *left;
  struct acmap_s *right;
} acmap_t __attribute__((aligned(2)));
/* The alignment is needed because the color uses the lowest bit */

/* iteration */
acmap_t *acmap_first(acmap_t *n);
acmap_t *acmap_last(acmap_t *n);
acmap_t *acmap_next(acmap_t *n);
acmap_t *acmap_previous(acmap_t *n);
acmap_t *acmap_postorder_first(acmap_t *n);
acmap_t *acmap_postorder_next(acmap_t *n);

/*
  acmap_copy_node_f is a callback meant to be used with acmap_copy to
  copy one map to another given a root node.  The tag will typically be an
  allocator such as the pool.
*/
typedef acmap_t *(*acmap_copy_node_f)(acmap_t *n, void *tag);

acmap_t *acmap_copy(acmap_t *root, acmap_copy_node_f copy,
                          void *tag);

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char *(*print_node_to_string_f)(acpool_t *pool, acmap_t *n);

bool acmap_valid_to_buffer(acbuffer_t *bh, acpool_t *pool,
                              acmap_t *root,
                              print_node_to_string_f print_node);

bool acmap_valid(acpool_t *pool, acmap_t *root,
                    print_node_to_string_f print_node);

#define ACMAP_DONT_PRINT_RED 1
#define ACMAP_DONT_PRINT_BLACK_HEIGHT 2

void acmap_print_to_buffer(acbuffer_t *bh, acpool_t *pool,
                              acmap_t *node,
                              print_node_to_string_f print_node, int flags);

void acmap_print(acpool_t *pool, acmap_t *node,
                    print_node_to_string_f print_node, int flags);

bool acmap_erase(acmap_t *node, acmap_t **root);

void acmap_fix_insert(acmap_t *node, acmap_t *parent,
                         acmap_t **root);

/*
  Finding and insertion cannot be made easily generic due to the need to access
  the key and value members of the structure.  Finding is a pretty trivial
  operation in binary search trees.  I've implemented a number of macros to
  create functions since most use cases will be the same.

  The macros have the following parameters (not all are used in all functions).

  name     - the name of the function you wish to create
  keytype  - the type of the key (only used in find functions)
  datatype - the type of the structure which utilizes the acmap_t
     structure.
  mapname  - the name of the field in your structure which references the
     acmap_t structure.  This is only used in macros with a 2 in the
     name such as acmap_find2_m.
  compare  - the find function will expect the signature to look like
     int compare(keytype key, datatype *d);
       or
     int compare(keytype key, datatype *d, void *arg);
       if there is _arg_ in the macro name such as acmap_find_arg_m.

     The insert function will expect the signature of the compare function to be
     int compare(datatype *d1, datatype *d2);
       or
     int compare(datatype *d1, datatype *d2);
       if there is _arg_ in the macro name such as acmap_insert_arg_m.

  The macros have the following naming convention.
     acmap_find prefix for find functions
     acmap_least prefix to find the least instance of a key
     acmap_greatest prefix to find the greatest instance of a key
     acmap_lower_bound to find the least instance of a key or the
       next item if it doesn't exist
     acmap_upper_bound to find the instance just after a key
     acmap_range to find a range from a start to an end.  The returned value
       is the lower bound and the upper bound (through the first parameter).
       The start and the end keys may be the same.

     acmap_insert prefix for all map insert functions where items must be
       unique
     acmultimap_insert prefix for all map insert functions where items can
       be repeated

     _m suffix for all macros to create functions.
     _arg_m suffix for all macros where the compare function has an arg.
     _compare_m suffix for all macros which expect user to provide compare
       function to resulting function.
     _compare_arg_m suffix for all macros which expect user to provide compare
       function with an extra arg field to resulting function.
     2_m, 2_arg_m, 2_compare_m, 2_compare_arg_m suffix for all macros
       which don't have acmap_t structure defined as the first field.

  The find macros are listed below (they are defined in impl/acmap.h)

  acmap_find_m(name, keytype, datatype, compare)
  acmap_least_m(name, keytype, datatype, compare)
  acmap_greatest_m(name, keytype, datatype, compare)
  acmap_lower_bound_m(name, keytype, datatype, compare)
  acmap_upper_bound_m(name, keytype, datatype, compare)

    expects: int compare( keytype *key, datatype *value);
    returns: datatype *name( keytype *key, acmap_t *root);

  acmap_find2_m(name, keytype, datatype, mapname, compare)
  acmap_least2_m(name, keytype, datatype, mapname, compare)
  acmap_greatest2_m(name, keytype, datatype, mapname, compare)
  acmap_lower_bound2_m(name, keytype, datatype, mapname, compare)
  acmap_upper_bound2_m(name, keytype, datatype, mapname, compare)

    expects: int compare( keytype *key, datatype *value);
    returns: datatype *name( keytype *key, acmap_t *root);

  acmap_find_arg_m(name, keytype, datatype, compare)
  acmap_least_arg_m(name, keytype, datatype, compare)
  acmap_greatest_arg_m(name, keytype, datatype, compare)
  acmap_lower_bound_arg_m(name, keytype, datatype, compare)
  acmap_upper_bound_arg_m(name, keytype, datatype, compare)

    expects: int compare( keytype *key, datatype *value, void *arg);
    returns: datatype *name(keytype *key, acmap_t *root, void *arg);

  acmap_find2_arg_m(name, keytype, datatype, mapname, compare)
  acmap_least2_arg_m(name, keytype, datatype, mapname, compare)
  acmap_greatest2_arg_m(name, keytype, datatype, mapname, compare)
  acmap_lower_bound2_arg_m(name, keytype, datatype, mapname, compare)
  acmap_upper_bound2_arg_m(name, keytype, datatype, mapname, compare)

    expects: int compare( keytype *key, datatype *value, void *arg);
    returns: datatype *name(keytype *key, acmap_t *root, void *arg);

  acmap_find_compare_m(name, keytype, datatype)
  acmap_least_compare_m(name, keytype, datatype)
  acmap_greatest_compare_m(name, keytype, datatype)
  acmap_lower_bound_compare_m(name, keytype, datatype)
  acmap_upper_bound_compare_m(name, keytype, datatype)

    returns: datatype *name(keytype *key,
                            acmap_t *root,
                            int compare(keytype *key,
                                        datatype *value));

  acmap_find2_compare_m(name, keytype, datatype, mapname)
  acmap_least2_compare_m(name, keytype, datatype, mapname)
  acmap_greatest2_compare_m(name, keytype, datatype, mapname)
  acmap_lower_bound2_compare_m(name, keytype, datatype, mapname)
  acmap_upper_bound2_compare_m(name, keytype, datatype, mapname)

    returns: datatype *name(keytype *key,
                            acmap_t *root,
                            int compare(keytype *key,
                                        datatype *value));

  acmap_find_compare_arg_m(name, keytype, datatype)
  acmap_least_compare_arg_m(name, keytype, datatype)
  acmap_greatest_compare_arg_m(name, keytype, datatype)
  acmap_lower_bound_compare_arg_m(name, keytype, datatype)
  acmap_upper_bound_compare_arg_m(name, keytype, datatype)

    returns: datatype *name(keytype *key,
                            acmap_t *root,
                            int compare(keytype *key,
                                        datatype *value,
                                        void *arg),
                            void *arg);

  acmap_find2_compare_arg_m(name, keytype, datatype, mapname)
  acmap_least2_compare_arg_m(name, keytype, datatype, mapname)
  acmap_greatest2_compare_arg_m(name, keytype, datatype, mapname)
  acmap_lower_bound2_compare_arg_m(name, keytype, datatype, mapname)
  acmap_upper_bound2_compare_arg_m(name, keytype, datatype, mapname)

    returns: datatype *name(keytype *key,
                            acmap_t *root,
                            int compare(keytype *key,
                                        datatype *value,
                                        void *arg),
                            void *arg);

  The insert macros are listed below (they are defined in impl/acmap.h)

  acmap_insert_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, acmap_t **root);

  acmap_insert2_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, acmap_t **root);

  acmap_insert_arg_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            acmap_t **root,
                            void *arg);

  acmap_insert2_arg_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            acmap_t **root,
                            void *arg);

  acmultimap_insert_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, acmap_t **root);

  acmultimap_insert2_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,  datatype *value);
    returns: datatype *name(datatype *node_to_insert, acmap_t **root);

  acmultimap_insert_arg_m(name, datatype, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            acmap_t **root,
                            void *arg);

  acmultimap_insert2_arg_m(name, datatype, mapname, compare)
    expects: int compare( datatype *node_to_insert,
                          datatype *value,
                         void *arg);
    returns: datatype *name(datatype *node_to_insert,
                            acmap_t **root,
                            void *arg);
*/

#include "impl/acmap.h"

#endif
