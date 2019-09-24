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

/* still a bit incomplete */

#include "stla_common.h"
#include "stla_pool.h"
#include "stla_buffer.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct stla_map_node_s {
  size_t parent_color;
  struct stla_map_node_s *left;
  struct stla_map_node_s *right;
} stla_map_node_t;

/* iteration */
stla_map_node_t * stla_map_first( stla_map_node_t *n );
stla_map_node_t * stla_map_last( stla_map_node_t *n );
stla_map_node_t * stla_map_next( stla_map_node_t *n );
stla_map_node_t * stla_map_previous( stla_map_node_t *n );
stla_map_node_t * stla_map_postorder_first( stla_map_node_t *n );
stla_map_node_t * stla_map_postorder_next( stla_map_node_t *n );

/*
  stla_map_copy_node_f is a callback meant to be used with stla_map_copy to
  copy one map to another given a root node.  The tag will typically be an
  allocator such as the pool.
*/
typedef stla_map_node_t * (*stla_map_copy_node_f)( stla_map_node_t *n, void *tag );

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char * (*print_node_to_string_f)(stla_pool_t *pool, stla_map_node_t *n);


stla_map_node_t * stla_map_copy( stla_map_node_t *root, stla_map_copy_node_f copy, void *tag);

bool stla_map_valid_to_buffer(stla_buffer_t *bh, stla_pool_t *pool,
                              stla_map_node_t *root, print_node_to_string_f print_node );

bool stla_map_valid(stla_pool_t *pool,
                    stla_map_node_t *root,
                    print_node_to_string_f print_node);

#define STLA_MAP_DONT_PRINT_RED 1
#define STLA_MAP_DONT_PRINT_BLACK_HEIGHT 2

void stla_map_print_to_buffer(stla_buffer_t *bh,
                              stla_pool_t *pool, stla_map_node_t *node,
                              print_node_to_string_f print_node,
                              int flags );

void stla_map_print(stla_pool_t *pool, stla_map_node_t *node,
                    print_node_to_string_f print_node,
                    int flags );

void stla_map_fix_insert(stla_map_node_t *node,
                         stla_map_node_t *parent,
                         stla_map_node_t **root);

bool stla_map_erase(stla_map_node_t *node, stla_map_node_t **root);

/*
  The stla_map_find and stla_map_insert methods are the only functions which
  require access to the key/value members of the structure.  Because of this,
  it is efficient to define these as macros.  stla_map_insert must call the
  stla_map_fix_insert call to balance the tree after the node is inserted as a
  leaf.
*/

#define stla_map_find_m(name, keytype, datatype, compare) \
  datatype *name(keytype p, stla_map_node_t *root) { \
    while (root) {                                   \
      int n=compare(p, (datatype *)root);            \
      if(n < 0)                                      \
        root = root->left;                           \
      else if(n > 0)                                 \
        root = root->right;                          \
      else                                           \
        return (datatype *)root;                     \
    }                                                \
    return NULL;                                     \
  }

#define stla_map_insert_m(name, datatype, compare)    \
  bool name(datatype *node, stla_map_node_t **root) { \
    stla_map_node_t **np = root, *parent = NULL;      \
    while (*np) {                                     \
      parent = *np;                                   \
      int n=compare(node, (datatype *)parent);        \
      if(n < 0)                                       \
        np = &(parent->left);                         \
      else if(n > 0)                                  \
        np = &(parent->right);                        \
      else                                            \
        return false;                                 \
    }                                                 \
    *np = (stla_map_node_t *)node;                    \
    stla_map_fix_insert(*np, parent, root);           \
    return true;                                      \
  }

#endif
