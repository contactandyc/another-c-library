# The Map Object
- Turning the red black tree into a map

*This and the src/stla_map.h/c files were started on Monday 9/23/19 and is still a work in progress.*

The code for this section is found in <i>illustrations/13_map/1_map</i>
```bash
cd $stla/illustrations/13_map/1_map
```

```bash
$ make
gcc -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c ../../../src/stla_map.c test_data_structure.c -o test_data_structure
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

The third parameter to test_data_structure is 10000, which means to build the tree 10,000 times randomly inserting and deleting the values in the second argument.

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
#ifndef _stla_map_H
#define _stla_map_H

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

stla_map_node_t * stla_map_copy( stla_map_node_t *root, stla_map_copy_node_f copy, void *tag);

/*
  print_node_to_string_f is a callback meant to print the value of the node n.
  There is an expectation that the value will be printed on a single line.
  Printing the internal representation of the tree is largely meant for testing
  and doesn't need to be complete.
*/
typedef char * (*print_node_to_string_f)(stla_pool_t *pool, stla_map_node_t *n);

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

bool stla_map_erase(stla_map_node_t *node, stla_map_node_t **root);

void stla_map_fix_insert(stla_map_node_t *node,
                         stla_map_node_t *parent,
                         stla_map_node_t **root);

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
```

The code in stla_map.c is largely not new.  The functions have been prefixed with stla_map_.  The node_t structure has been renamed to stla_map_node_t.  The internal knowledge of the value type (char key) has been eliminated from the code.  Finally, the stla_map_node_t structure is exposed in the header file.

```c
typedef struct stla_map_node_s {
  size_t parent_color;
  struct stla_map_node_s *left;
  struct stla_map_node_s *right;
} stla_map_node_t;
```

is similar to the node_t structure before, except it doesn't have *char key*.

To use the stla_map, this structure will be added to the custom structure.  test_data_structure.c defines the node_t structure as it was defined before (with a char key).  Most of the time, you will be able to make the *stla_map_node_t map* be the first member of your structure.  This makes it easy to cast between your structure and the stla_map_node_t structure since they both have the same memory address.  Having the map not be the first member will be explored later (including having multiple map members in a single structure).
```c
typedef struct {
  stla_map_node_t map;
  char key;
} node_t;
```

The iteration methods still exist (first, last, next, previous).  They have just been renamed.
```c
stla_map_node_t * stla_map_first( stla_map_node_t *n );
stla_map_node_t * stla_map_last( stla_map_node_t *n );
stla_map_node_t * stla_map_next( stla_map_node_t *n );
stla_map_node_t * stla_map_previous( stla_map_node_t *n );
```

If your structure is called node_t, you can call these functions as
```c
void print_using_iteration(stla_map_node_t *root) {
  node_t *n = (node_t *)stla_map_first(root);
  while(n) {
    printf( "%c", n->key );
    n = (node_t *)stla_map_next((node_t *)n);
  }
  printf( "\n" );
}
```

or

```c
void print_using_iteration(stla_map_node_t *root) {
  stla_map_node_t *n = stla_map_first(root);
  while(n) {
    printf( "%c", ((node_t *)n)->key );
    n = stla_map_next(n);
  }
  printf( "\n" );
}
```

Both approaches are equivalent assuming that the map is the first member of the node_t structure.

The node_first_to_erase and node_next_to_erase have been renamed.
```c
stla_map_node_t * stla_map_postorder_first( stla_map_node_t *n );
stla_map_node_t * stla_map_postorder_next( stla_map_node_t *n );
```

stla_map_copy_node_f is a callback meant to be used with stla_map_copy to copy one map to another given a root node.  The tag will typically be an allocator such as the pool.  stla_map_copy will make a complete copy of a map (or red black tree) and return a pointer to it.  I like to suffix function typedefs with a *_f*.
```c
typedef stla_map_node_t * (*stla_map_copy_node_f)( stla_map_node_t *n, void *tag );

stla_map_node_t * stla_map_copy( stla_map_node_t *root, stla_map_copy_node_f copy, void *tag);
```

stla_map_copy could be used in the following way...
```c
stla_map_node_t * copy_node( stla_map_node_t *n, void *tag ) {
  stla_pool_t *pool = (stla_pool_t *)tag;
  node_t *res = (node_t *)stla_pool_alloc(pool, sizeof(node_t));
  res->key = n->key;
  return res;
}

stla_map_node_t *root = /* a valid map with zero or more entries */;
stla_map_node_t *copy_of_root = stla_map_copy(root, copy_node, pool);
```

print_node_to_string_f is a callback meant to print the value of the node n.  There is an expectation that the value will be printed on a single line.  Printing the internal representation of the tree is used for testing and printing the tree and doesn't need to be complete.  It generally doesn't have practical use within applications.

```c
typedef char * (*print_node_to_string_f)(stla_pool_t *pool, stla_map_node_t *n);
```

For debugging, it is often nice to be able to just print something to the terminal.  Sometimes, it is also nice to print to a buffer and then have the output directed to some other location.  I provide both mechanisms below.  The first prints errors in the map (red black violations) to a buffer and returns false if there are any errors.  The second function is like the first, except it outputs directly to the screen.  Both of these functions require the root of the tree and a pointer to a print_node_to_string_f function.
```c
bool stla_map_valid_to_buffer(stla_buffer_t *bh, stla_pool_t *pool,
                              stla_map_node_t *root, print_node_to_string_f print_node );

bool stla_map_valid(stla_pool_t *pool,
                    stla_map_node_t *root,
                    print_node_to_string_f print_node);
```

stla_map_valid checks if the map is valid.  stla_map_print dumps the contents of a map in a tree like manner.  It takes an additional parameter flags which would typically be zero unless you with to not print red nodes in the color red and/or you don't want the black height of a node suffixed in the print.  The flags are bit oriented and can be or'ed together.
```c
#define STLA_MAP_DONT_PRINT_RED 1
#define STLA_MAP_DONT_PRINT_BLACK_HEIGHT 2

void stla_map_print_to_buffer(stla_buffer_t *bh,
                              stla_pool_t *pool, stla_map_node_t *node,
                              print_node_to_string_f print_node,
                              int flags );

void stla_map_print(stla_pool_t *pool, stla_map_node_t *node,
                    print_node_to_string_f print_node,
                    int flags );
```

stla_map_erase unlinks node from the given map.  The node is expected to be a valid node within the tree (typically found via a find method).  stla_map_erase does not destroy the node (it simply unlinks it from the map).
```c
bool stla_map_erase(stla_map_node_t *node, stla_map_node_t **root);
```

The find and insert methods were left to the end for a reason.  These are the only two functions which need to understand the value of a node.  These functions end up being custom.  To aid in writing these functions, I've created the following macros.
