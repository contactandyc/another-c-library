# The Map Object
- Turning the red black tree into a map

*This and the src/acmap.h/c files were started on Monday 9/23/19 and are still a work in progress.*

The code for this section is found in <i>illustrations/13_map/1_map</i>
```bash
cd $ac/illustrations/13_map/1_map
```

```bash
$ make
gcc -O3 -I../../../src -D_ACDEBUG_MEMORY_=NULL ../../../src/actimer.c ../../../src/acallocator.c ../../../src/acbuffer.c ../../../src/acpool.c ../../../src/acmap.c test_data_structure.c -o test_data_structure
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
#ifndef _acmap_H
#define _acmap_H

#include "accommon.h"
#include "acpool.h"
#include "acbuffer.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct acmap_node_s {
  size_t parent_color;
  struct acmap_node_s *left;
  struct acmap_node_s *right;
} acmap_node_t;

/* iteration */
acmap_node_t * acmap_first( acmap_node_t *n );
acmap_node_t * acmap_last( acmap_node_t *n );
acmap_node_t * acmap_next( acmap_node_t *n );
acmap_node_t * acmap_previous( acmap_node_t *n );
acmap_node_t * acmap_postorder_first( acmap_node_t *n );
acmap_node_t * acmap_postorder_next( acmap_node_t *n );

/*
  acmap_copy_node_f is a callback meant to be used with acmap_copy to
  copy one map to another given a root node.  The tag will typically be an
  allocator such as the pool.
*/
typedef acmap_node_t * (*acmap_copy_node_f)( acmap_node_t *n, void *tag );

acmap_node_t * acmap_copy( acmap_node_t *root, acmap_copy_node_f copy, void *tag);

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char * (*print_node_to_string_f)(acpool_t *pool, acmap_node_t *n);

bool acmap_valid_to_buffer(acbuffer_t *bh, acpool_t *pool,
                              acmap_node_t *root, print_node_to_string_f print_node );

bool acmap_valid(acpool_t *pool,
                    acmap_node_t *root,
                    print_node_to_string_f print_node);

#define ACMAP_DONT_PRINT_RED 1
#define ACMAP_DONT_PRINT_BLACK_HEIGHT 2

void acmap_print_to_buffer(acbuffer_t *bh,
                              acpool_t *pool, acmap_node_t *node,
                              print_node_to_string_f print_node,
                              int flags );

void acmap_print(acpool_t *pool, acmap_node_t *node,
                    print_node_to_string_f print_node,
                    int flags );

bool acmap_erase(acmap_node_t *node, acmap_node_t **root);

void acmap_fix_insert(acmap_node_t *node,
                         acmap_node_t *parent,
                         acmap_node_t **root);

/* find and insert macros */

#endif
```

The code in acmap.c is largely not new.  The functions have been prefixed with acmap_.  The node_t structure has been renamed to acmap_node_t.  The internal knowledge of the value type (char key) has been eliminated from the code.  Finally, the acmap_node_t structure is exposed in the header file.

```c
typedef struct acmap_node_s {
  size_t parent_color;
  struct acmap_node_s *left;
  struct acmap_node_s *right;
} acmap_node_t;
```

is similar to the node_t structure before, except it doesn't have *char key*.

To use the acmap, this structure will be added to the custom structure.  test_data_structure.c defines the node_t structure as it was defined before (with a char key).  Most of the time, you will be able to make the *acmap_node_t map* be the first member of your structure.  This makes it easy to cast between your structure and the acmap_node_t structure since they both have the same memory address.  Having the map not be the first member will be explored later (including having multiple map members in a single structure).
```c
typedef struct {
  acmap_node_t map;
  char key;
} node_t;
```

The iteration methods still exist (first, last, next, previous).  They have just been renamed.
```c
acmap_node_t * acmap_first( acmap_node_t *n );
acmap_node_t * acmap_last( acmap_node_t *n );
acmap_node_t * acmap_next( acmap_node_t *n );
acmap_node_t * acmap_previous( acmap_node_t *n );
```

If your structure is called node_t, you can call these functions as
```c
void print_using_iteration(acmap_node_t *root) {
  node_t *n = (node_t *)acmap_first(root);
  while(n) {
    printf( "%c", n->key );
    n = (node_t *)acmap_next((node_t *)n);
  }
  printf( "\n" );
}
```

or

```c
void print_using_iteration(acmap_node_t *root) {
  acmap_node_t *n = acmap_first(root);
  while(n) {
    printf( "%c", ((node_t *)n)->key );
    n = acmap_next(n);
  }
  printf( "\n" );
}
```

Both approaches are equivalent, assuming that the map is the first member of the node_t structure.

The node_first_to_erase and node_next_to_erase have been renamed.
```c
acmap_node_t * acmap_postorder_first( acmap_node_t *n );
acmap_node_t * acmap_postorder_next( acmap_node_t *n );
```

acmap_copy_node_f is a callback meant to be used with acmap_copy to copy one map to another given a root node.  The tag will typically be an allocator, such as the pool.  acmap_copy will make a complete copy of a map (or red-black tree) and return a pointer to it.  I like to suffix function typedefs with a *_f*.
```c
typedef acmap_node_t * (*acmap_copy_node_f)( acmap_node_t *n, void *tag );

acmap_node_t * acmap_copy( acmap_node_t *root, acmap_copy_node_f copy, void *tag);
```

acmap_copy could be used in the following way...
```c
acmap_node_t * copy_node( acmap_node_t *n, void *tag ) {
  acpool_t *pool = (acpool_t *)tag;
  node_t *res = (node_t *)acpool_alloc(pool, sizeof(node_t));
  res->key = n->key;
  return res;
}

acmap_node_t *root = /* a valid map with zero or more entries */;
acmap_node_t *copy_of_root = acmap_copy(root, copy_node, pool);
```

print_node_to_string_f is a callback meant to print the value of the node n.  There is an expectation that the value will be printed on a single line.  Printing the internal representation of the tree is used for testing and printing the tree and doesn't need to be complete.  It generally doesn't have practical use within applications.

```c
typedef char * (*print_node_to_string_f)(acpool_t *pool, acmap_node_t *n);
```

For debugging, it is often nice to be able just to print something to the terminal.  Sometimes, it is also nice to print to a buffer and then have the output directed to some other location.  I provide both mechanisms below.  The first prints errors in the map (red-black violations) to a buffer and returns false if there are any errors.  The second function is like the first, except it outputs directly to the screen.  Both of these functions require the root of the tree and a pointer to a print_node_to_string_f function.
```c
bool acmap_valid_to_buffer(acbuffer_t *bh, acpool_t *pool,
                              acmap_node_t *root, print_node_to_string_f print_node );

bool acmap_valid(acpool_t *pool,
                    acmap_node_t *root,
                    print_node_to_string_f print_node);
```

acmap_valid checks if the map is valid.  acmap_print dumps the contents of a map in a tree-like manner.  It takes additional parameter flags, which would typically be zero unless you with to not print red nodes in the color red and/or you don't want the black height of a node suffixed in the print.  The flags are bit-oriented and can be or'ed together.
```c
#define ACMAP_DONT_PRINT_RED 1
#define ACMAP_DONT_PRINT_BLACK_HEIGHT 2

void acmap_print_to_buffer(acbuffer_t *bh,
                              acpool_t *pool, acmap_node_t *node,
                              print_node_to_string_f print_node,
                              int flags );

void acmap_print(acpool_t *pool, acmap_node_t *node,
                    print_node_to_string_f print_node,
                    int flags );
```

acmap_erase unlinks node from the given map.  The node is expected to be a valid node within the tree (typically found via a find method).  acmap_erase does not destroy the node (it simply unlinks it from the map).
```c
bool acmap_erase(acmap_node_t *node, acmap_node_t **root);
```

The find and insert methods were left to the end for a reason.  These are the only two groups of functions that need to understand the value of a node.  These functions end up being custom.  To aid in writing these functions, I've created the following macros.

```c
acmap_find_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find_least_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_least2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_least_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find_least2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find_least_or_next_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_least_or_next2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_least_or_next_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find_least_or_next2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find_greatest_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_greatest2_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value);
  returns: datatype *name(const keytype *key, const acmap_node_t *root);

acmap_find_greatest_arg_m(name, keytype, datatype, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

acmap_find_greatest2_arg_m(name, keytype, datatype, mapname, compare)
  expects: int compare(const keytype *key, const datatype *value, void *arg);
  returns: datatype *name(const keytype *key,
                          const acmap_node_t *root,
                          void *arg);

The insert macros are listed below (they are defined in impl/acmap.h)

acmap_insert_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, acmap_node_t **root);

acmap_insert2_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, acmap_node_t **root);

acmap_insert_arg_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          acmap_node_t **root,
                          void *arg);

acmap_insert2_arg_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          acmap_node_t **root,
                          void *arg);

acmultimap_insert_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, acmap_node_t **root);

acmultimap_insert2_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert, const datatype *value);
  returns: datatype *name(datatype *node_to_insert, acmap_node_t **root);

acmultimap_insert_arg_m(name, datatype, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          acmap_node_t **root,
                          void *arg);

acmultimap_insert2_arg_m(name, datatype, mapname, compare)
  expects: int compare(const datatype *node_to_insert,
                       const datatype *value,
                       void *arg);
  returns: datatype *name(datatype *node_to_insert,
                          acmap_node_t **root,
                          void *arg);
```
