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

#include "stla_map.h"

#include <stdio.h>
#include <stdlib.h>

#define rb_color(n) ((n)->parent_color & 1)
#define rb_is_red(n) (((n)->parent_color & 1) == 0)
#define rb_is_black(n) (((n)->parent_color & 1) == 1)
#define rb_parent(n) (stla_map_node_t *)((n)->parent_color - ((n)->parent_color & 1))

#define rb_set_black(n) (n)->parent_color |= 1
#define rb_set_red(n) (n)->parent_color -= ((n)->parent_color & 1)
#define rb_set_parent(n, parent) (n)->parent_color = ((n)->parent_color & 1) + (size_t)(parent)

#define rb_clear_black(n) (n)->parent_color = 1

/* iteration */
stla_map_node_t * stla_map_first( stla_map_node_t *n ) {
  if (!n)
    return NULL;
  while (n->left)
    n = n->left;
  return n;
}

stla_map_node_t * stla_map_last( stla_map_node_t *n ) {
  if (!n)
    return NULL;
  while (n->right)
    n = n->right;
  return n;
}

stla_map_node_t * stla_map_next( stla_map_node_t *n ) {
  if (n->right) {
    n = n->right;
    while (n->left)
      n = n->left;
    return n;
  }
  stla_map_node_t *parent = rb_parent(n);
  while (parent && parent->right == n) {
    n = parent;
    parent = rb_parent(n);
  }
  return parent;
}

stla_map_node_t * stla_map_previous( stla_map_node_t *n ) {
  if (n->left) {
    n = n->left;
    while (n->right)
      n = n->right;
    return n;
  }
  stla_map_node_t *parent = rb_parent(n);
  while (parent && parent->left == n) {
    n = parent;
    parent = rb_parent(n);
  }
  return parent;
}

static inline stla_map_node_t *left_deepest_node(stla_map_node_t *n) {
  while (true) {
    if (n->left)
      n = n->left;
    else if (n->right)
      n = n->right;
    else
      return n;
  }
}

stla_map_node_t * stla_map_postorder_first( stla_map_node_t *n ) {
  if (!n)
    return NULL;
  return left_deepest_node(n);
}

stla_map_node_t * stla_map_postorder_next( stla_map_node_t *n ) {
  stla_map_node_t *parent = rb_parent(n);
  if (parent && n == parent->left && parent->right)
    return left_deepest_node(parent->right);
  else
    return parent;
}

/* copy */
static void tree_copy( stla_map_node_t *n, stla_map_node_t **res,
                       stla_map_node_t *parent,
                       stla_map_copy_node_f copy, void *tag ) {
  stla_map_node_t *c = copy(n, tag);
  c->parent_color = n->parent_color;
  rb_set_parent(c, parent);
  if(n->left)
    tree_copy(n->left, &c->left, c, copy, tag);
  else
    c->left = NULL;
  if(n->left)
    tree_copy(n->right, &c->right, c, copy, tag);
  else
    c->right = NULL;
}

stla_map_node_t * stla_map_copy( stla_map_node_t *root, stla_map_copy_node_f copy, void *tag) {
  stla_map_node_t *res = NULL;
  if(root)
    tree_copy(root, &res, NULL, copy, tag);
  return res;
}

static int count_black_nodes(stla_map_node_t *n) {
  int black_nodes = 0;
  while(n) {
    if(rb_is_black(n))
      black_nodes++;
    n = rb_parent(n);
  }
  return black_nodes;
}

static void print_node_with_color_to_buffer(stla_buffer_t *bh,
                                            stla_pool_t *pool,
                                            stla_map_node_t *n,
                                            print_node_to_string_f print_node) {
  bool red = rb_is_red(n);
  char *s = print_node(pool, n);
  if(red)
    stla_buffer_appends(bh, "\x1B[31m(");
  stla_buffer_appends(bh, s);
  if(red)
    stla_buffer_appends(bh, "\x1B[0m)");
}

/* test if valid */
bool stla_map_valid(stla_buffer_t *bh, stla_pool_t *pool,
                    stla_map_node_t *root, print_node_to_string_f print_node ) {
  /* an empty tree is valid */
  if(!root)
    return true;
  bool success = true;
  /* the root is black */
  if(rb_is_red(root)) {
    success = false;
    stla_buffer_appendf( bh, "The root is not black!\n" );
  }
  stla_map_node_t *n = stla_map_first(root);
  int black_nodes = 0;
  stla_map_node_t *first_black_leaf = NULL, *parent;
  stla_map_node_t *sn = n;
  while(n) {
    if(!n->left && !n->right) { /* only consider leaf nodes */
      black_nodes = count_black_nodes(n);
      first_black_leaf = n;
      break;
    }
    n = stla_map_next(n);
  }
  n = sn;
  while(n) {
    /* check if one child and that child is red */
    if(!n->left) {
      if(n->right) {
        int bn = count_black_nodes(n);
        if(bn != black_nodes) {
          success = false;
          print_node_with_color_to_buffer(bh, pool, n, print_node);
          stla_buffer_appendf( bh, " has a NULL left child with a different black height than " );
          print_node_with_color_to_buffer(bh, pool, first_black_leaf, print_node);
          stla_buffer_appendf( bh, "\n");
        }
      }
    }
    else if(!n->right) {
      int bn = count_black_nodes(n);
      if(bn != black_nodes) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        stla_buffer_appendf( bh, " has a NULL right child with a different black height than " );
        print_node_with_color_to_buffer(bh, pool, first_black_leaf, print_node);
        stla_buffer_appendf( bh, "\n");
      }
    }

    if(n->left && !n->right && rb_is_black(n->left)) {
      success = false;
      print_node_with_color_to_buffer(bh, pool, n, print_node);
      stla_buffer_appendf( bh, " has one left child and it isn't red\n" );
    }
    if(!n->left && n->right && rb_is_black(n->right)) {
      success = false;
      print_node_with_color_to_buffer(bh, pool, n, print_node);
      stla_buffer_appendf( bh, " has one right child and it isn't red\n" );
    }
    if(rb_is_red(n)) {
      if(n->left && rb_is_red(n->left)) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        stla_buffer_appendf( bh, " has a red left child and is red\n" );
      }
      if(n->right && rb_is_red(n->right)) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        stla_buffer_appendf( bh, " has a red right child and is red\n" );
      }
      parent = rb_parent(n);
      if(parent && rb_is_red(parent)) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        stla_buffer_appendf( bh, " has a red parent and is red\n" );
      }
    }
    if(!n->left && !n->right) { /* only consider leaf nodes */
      int bn = count_black_nodes(n);
      if(black_nodes != bn) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        stla_buffer_appendf( bh, " has a different black height than " );
        print_node_with_color_to_buffer(bh, pool, first_black_leaf, print_node);
        stla_buffer_appendf( bh, "\n");
      }
    }
    n = stla_map_next(n);
  }
  return success;
}


/* print */
typedef struct stla_map_node_print_s {
  size_t position;
  char *printed_key;
  size_t length;
  bool black;
  int depth;
  struct stla_map_node_print_s *parent;
  struct stla_map_node_print_s *left, *right;
} stla_map_node_print_t;

static int get_black_height(stla_map_node_t *n) {
  int depth = 0;
  while (n) {
    if(rb_is_black(n))
      depth++;
    n = rb_parent(n);
  }
  return depth;
}

static char *get_printed_key(stla_pool_t *pool, stla_map_node_t *n,
                             print_node_to_string_f print_node, int flags ) {
  char *s = print_node(pool, n);
  bool red = rb_is_red(n);
  if(red && (flags & STLA_MAP_DONT_PRINT_RED) == 1)
    red = false;

  if((flags & STLA_MAP_DONT_PRINT_BLACK_HEIGHT) == 0)
    return stla_pool_strdupf(pool, "%s%s-%d%s",
                             red ? "(" : "",
                             s, get_black_height(n),
                             red ? ")" : "");
  else
    return stla_pool_strdupf(pool, "%s%s%s",
                             red ? "(" : "",
                             s,
                             red ? ")" : "");
}

static void copy_tree_to_print(stla_pool_t *pool, stla_map_node_t *node,
                               stla_map_node_print_t **res,
                               stla_map_node_print_t *parent,
                               print_node_to_string_f print_node,
                               int flags ) {
  stla_map_node_print_t *copy = (stla_map_node_print_t *)stla_pool_alloc(pool, sizeof(stla_map_node_print_t));
  *res = copy;

  copy->printed_key = get_printed_key(pool, node, print_node, flags);
  copy->length = strlen(copy->printed_key);
  copy->black = rb_is_black(node) ? true : false;
  copy->position = parent ? ((parent->left == copy) ? parent->position : parent->position + parent->length + 1) : 0;
  copy->depth = 1;
  copy->left = NULL;
  copy->right = NULL;
  copy->parent = parent;

  if(node->left)
    copy_tree_to_print(pool, node->left, &copy->left, copy, print_node, flags );
  if(node->right)
    copy_tree_to_print(pool, node->right, &copy->right, copy, print_node, flags );
}

static stla_map_node_print_t *find_left_parent_with_right_child( stla_map_node_print_t * item,
                                                                 int *depth ) {
  while(item->parent && (item->parent->right == item || !item->parent->right)) {
    *depth += item->depth;
    item = item->parent;
  }
  *depth += item->depth;
  return item->parent;
}

static stla_map_node_print_t *find_left_most_at_depth( stla_map_node_print_t * item, int depth ) {
  if(!item)
    return NULL;

  if(depth <= item->depth)
    return item;
  if(item->left) {
    stla_map_node_print_t *r = find_left_most_at_depth(item->left, depth-item->depth);
    if(r)
      return r;
  }
  if(item->right) {
    stla_map_node_print_t *r = find_left_most_at_depth(item->right, depth-item->depth);
    if(r)
      return r;
  }
  return NULL;
}

static stla_map_node_print_t *find_next_peer( stla_map_node_print_t * item, int depth ) {
  while(item) {
    stla_map_node_print_t *p = find_left_parent_with_right_child(item, &depth);
    if(!p)
      return NULL;
    stla_map_node_print_t *np = find_left_most_at_depth( p->right, depth );
    if(np)
      return np;
    item = p;
  }
  return NULL;
}

static int get_node_depth( stla_map_node_print_t *item ) {
  int r=0;
  while(item) {
    r += item->depth;
    item = item->parent;
  }
  return r;
}


void stla_map_print_to_buffer(stla_buffer_t *bh,
                              stla_pool_t *pool, stla_map_node_t *root,
                              print_node_to_string_f print_node,
                              int flags ) {
  if (!root)
    return;

  stla_map_node_print_t *printable = NULL;
  copy_tree_to_print(pool, root, &printable, NULL, print_node, flags );

  stla_map_node_print_t *sn,*n,*n2;
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
        stla_buffer_appendc(bh, ' ');
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        n2 = find_next_peer(n, 0);
        int extra = 0;
        if(n->right)
          extra = 2;

        if(n2 && (n->position+n->length+1+extra > n2->position)) {
          n->depth++;
          stla_buffer_appendc(bh, '|');
          position++;
        }
        else {
          if(!n->black && (flags & STLA_MAP_DONT_PRINT_RED) == 0)
            stla_buffer_appends(bh, "\x1B[31m");
          stla_buffer_appends(bh, n->printed_key);
          if(!n->black && (flags & STLA_MAP_DONT_PRINT_RED) == 0)
            stla_buffer_appends(bh, "\x1B[0m");
          position += n->length;
        }
        n = n2;
      }
      else {
        n = find_next_peer(n, depth-actual_depth);
        stla_buffer_appendc(bh, '|');
        position++;
      }
    }
    stla_buffer_appendc(bh, '\n');
    position = 0;
    n = sn;
    while(n) {
      for( ; position<n->position; position++ )
        stla_buffer_appendc(bh, ' ');
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        if(n->left) {
          stla_buffer_appendc(bh, '|');
          position++;
        }
        for( ; position<n->position+n->length; position++ )
          stla_buffer_appendc(bh, ' ');
        if(n->right) {
          stla_buffer_appendc(bh, '\\');
          position++;
        }
      }
      else {
        stla_buffer_appendc(bh, '|');
        position++;
      }
      n = find_next_peer(n, depth-actual_depth);
    }
    stla_buffer_appendc(bh, '\n');
    depth++;
  }
}

static void rotate_left(stla_map_node_t *A, stla_map_node_t **root) {
  stla_map_node_t *new_root = A->right;

  size_t tmp_pc = A->parent_color;
  A->parent_color = new_root->parent_color;
  new_root->parent_color = tmp_pc;
  stla_map_node_t *parent = rb_parent(new_root);
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
  }
  else
    *root = new_root;

  stla_map_node_t *tmp = new_root->left;
  new_root->left = A;
  rb_set_parent(A, new_root);

  A->right = tmp;
  if(tmp)
    rb_set_parent(tmp, A);
}

static void rotate_right(stla_map_node_t *A, stla_map_node_t **root) {
  stla_map_node_t *new_root = A->left;
  size_t tmp_pc = A->parent_color;
  A->parent_color = new_root->parent_color;
  new_root->parent_color = tmp_pc;
  stla_map_node_t *parent = rb_parent(new_root);
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
  }
  else
    *root = new_root;

  stla_map_node_t *tmp = new_root->right;
  new_root->right = A;
  rb_set_parent(A, new_root);

  A->left = tmp;
  if(tmp)
    rb_set_parent(tmp, A);
}


void stla_map_fix_insert(stla_map_node_t *node,
                         stla_map_node_t *parent,
                         stla_map_node_t **root) {
  rb_set_red(node);
  rb_set_parent(node, parent);
  node->left = node->right = NULL;

  stla_map_node_t *grandparent, *uncle;

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

static void fix_color_for_erase(stla_map_node_t *parent, stla_map_node_t *node, stla_map_node_t **root) {
  stla_map_node_t *sibling;
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

static inline void replace_node_with_child(stla_map_node_t *child, stla_map_node_t *node, stla_map_node_t **root ) {
  stla_map_node_t *parent = rb_parent(node);
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

bool stla_map_erase(stla_map_node_t *node, stla_map_node_t **root) {
  stla_map_node_t *parent = rb_parent(node);
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
    stla_map_node_t *successor = node->right;
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
      stla_map_node_t *right = successor->right;
      stla_map_node_t *parent = rb_parent(successor);
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
