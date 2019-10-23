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

#include "data_structure.h"

#include <stdio.h>
#include <stdlib.h>

struct node_s {
  size_t parent_color;
  struct node_s *left;
  struct node_s *right;
  char key;
};

#define rb_color(n) ((n)->parent_color & 1)
#define rb_is_red(n) (((n)->parent_color & 1) == 0)
#define rb_is_black(n) (((n)->parent_color & 1) == 1)
#define rb_parent(n) (node_t *)((n)->parent_color - ((n)->parent_color & 1))

#define rb_set_black(n) (n)->parent_color |= 1
#define rb_set_red(n) (n)->parent_color -= ((n)->parent_color & 1)
#define rb_set_parent(n, parent) (n)->parent_color = ((n)->parent_color & 1) + (size_t)(parent)

#define rb_clear_black(n) (n)->parent_color = 1


node_t *node_init(ac_pool_t *pool, char key) {
  node_t *n = (node_t *)ac_pool_alloc(pool, sizeof(node_t));
  n->left = n->right = NULL;
  rb_clear_black(n);
  n->key = key;
  return n;
}

static void _tree_copy(ac_pool_t *pool, node_t *node, node_t **res, node_t *parent ) {
  if(node) {
    node_t *copy = (node_t *)ac_pool_alloc(pool, sizeof(node_t));
    *res = copy;
    copy->key = node->key;
    copy->parent_color = node->parent_color;
    rb_set_parent(copy, parent);
    _tree_copy(pool, node->left, &copy->left, copy);
    _tree_copy(pool, node->right, &copy->right, copy);
  }
  else
    *res = NULL;
}

node_t *tree_copy(ac_pool_t *pool, node_t *root) {
  node_t *res = NULL;
  _tree_copy(pool, root, &res, NULL );
  return res;
}

void node_destroy(node_t *n) {
  // ac_free(n);
}

char node_key(node_t *n) { return n->key; }

node_t *node_find(char key, node_t *root) {
  while (root) {
    if (key < root->key)
      root = root->left;
    else if (key > root->key)
      root = root->right;
    else
      return root;
  }
  return NULL;
}

void red_black_insert(node_t *node, node_t **root);

bool node_insert(node_t *node_to_insert, node_t **root) {
  node_t **n = root, *parent = NULL;
  while (*n) {
    parent = *n;
    if (node_to_insert->key < parent->key)
      n = &(parent->left);
    else if (node_to_insert->key > parent->key)
      n = &(parent->right);
    else
      return false;
  }

  rb_set_parent(node_to_insert, parent);
  node_to_insert->left = node_to_insert->right = NULL;
  *n = node_to_insert;
  red_black_insert(node_to_insert, root);
  return true;
}



typedef struct node_print_item_s {
  size_t position;
  char *printed_key;
  size_t length;
  bool black;
  int depth;
  struct node_print_item_s *parent;
  struct node_print_item_s *left, *right;
} node_print_item_t;

static int get_black_height(node_t *n) {
  int depth = 0;
  while (n) {
    if(rb_is_black(n))
      depth++;
    n = rb_parent(n);
  }
  return depth;
}

static char *get_printed_key(ac_pool_t *pool, node_t *n ) {
  return ac_pool_strdupf(pool, "%s%c%d%s", rb_is_black(n) ? "" : "(",
                           n->key, get_black_height(n),
                           rb_is_black(n) ? "" : ")");
}

void print_node_with_color(node_t *n) {
  printf( "%s%c%d%s", rb_is_black(n) ? "" : "\x1B[31m(", n->key, get_black_height(n), rb_is_black(n) ? "" : ")\x1B[0m");
}

static void copy_tree(ac_pool_t *pool, node_t *node,
                      node_print_item_t **res, node_print_item_t *parent ) {
  node_print_item_t *copy = (node_print_item_t *)ac_pool_alloc(pool, sizeof(node_print_item_t));
  *res = copy;

  copy->printed_key = get_printed_key(pool, node);
  copy->length = strlen(copy->printed_key);
  copy->black = rb_is_black(node) ? true : false;
  copy->position = parent ? ((parent->left == copy) ? parent->position : parent->position + parent->length + 1) : 0;
  copy->depth = 1;
  copy->left = NULL;
  copy->right = NULL;
  copy->parent = parent;

  if(node->left)
    copy_tree(pool, node->left, &copy->left, copy );
  if(node->right)
    copy_tree(pool, node->right, &copy->right, copy );
}

static node_print_item_t *find_left_parent_with_right_child( node_print_item_t * item,
                                                             int *depth ) {
  while(item->parent && (item->parent->right == item || !item->parent->right)) {
    *depth += item->depth;
    item = item->parent;
  }
  *depth += item->depth;
  return item->parent;
}

static node_print_item_t *find_left_most_at_depth( node_print_item_t * item, int depth ) {
  if(!item)
    return NULL;

  if(depth <= item->depth)
    return item;
  if(item->left) {
    node_print_item_t *r = find_left_most_at_depth(item->left, depth-item->depth);
    if(r)
      return r;
  }
  if(item->right) {
    node_print_item_t *r = find_left_most_at_depth(item->right, depth-item->depth);
    if(r)
      return r;
  }
  return NULL;
}

static node_print_item_t *find_next_peer( node_print_item_t * item, int depth ) {
  while(item) {
    node_print_item_t *p = find_left_parent_with_right_child(item, &depth);
    if(!p)
      return NULL;
    node_print_item_t *np = find_left_most_at_depth( p->right, depth );
    if(np)
      return np;
    item = p;
  }
  return NULL;
}

static int get_node_depth( node_print_item_t *item ) {
  int r=0;
  while(item) {
    r += item->depth;
    item = item->parent;
  }
  return r;
}


void node_print(ac_pool_t *pool, node_t *root) {
  if (!root)
    return;

  node_print_item_t *printable = NULL;
  copy_tree(pool, root, &printable, NULL );

  node_print_item_t *sn,*n,*n2;
  int actual_depth;
  int depth=1;
  while(true) {
    sn = find_left_most_at_depth(printable, depth);
    if(!sn)
      break;
    n = sn;
    int position = 0;
    while(n) {
      for( ; position<n->position; position++ )
        printf( " ");
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        n2 = find_next_peer(n, 0);
        int extra = 0;
        if(n->right)
          extra = 2;

        if(n2 && (n->position+n->length+1+extra > n2->position)) {
          n->depth++;
          printf( "|");
          position++;
        }
        else {
          printf( "%s%s%s", n->black ? "" : "\x1B[31m", n->printed_key, n->black ? "" : "\x1B[0m");
          position += n->length;
        }
        n = n2;
      }
      else {
        n = find_next_peer(n, depth-actual_depth);
        printf( "|");
        position++;
      }
    }
    printf( "\n");
    position = 0;
    n = sn;
    while(n) {
      for( ; position<n->position; position++ )
        printf( " ");
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        if(n->left) {
          printf( "|" );
          position++;
        }
        for( ; position<n->position+n->length; position++ )
          printf( " ");
        if(n->right) {
          printf( "\\" );
          position++;
        }
      }
      else {
        printf( "|");
        position++;
      }
      n = find_next_peer(n, depth-actual_depth);
    }
    printf( "\n");
    depth++;
  }
}

static inline void replace_node_with_child(node_t *child, node_t *node, node_t **root ) {
  node_t *parent = rb_parent(node);
  if(parent) {
    if(parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  }
  else
    *root = child;

  child->parent_color = node->parent_color;
}

static void fix_color_for_erase(node_t *parent, node_t *node, node_t **root);

bool node_erase(node_t *node, node_t **root) {
  node_t *parent = rb_parent(node);
  if(!node->left) {
    if(node->right)
      replace_node_with_child(node->right, node, root);
    else {
      if(parent) {
        if(parent->left == node)
          parent->left = NULL;
        else
          parent->right = NULL;
        if(rb_is_black(node))
          fix_color_for_erase(parent, NULL, root);
      }
      else
        *root = NULL;
    }
  }
  else if(!node->right)
    replace_node_with_child(node->left, node, root );
  else {
    node_t *successor = node->right;
    if(!successor->left) {
      bool black = rb_is_black(successor);
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      rb_set_parent(successor->left, successor);
      if(successor->right)
        rb_set_black(successor->right);
      else {
        if(black)
          fix_color_for_erase(successor, NULL, root);
      }
    }
    else {
      while(successor->left)
        successor = successor->left;

      bool black = rb_is_black(successor);
      node_t *right = successor->right;
      node_t *parent = rb_parent(successor);
      parent->left = right;
      if(right) {
        rb_clear_black(right);
        rb_set_parent(right, parent);
        black = false;
      }
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      rb_set_parent(successor->left, successor);
      successor->right = node->right;
      rb_set_parent(successor->right, successor);
      if(black)
        fix_color_for_erase(parent, NULL, root);
    }
  }
  return true;
}

node_t *node_first(node_t *n) {
  if (!n)
    return NULL;
  while (n->left)
    n = n->left;
  return n;
}

node_t *node_last(node_t *n) {
  if (!n)
    return NULL;
  while (n->right)
    n = n->right;
  return n;
}

node_t *node_next(node_t *n) {
  if (n->right) {
    n = n->right;
    while (n->left)
      n = n->left;
    return n;
  }
  node_t *parent = rb_parent(n);
  while (parent && parent->right == n) {
    n = parent;
    parent = rb_parent(n);
  }
  return parent;
}

bool node_previous_supported() { return true; }

node_t *node_previous(node_t *n) {
  if (n->left) {
    n = n->left;
    while (n->right)
      n = n->right;
    return n;
  }
  node_t *parent = rb_parent(n);
  while (parent && parent->left == n) {
    n = parent;
    parent = rb_parent(n);
  }
  return parent;
}

static inline node_t *left_deepest_node(node_t *n) {
  while (true) {
    if (n->left)
      n = n->left;
    else if (n->right)
      n = n->right;
    else
      return n;
  }
}

node_t *node_first_to_erase(node_t *n) {
  if (!n)
    return NULL;
  return left_deepest_node(n);
}

node_t *node_next_to_erase(node_t *n) {
  node_t *parent = rb_parent(n);
  if (parent && n == parent->left && parent->right)
    return left_deepest_node(parent->right);
  else
    return parent;
}

void rotate_left(node_t *A, node_t **root) {
  node_t *new_root = A->right;

  size_t tmp_pc = A->parent_color;
  A->parent_color = new_root->parent_color;
  new_root->parent_color = tmp_pc;
  node_t *parent = rb_parent(new_root);
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
  }
  else
    *root = new_root;

  node_t *tmp = new_root->left;
  new_root->left = A;
  rb_set_parent(A, new_root);

  A->right = tmp;
  if(tmp)
    rb_set_parent(tmp, A);
}

void rotate_right(node_t *A, node_t **root) {
  node_t *new_root = A->left;
  size_t tmp_pc = A->parent_color;
  A->parent_color = new_root->parent_color;
  new_root->parent_color = tmp_pc;
  node_t *parent = rb_parent(new_root);
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
  }
  else
    *root = new_root;

  node_t *tmp = new_root->right;
  new_root->right = A;
  rb_set_parent(A, new_root);

  A->left = tmp;
  if(tmp)
    rb_set_parent(tmp, A);
}


void red_black_insert(node_t *node, node_t **root) {
  rb_set_red(node);
  node_t *parent, *grandparent, *uncle;

  while (true) {
    parent = rb_parent(node);
    if(!parent) {
      rb_clear_black(node);
      break;
    }

    if(rb_is_black(parent))
      break;

    grandparent = rb_parent(parent);
    if(grandparent->left == parent) {
      uncle = grandparent->right;
      if(uncle && rb_is_red(uncle)) {
        rb_set_red(grandparent);
        rb_set_black(parent);
        rb_set_black(uncle);
        node = grandparent;
        continue;
      }
      if(parent->right == node)
        rotate_left(parent, NULL);
      rotate_right(grandparent, root);
      break;
    }
    else {
      uncle = grandparent->left;
      if(uncle && rb_is_red(uncle)) {
        rb_set_red(grandparent);
        rb_set_black(parent);
        rb_set_black(uncle);
        node = grandparent;
        continue;
      }
      if(parent->left == node)
        rotate_right(parent, NULL);
      rotate_left(grandparent, root);
      break;
    }
  }
}

static void fix_color_for_erase(node_t *parent, node_t *node, node_t **root) {
  node_t *sibling;
  if(parent->right != node) {
    sibling = parent->right;
    if(rb_is_red(sibling)) {
      rotate_left(parent, root);
      sibling = parent->right;
    }
    if(sibling->right && rb_is_red(sibling->right)) {
      rb_set_black(sibling->right);
      rotate_left(parent, root);
    }
    else if(sibling->left && rb_is_red(sibling->left)) {
      rotate_right(sibling, root);
      rotate_left(parent, root);
      rb_set_black(sibling);
    }
    else {
      rb_set_red(sibling);
      if(rb_parent(parent) && rb_is_black(parent))
        fix_color_for_erase(rb_parent(parent), parent, root);
      else
        rb_set_black(parent);
    }
  }
  else {
    sibling = parent->left;
    if(rb_is_red(sibling)) {
      rotate_right(parent, root);
      sibling = parent->left;
    }
    if(sibling->left && rb_is_red(sibling->left)) {
      rb_set_black(sibling->left);
      rotate_right(parent, root);
    }
    else if(sibling->right && rb_is_red(sibling->right)) {
      rotate_left(sibling, root);
      rotate_right(parent, root);
      rb_set_black(sibling);
    }
    else {
      rb_set_red(sibling);
      if(rb_parent(parent) && rb_is_black(parent))
        fix_color_for_erase(rb_parent(parent), parent, root);
      else
        rb_set_black(parent);
    }
  }
}


int count_black_nodes(node_t *n) {
  int black_nodes = 0;
  while(n) {
    if(rb_is_black(n))
      black_nodes++;
    n = rb_parent(n);
  }
  return black_nodes;
}


bool test_red_black_rules(ac_pool_t *pool, node_t *root) {
  /* an empty tree is valid */
  if(!root)
    return true;
  bool success = true;
  /* the root is black */
  if(rb_is_red(root)) {
    success = false;
    printf( "The root is not black!\n" );
  }
  node_t *n = node_first(root);
  int black_nodes = 0;
  node_t *first_black_leaf = NULL, *parent;
  node_t *sn = n;
  while(n) {
    if(!n->left && !n->right) { /* only consider leaf nodes */
      black_nodes = count_black_nodes(n);
      first_black_leaf = n;
      break;
    }
    n = node_next(n);
  }
  n = sn;
  while(n) {
    /* check if one child and that child is red */
    if(!n->left) {
      if(n->right) {
        int bn = count_black_nodes(n);
        if(bn != black_nodes) {
          success = false;
          print_node_with_color(n);
          printf( " has a NULL left child with a different black height than " );
          print_node_with_color(first_black_leaf);
          printf( "\n");
        }
      }
    }
    else if(!n->right) {
      int bn = count_black_nodes(n);
      if(bn != black_nodes) {
        success = false;
        print_node_with_color(n);
        printf( " has a NULL right child with a different black height than " );
        print_node_with_color(first_black_leaf);
        printf( "\n");
      }
    }

    if(n->left && !n->right && rb_is_black(n->left)) {
      success = false;
      print_node_with_color(n);
      printf( " has one left child and it isn't red\n" );
    }
    if(!n->left && n->right && rb_is_black(n->right)) {
      success = false;
      print_node_with_color(n);
      printf( " has one right child and it isn't red\n" );
    }
    if(rb_is_red(n)) {
      if(n->left && rb_is_red(n->left)) {
        success = false;
        print_node_with_color(n);
        printf( " has a red left child and is red\n" );
      }
      if(n->right && rb_is_red(n->right)) {
        success = false;
        print_node_with_color(n);
        printf( " has a red right child and is red\n" );
      }
      parent = rb_parent(n);
      if(parent && rb_is_red(parent)) {
        success = false;
        print_node_with_color(n);
        printf( " has a red parent and is red\n" );
      }
    }
    if(!n->left && !n->right) { /* only consider leaf nodes */
      int bn = count_black_nodes(n);
      if(black_nodes != bn) {
        success = false;
        print_node_with_color(n);
        printf( " has a different black height than " );
        print_node_with_color(first_black_leaf);
        printf( "\n");
      }
    }
    n = node_next(n);
  }
  return success;
}
