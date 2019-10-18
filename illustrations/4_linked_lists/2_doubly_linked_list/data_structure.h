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

#ifndef _data_structure_H
#define _data_structure_H

#include "accommon.h"

struct node_s;
typedef struct node_s node_t;

/*
  You may or may not want to have methods like node_init, node_key, node_print,
  and node_destroy.  My purpose in having them is to provide a uniform interface
  for test_data_structure.  This is only an interface for testing.
*/
node_t *node_init(char key);
void node_destroy(node_t *n);
char node_key(node_t *n);
void node_print(node_t *root);

/*
  Given a data structure, find a node given a pointer to the root node and the
	key of interest.
*/
node_t *node_find(char key, node_t *root);

/*
	If the key doesn't exist, an insert a node into the data structure.  The
	node_to_insert has already been initialized by the client application (most
	likely through node_init).  The node's physical location in memory will not
	change as a result of inserting or erasing nodes.  Because the root might
	change, a pointer to the root is passed to node_insert.
*/
bool node_insert(node_t *node_to_insert, node_t **root);

/*
	Once finding a node, erase the node from the data structure.  The
  node_to_erase is typically found through node_find.  The pointer to root is
	passed to node_erase as the root might change.  node_erase doesn't free
	resources used by node_to_erase.  It only unlinks it from the data structure.
*/
bool node_erase(node_t *node_to_erase, node_t **root);

/* Iteration
   All of the linked structures will support first and next.
   Some can support last and previous as well.
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

	 Notice that a temporary link to the next node to erase is saved prior to destroying the node.  For binary search trees, the beginning to the end isn't the same as a sorted order.  It's known as postorder iteration.  Post order iteration can be useful for fast construction and destruction of binary search trees.  By having an alternate iterator for destruction, we can have one interface which works for a number of data structure types.
*/
node_t *node_first_to_erase(node_t *root);
node_t *node_next_to_erase(node_t *n);

#endif
