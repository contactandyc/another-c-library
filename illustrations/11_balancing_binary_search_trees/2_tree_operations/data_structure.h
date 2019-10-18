#ifndef _data_structure_H
#define _data_structure_H

#include "acpool.h"
#include <stdbool.h>


#define RED 1
#define BLACK 0

struct node_s;
typedef struct node_s node_t;

struct node_s {
  size_t color;
  struct node_s *parent;
  struct node_s *left;
  struct node_s *right;
  char key;
};

/* You may or may not want to have methods like node_init, node_key, and
  node_destroy.  My purpose in having them is to provide a uniform interface for
  test_data_structure.
*/
node_t *node_init(acpool_t *pool, char key);
void node_destroy(node_t *n);
char node_key(node_t *n);
void node_print(acpool_t *pool, node_t *root);

bool node_red_black_insert(node_t *node_to_insert, node_t **root);
node_t *tree_copy(acpool_t *pool, node_t *root);
void rotate_left(node_t *A, node_t **root);
void rotate_right(node_t *A, node_t **root);
void color_node_red( node_t *node );
void color_node_black( node_t *node );
void recolor( node_t *node );
bool test_red_black_rules(acpool_t *pool, node_t *root);


node_t *node_find(char key, node_t *root);
bool node_insert(node_t *node_to_insert, node_t **root);
bool node_erase(node_t *node_to_erase, node_t **root);

/* Iteration
   All of the linked structures will support first and next.
   Some can support previous as well.
*/
bool node_previous_supported();
node_t *node_first(node_t *root);
node_t *node_last(node_t *root);
node_t *node_next(node_t *n);
node_t *node_previous(node_t *n);

/* To destroy a data structure, you will need to use the following pattern...

   node_t *n = node_first_to_erase(root);
   while(n) {
     node_t *next = node_next_to_erase(n);
     node_destroy(n);
     n = next;
   }

   It is important to get the next item prior to destroying it.  Some data
   structures also require nodes to be destroyed in a certain order.
*/
node_t *node_first_to_erase(node_t *root);
node_t *node_next_to_erase(node_t *n);

#endif
