---
path: "/13-map"
posttype: "tutorial"
title: "13. The Map Object"
---

- Turning the red black tree into a map

*This and the src/ac_map.h/c files were started on Monday 9/23/19 and are still a work in progress.*

The code for this section is found in <i>illustrations/13_map/1_map</i>
```
cd $ac/illustrations/13_map/1_map
```

```
$ make
gcc -O3 -I../../../src -D_AC_DEBUG_MEMORY_=NULL ../../../src/ac_timer.c ../../../src/ac_allocator.c ../../../src/ac_buffer.c ../../../src/ac_pool.c ../../../src/ac_map.c test_data_structure.c -o test_data_structure
./test_data_structure ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 10000
Creating ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 using map
P-1
|  \
|   X-2
|   |  \
|   |   (f-2)
|   |   |    \
|   |   b-3   n-3
|   |   |  \  |  \
|   |   |   | |   (r-3)
|   |   |   | |   |    \
|   |   |   | |   p-4   v-4
|   |   |   | |   |  \  |  \
|   |   |   | |   o-5 | |   (x-4)
|   |   |   | |       | |   |    \
|   |   |   | (j-3)   | |   w-5   y-5
|   |   |   | |    \  | |            \
|   |   |   | h-4   | | (t-4)         (z-5)
|   |   |   | |  \  | | |    \             
|   |   |   | g-5 | | | s-5   u-5
|   |   |   |     | | |          
|   |   |   d-4   | | q-5
|   |   |   |  \  | |    
|   |   |   c-5 | | l-4
|   |   |       | | |  \
|   |   Z-4     | | k-5 m-5
|   |   |  \    | |        
|   |   Y-5 a-5 | i-5
|   |           |    
|   T-3         e-5
|   |  \           
|   |   V-4
|   |   |  \
|   |   U-5 W-5
|   |          
|   R-4
|   |  \
|   Q-5 S-5
|          
H-2
|  \
|   L-3
|   |  \
|   |   N-4
|   |   |  \
|   |   M-5 O-5
|   |          
|   J-4
|   |  \
|   I-5 K-5
|          
D-3
|  \
|   F-4
|   |  \
|   E-5 G-5
|          
(3-3)
|    \
1-4   7-4
|  \  |  \
0-5 | |   (B-4)
    | |   |    \
    | |   9-5   C-5
    | |   |  \     
    | |   |   (A-5)
    | |   |        
    | |   (8-5)
    | |        
    | (5-4)
    | |    \
    | 4-5   6-5
    |          
    2-5
```

The third parameter to test_data_structure is 10000, which means to build the tree 10,000 times, randomly, inserting and deleting the values in the second argument.

A good first step in designing any object is to understand the requirements.
1. find, insert, update, remove
2. find less than (or equal if equal exists)
3. find greater (or equal if equal exists)
4. iteration from nodes
5. a copy function
6. a print function
7. any data type should be supported
8. multiple data structures should be able to be embedded to the same node
9. once a node is inserted, its memory location should not change

The code for the interface follows
```c
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

#include "ac_common.h"
#include "ac_pool.h"
#include "ac_buffer.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct ac_map_node_s {
  size_t parent_color;
  struct ac_map_node_s *left;
  struct ac_map_node_s *right;
} ac_map_node_t;

/* iteration */
ac_map_node_t * ac_map_first( ac_map_node_t *n );
ac_map_node_t * ac_map_last( ac_map_node_t *n );
ac_map_node_t * ac_map_next( ac_map_node_t *n );
ac_map_node_t * ac_map_previous( ac_map_node_t *n );
ac_map_node_t * ac_map_postorder_first( ac_map_node_t *n );
ac_map_node_t * ac_map_postorder_next( ac_map_node_t *n );

/*
  ac_map_copy_node_f is a callback meant to be used with ac_map_copy to
  copy one map to another given a root node.  The tag will typically be an
  allocator such as the pool.
*/
typedef ac_map_node_t * (*ac_map_copy_node_f)( ac_map_node_t *n, void *tag );

ac_map_node_t * ac_map_copy( ac_map_node_t *root, ac_map_copy_node_f copy, void *tag);

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char * (*print_node_to_string_f)(ac_pool_t *pool, ac_map_node_t *n);

bool ac_map_valid_to_buffer(ac_buffer_t *bh, ac_pool_t *pool,
                              ac_map_node_t *root, print_node_to_string_f print_node );

bool ac_map_valid(ac_pool_t *pool,
                    ac_map_node_t *root,
                    print_node_to_string_f print_node);

#define AC_MAP_DONT_PRINT_RED 1
#define AC_MAP_DONT_PRINT_BLACK_HEIGHT 2

void ac_map_print_to_buffer(ac_buffer_t *bh,
                              ac_pool_t *pool, ac_map_node_t *node,
                              print_node_to_string_f print_node,
                              int flags );

void ac_map_print(ac_pool_t *pool, ac_map_node_t *node,
                    print_node_to_string_f print_node,
                    int flags );

bool ac_map_erase(ac_map_node_t *node, ac_map_node_t **root);

void ac_map_fix_insert(ac_map_node_t *node,
                         ac_map_node_t *parent,
                         ac_map_node_t **root);

/* find and insert macros */

#endif
```

The code in ac_map.c is largely not new.  The functions have been prefixed with ac_map_.  The node_t structure has been renamed to ac_map_node_t.  The internal knowledge of the value type (char key) has been eliminated from the code.  Finally, the ac_map_node_t structure is exposed in the header file.

```c
typedef struct ac_map_node_s {
  size_t parent_color;
  struct ac_map_node_s *left;
  struct ac_map_node_s *right;
} ac_map_node_t;
```

is similar to the node_t structure before, except it doesn't have *char key*.

To use the ac_map, this structure will be added to the custom structure.  test_data_structure.c defines the node_t structure as it was defined before (with a char key).  Most of the time, you will be able to make the *ac_map_node_t map* be the first member of your structure.  This makes it easy to cast between your structure and the ac_map_node_t structure since they both have the same memory address.  Having the map not be the first member will be explored later (including having multiple map members in a single structure).
```c
typedef struct {
  ac_map_node_t map;
  char key;
} node_t;
```

The iteration methods still exist (first, last, next, previous).  They have just been renamed.
```c
ac_map_node_t * ac_map_first( ac_map_node_t *n );
ac_map_node_t * ac_map_last( ac_map_node_t *n );
ac_map_node_t * ac_map_next( ac_map_node_t *n );
ac_map_node_t * ac_map_previous( ac_map_node_t *n );
```

If your structure is called node_t, you can call these functions as
```c
void print_using_iteration(ac_map_node_t *root) {
  node_t *n = (node_t *)ac_map_first(root);
  while(n) {
    printf( "%c", n->key );
    n = (node_t *)ac_map_next((node_t *)n);
  }
  printf( "\n" );
}
```

or

```c
void print_using_iteration(ac_map_node_t *root) {
  ac_map_node_t *n = ac_map_first(root);
  while(n) {
    printf( "%c", ((node_t *)n)->key );
    n = ac_map_next(n);
  }
  printf( "\n" );
}
```

Both approaches are equivalent, assuming that the map is the first member of the node_t structure.

The node_first_to_erase and node_next_to_erase have been renamed.
```c
ac_map_node_t * ac_map_postorder_first( ac_map_node_t *n );
ac_map_node_t * ac_map_postorder_next( ac_map_node_t *n );
```

ac_map_copy_node_f is a callback meant to be used with ac_map_copy to copy one map to another given a root node.  The tag will typically be an allocator, such as the pool.  ac_map_copy will make a complete copy of a map (or red-black tree) and return a pointer to it.  I like to suffix function typedefs with a *_f*.
```c
typedef ac_map_node_t * (*ac_map_copy_node_f)( ac_map_node_t *n, void *tag );

ac_map_node_t * ac_map_copy( ac_map_node_t *root, ac_map_copy_node_f copy, void *tag);
```

ac_map_copy could be used in the following way...
```c
ac_map_node_t * copy_node( ac_map_node_t *n, void *tag ) {
  ac_pool_t *pool = (ac_pool_t *)tag;
  node_t *res = (node_t *)ac_pool_alloc(pool, sizeof(node_t));
  res->key = n->key;
  return res;
}

ac_map_node_t *root = /* a valid map with zero or more entries */;
ac_map_node_t *copy_of_root = ac_map_copy(root, copy_node, pool);
```

print_node_to_string_f is a callback meant to print the value of the node n.  There is an expectation that the value will be printed on a single line.  Printing the internal representation of the tree is used for testing and printing the tree and doesn't need to be complete.  It generally doesn't have practical use within applications.

```c
typedef char * (*print_node_to_string_f)(ac_pool_t *pool, ac_map_node_t *n);
```

For debugging, it is often nice to be able just to print something to the terminal.  Sometimes, it is also nice to print to a buffer and then have the output directed to some other location.  I provide both mechanisms below.  The first prints errors in the map (red-black violations) to a buffer and returns false if there are any errors.  The second function is like the first, except it outputs directly to the screen.  Both of these functions require the root of the tree and a pointer to a print_node_to_string_f function.
```c
bool ac_map_valid_to_buffer(ac_buffer_t *bh, ac_pool_t *pool,
                              ac_map_node_t *root, print_node_to_string_f print_node );

bool ac_map_valid(ac_pool_t *pool,
                    ac_map_node_t *root,
                    print_node_to_string_f print_node);
```

ac_map_valid checks if the map is valid.  ac_map_print dumps the contents of a map in a tree-like manner.  It takes additional parameter flags, which would typically be zero unless you with to not print red nodes in the color red and/or you don't want the black height of a node suffixed in the print.  The flags are bit-oriented and can be or'ed together.
```c
#define AC_MAP_DONT_PRINT_RED 1
#define AC_MAP_DONT_PRINT_BLACK_HEIGHT 2

void ac_map_print_to_buffer(ac_buffer_t *bh,
                              ac_pool_t *pool, ac_map_node_t *node,
                              print_node_to_string_f print_node,
                              int flags );

void ac_map_print(ac_pool_t *pool, ac_map_node_t *node,
                    print_node_to_string_f print_node,
                    int flags );
```

ac_map_erase unlinks node from the given map.  The node is expected to be a valid node within the tree (typically found via a find method).  ac_map_erase does not destroy the node (it simply unlinks it from the map).
```c
bool ac_map_erase(ac_map_node_t *node, ac_map_node_t **root);
```

The find and insert methods were left to the end for a reason.  These are the only two groups of functions that need to understand the value of a node.  These functions end up being custom.  To aid in writing these functions, I've created the following macros.

```c
ac_map_find_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find_least_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_least2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_least_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find_least2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find_least_or_next_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_least_or_next2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_least_or_next_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find_least_or_next2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find_greatest_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_greatest2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const ac_map_node_t *root);

ac_map_find_greatest_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

ac_map_find_greatest2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const ac_map_node_t *root,
                          void *arg);

The insert macros are listed below (they are defined in impl/ac_map.h)

ac_map_insert_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, ac_map_node_t **root);

ac_map_insert2_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, ac_map_node_t **root);

ac_map_insert_arg_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          ac_map_node_t **root,
                          void *arg);

ac_map_insert2_arg_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          ac_map_node_t **root,
                          void *arg);

ac_multimap_insert_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, ac_map_node_t **root);

ac_multimap_insert2_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, ac_map_node_t **root);

ac_multimap_insert_arg_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          ac_map_node_t **root,
                          void *arg);

ac_multimap_insert2_arg_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          ac_map_node_t **root,
                          void *arg);
```

[Table of Contents (only if viewing on Github)](../../../README.md)
