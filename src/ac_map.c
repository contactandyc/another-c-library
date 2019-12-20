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

#include "ac_map.h"

#include <stdio.h>
#include <stdlib.h>

#define rb_color(n) ((n)->parent_color & 1)
#define rb_is_red(n) (((n)->parent_color & 1) == 0)
#define rb_is_black(n) (((n)->parent_color & 1) == 1)
#define rb_parent(n) (ac_map_t *)((n)->parent_color - ((n)->parent_color & 1))

#define rb_set_black(n) (n)->parent_color |= 1
#define rb_set_red(n) (n)->parent_color -= ((n)->parent_color & 1)
#define rb_set_parent(n, parent)                                               \
  (n)->parent_color = ((n)->parent_color & 1) + (size_t)(parent)

#define rb_clear_black(n) (n)->parent_color = 1

/* iteration */
ac_map_t *ac_map_first(ac_map_t *n) {
  if (!n)
    return NULL;
  while (n->left)
    n = n->left;
  return n;
}

ac_map_t *ac_map_last(ac_map_t *n) {
  if (!n)
    return NULL;
  while (n->right)
    n = n->right;
  return n;
}

ac_map_t *ac_map_next(ac_map_t *n) {
  if (n->right) {
    n = n->right;
    while (n->left)
      n = n->left;
    return n;
  }
  ac_map_t *parent = rb_parent(n);
  while (parent && parent->right == n) {
    n = parent;
    parent = rb_parent(n);
  }
  return parent;
}

ac_map_t *ac_map_previous(ac_map_t *n) {
  if (n->left) {
    n = n->left;
    while (n->right)
      n = n->right;
    return n;
  }
  ac_map_t *parent = rb_parent(n);
  while (parent && parent->left == n) {
    n = parent;
    parent = rb_parent(n);
  }
  return parent;
}

static inline ac_map_t *left_deepest_node(ac_map_t *n) {
  while (true) {
    if (n->left)
      n = n->left;
    else if (n->right)
      n = n->right;
    else
      return n;
  }
}

ac_map_t *ac_map_postorder_first(ac_map_t *n) {
  if (!n)
    return NULL;
  return left_deepest_node(n);
}

ac_map_t *ac_map_postorder_next(ac_map_t *n) {
  ac_map_t *parent = rb_parent(n);
  if (parent && n == parent->left && parent->right)
    return left_deepest_node(parent->right);
  else
    return parent;
}

/* copy */
static void tree_copy(ac_map_t *n, ac_map_t **res, ac_map_t *parent,
                      ac_map_copy_node_f copy, void *tag) {
  ac_map_t *c = copy(n, tag);
  c->parent_color = n->parent_color;
  rb_set_parent(c, parent);
  if (n->left)
    tree_copy(n->left, &c->left, c, copy, tag);
  else
    c->left = NULL;
  if (n->right)
    tree_copy(n->right, &c->right, c, copy, tag);
  else
    c->right = NULL;
}

ac_map_t *ac_map_copy(ac_map_t *root, ac_map_copy_node_f copy, void *tag) {
  ac_map_t *res = NULL;
  if (root)
    tree_copy(root, &res, NULL, copy, tag);
  return res;
}

static int count_black_nodes(ac_map_t *n) {
  int black_nodes = 0;
  while (n) {
    if (rb_is_black(n))
      black_nodes++;
    n = rb_parent(n);
  }
  return black_nodes;
}

static void print_node_with_color_to_buffer(ac_buffer_t *bh, ac_pool_t *pool,
                                            ac_map_t *n,
                                            print_node_to_string_f print_node) {
  bool red = rb_is_red(n);
  char *s = print_node(pool, n);
  if (red)
    ac_buffer_appends(bh, "\x1B[31m(");
  ac_buffer_appends(bh, s);
  if (red)
    ac_buffer_appends(bh, "\x1B[0m)");
}

bool ac_map_valid(ac_pool_t *pool, ac_map_t *root,
                  print_node_to_string_f print_node) {
  ac_buffer_t *bh = ac_buffer_init(10000);
  bool valid = ac_map_valid_to_buffer(bh, pool, root, print_node);
  if (!valid)
    printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
  return valid;
}

/* test if valid */
bool ac_map_valid_to_buffer(ac_buffer_t *bh, ac_pool_t *pool, ac_map_t *root,
                            print_node_to_string_f print_node) {
  /* an empty tree is valid */
  if (!root)
    return true;
  bool success = true;
  /* the root is black */
  if (rb_is_red(root)) {
    success = false;
    ac_buffer_appendf(bh, "The root is not black!\n");
  }
  ac_map_t *n = ac_map_first(root);
  int black_nodes = 0;
  ac_map_t *first_black_leaf = NULL, *parent;
  ac_map_t *sn = n;
  while (n) {
    if (!n->left && !n->right) { /* only consider leaf nodes */
      black_nodes = count_black_nodes(n);
      first_black_leaf = n;
      break;
    }
    n = ac_map_next(n);
  }
  n = sn;
  while (n) {
    /* check if one child and that child is red */
    if (!n->left) {
      if (n->right) {
        int bn = count_black_nodes(n);
        if (bn != black_nodes) {
          success = false;
          print_node_with_color_to_buffer(bh, pool, n, print_node);
          ac_buffer_appendf(
              bh, " has a NULL left child with a different black height than ");
          print_node_with_color_to_buffer(bh, pool, first_black_leaf,
                                          print_node);
          ac_buffer_appendf(bh, "\n");
        }
      }
    } else if (!n->right) {
      int bn = count_black_nodes(n);
      if (bn != black_nodes) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        ac_buffer_appendf(
            bh, " has a NULL right child with a different black height than ");
        print_node_with_color_to_buffer(bh, pool, first_black_leaf, print_node);
        ac_buffer_appendf(bh, "\n");
      }
    }

    if (n->left && !n->right && rb_is_black(n->left)) {
      success = false;
      print_node_with_color_to_buffer(bh, pool, n, print_node);
      ac_buffer_appendf(bh, " has one left child and it isn't red\n");
    }
    if (!n->left && n->right && rb_is_black(n->right)) {
      success = false;
      print_node_with_color_to_buffer(bh, pool, n, print_node);
      ac_buffer_appendf(bh, " has one right child and it isn't red\n");
    }
    if (rb_is_red(n)) {
      if (n->left && rb_is_red(n->left)) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        ac_buffer_appendf(bh, " has a red left child and is red\n");
      }
      if (n->right && rb_is_red(n->right)) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        ac_buffer_appendf(bh, " has a red right child and is red\n");
      }
      parent = rb_parent(n);
      if (parent && rb_is_red(parent)) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        ac_buffer_appendf(bh, " has a red parent and is red\n");
      }
    }
    if (!n->left && !n->right) { /* only consider leaf nodes */
      int bn = count_black_nodes(n);
      if (black_nodes != bn) {
        success = false;
        print_node_with_color_to_buffer(bh, pool, n, print_node);
        ac_buffer_appendf(bh, " has a different black height than ");
        print_node_with_color_to_buffer(bh, pool, first_black_leaf, print_node);
        ac_buffer_appendf(bh, "\n");
      }
    }
    n = ac_map_next(n);
  }
  return success;
}

/* print */
typedef struct ac_map_print_s {
  size_t position;
  char *printed_key;
  size_t length;
  bool black;
  int depth;
  struct ac_map_print_s *parent;
  struct ac_map_print_s *left, *right;
} ac_map_print_t;

static int get_black_height(ac_map_t *n) {
  int depth = 0;
  while (n) {
    if (rb_is_black(n))
      depth++;
    n = rb_parent(n);
  }
  return depth;
}

static char *get_printed_key(ac_pool_t *pool, ac_map_t *n,
                             print_node_to_string_f print_node, int flags) {
  char *s = print_node(pool, n);
  bool red = rb_is_red(n);
  if (red && (flags & AC_MAP_DONT_PRINT_RED) == 1)
    red = false;

  if ((flags & AC_MAP_DONT_PRINT_BLACK_HEIGHT) == 0)
    return ac_pool_strdupf(pool, "%s%s-%d%s", red ? "(" : "", s,
                           get_black_height(n), red ? ")" : "");
  else
    return ac_pool_strdupf(pool, "%s%s%s", red ? "(" : "", s, red ? ")" : "");
}

static void copy_tree_to_print(ac_pool_t *pool, ac_map_t *node,
                               ac_map_print_t **res, ac_map_print_t *parent,
                               print_node_to_string_f print_node, int flags) {
  ac_map_print_t *copy =
      (ac_map_print_t *)ac_pool_alloc(pool, sizeof(ac_map_print_t));
  *res = copy;

  copy->printed_key = get_printed_key(pool, node, print_node, flags);
  copy->length = strlen(copy->printed_key);
  copy->black = rb_is_black(node) ? true : false;
  copy->position =
      parent ? ((parent->left == copy) ? parent->position
                                       : parent->position + parent->length + 1)
             : 0;
  copy->depth = 1;
  copy->left = NULL;
  copy->right = NULL;
  copy->parent = parent;

  if (node->left)
    copy_tree_to_print(pool, node->left, &copy->left, copy, print_node, flags);
  if (node->right)
    copy_tree_to_print(pool, node->right, &copy->right, copy, print_node,
                       flags);
}

static ac_map_print_t *find_left_parent_with_right_child(ac_map_print_t *item,
                                                         int *depth) {
  while (item->parent &&
         (item->parent->right == item || !item->parent->right)) {
    *depth += item->depth;
    item = item->parent;
  }
  *depth += item->depth;
  return item->parent;
}

static ac_map_print_t *find_left_most_at_depth(ac_map_print_t *item,
                                               int depth) {
  if (!item)
    return NULL;

  if (depth <= item->depth)
    return item;
  if (item->left) {
    ac_map_print_t *r =
        find_left_most_at_depth(item->left, depth - item->depth);
    if (r)
      return r;
  }
  if (item->right) {
    ac_map_print_t *r =
        find_left_most_at_depth(item->right, depth - item->depth);
    if (r)
      return r;
  }
  return NULL;
}

static ac_map_print_t *find_next_peer(ac_map_print_t *item, int depth) {
  while (item) {
    ac_map_print_t *p = find_left_parent_with_right_child(item, &depth);
    if (!p)
      return NULL;
    ac_map_print_t *np = find_left_most_at_depth(p->right, depth);
    if (np)
      return np;
    item = p;
  }
  return NULL;
}

static int get_node_depth(ac_map_print_t *item) {
  int r = 0;
  while (item) {
    r += item->depth;
    item = item->parent;
  }
  return r;
}

void ac_map_print(ac_pool_t *pool, ac_map_t *root,
                  print_node_to_string_f print_node, int flags) {
  ac_buffer_t *bh = ac_buffer_init(10000);
  ac_map_print_to_buffer(bh, pool, root, print_node, flags);
  if (ac_buffer_length(bh))
    printf("%s\n", ac_buffer_data(bh));
  ac_buffer_destroy(bh);
}

void ac_map_print_to_buffer(ac_buffer_t *bh, ac_pool_t *pool, ac_map_t *root,
                            print_node_to_string_f print_node, int flags) {
  if (!root)
    return;

  ac_map_print_t *printable = NULL;
  copy_tree_to_print(pool, root, &printable, NULL, print_node, flags);

  ac_map_print_t *sn, *n, *n2;
  int actual_depth;
  int depth = 1;
  while (true) {
    sn = find_left_most_at_depth(printable, depth);
    if (!sn)
      break;
    n = sn;
    int position = 0;
    while (n) {
      for (; position < n->position; position++)
        ac_buffer_appendc(bh, ' ');
      actual_depth = get_node_depth(n);
      if (actual_depth == depth) {
        n2 = find_next_peer(n, 0);
        int extra = 0;
        if (n->right)
          extra = 2;

        if (n2 && (n->position + n->length + 1 + extra > n2->position)) {
          n->depth++;
          ac_buffer_appendc(bh, '|');
          position++;
        } else {
          if (!n->black && (flags & AC_MAP_DONT_PRINT_RED) == 0)
            ac_buffer_appends(bh, "\x1B[31m");
          ac_buffer_appends(bh, n->printed_key);
          if (!n->black && (flags & AC_MAP_DONT_PRINT_RED) == 0)
            ac_buffer_appends(bh, "\x1B[0m");
          position += n->length;
        }
        n = n2;
      } else {
        n = find_next_peer(n, depth - actual_depth);
        ac_buffer_appendc(bh, '|');
        position++;
      }
    }
    ac_buffer_appendc(bh, '\n');
    position = 0;
    n = sn;
    while (n) {
      for (; position < n->position; position++)
        ac_buffer_appendc(bh, ' ');
      actual_depth = get_node_depth(n);
      if (actual_depth == depth) {
        if (n->left) {
          ac_buffer_appendc(bh, '|');
          position++;
        }
        for (; position < n->position + n->length; position++)
          ac_buffer_appendc(bh, ' ');
        if (n->right) {
          ac_buffer_appendc(bh, '\\');
          position++;
        }
      } else {
        ac_buffer_appendc(bh, '|');
        position++;
      }
      n = find_next_peer(n, depth - actual_depth);
    }
    ac_buffer_appendc(bh, '\n');
    depth++;
  }
}

static void rotate_left(ac_map_t *A, ac_map_t **root) {
  ac_map_t *new_root = A->right;

  size_t tmp_pc = A->parent_color;
  A->parent_color = new_root->parent_color;
  new_root->parent_color = tmp_pc;
  ac_map_t *parent = rb_parent(new_root);
  if (parent) {
    if (parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
  } else
    *root = new_root;

  ac_map_t *tmp = new_root->left;
  new_root->left = A;
  rb_set_parent(A, new_root);

  A->right = tmp;
  if (tmp)
    rb_set_parent(tmp, A);
}

static void rotate_right(ac_map_t *A, ac_map_t **root) {
  ac_map_t *new_root = A->left;
  size_t tmp_pc = A->parent_color;
  A->parent_color = new_root->parent_color;
  new_root->parent_color = tmp_pc;
  ac_map_t *parent = rb_parent(new_root);
  if (parent) {
    if (parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
  } else
    *root = new_root;

  ac_map_t *tmp = new_root->right;
  new_root->right = A;
  rb_set_parent(A, new_root);

  A->left = tmp;
  if (tmp)
    rb_set_parent(tmp, A);
}

void ac_map_fix_insert(ac_map_t *node, ac_map_t *parent, ac_map_t **root) {
  rb_set_red(node);
  rb_set_parent(node, parent);
  node->left = node->right = NULL;

  ac_map_t *grandparent, *uncle;

  while (true) {
    parent = rb_parent(node);
    if (!parent) {
      rb_clear_black(node);
      break;
    }

    if (rb_is_black(parent))
      break;

    grandparent = rb_parent(parent);
    if (grandparent->left == parent) {
      uncle = grandparent->right;
      if (uncle && rb_is_red(uncle)) {
        rb_set_red(grandparent);
        rb_set_black(parent);
        rb_set_black(uncle);
        node = grandparent;
        continue;
      }
      if (parent->right == node)
        rotate_left(parent, NULL);
      rotate_right(grandparent, root);
      break;
    } else {
      uncle = grandparent->left;
      if (uncle && rb_is_red(uncle)) {
        rb_set_red(grandparent);
        rb_set_black(parent);
        rb_set_black(uncle);
        node = grandparent;
        continue;
      }
      if (parent->left == node)
        rotate_right(parent, NULL);
      rotate_left(grandparent, root);
      break;
    }
  }
}

static void fix_color_for_erase(ac_map_t *parent, ac_map_t *node,
                                ac_map_t **root) {
  ac_map_t *sibling;
  if (parent->right != node) {
    sibling = parent->right;
    if (rb_is_red(sibling)) {
      rotate_left(parent, root);
      sibling = parent->right;
    }
    if (sibling->right && rb_is_red(sibling->right)) {
      rb_set_black(sibling->right);
      rotate_left(parent, root);
    } else if (sibling->left && rb_is_red(sibling->left)) {
      rotate_right(sibling, root);
      rotate_left(parent, root);
      rb_set_black(sibling);
    } else {
      rb_set_red(sibling);
      if (rb_parent(parent) && rb_is_black(parent))
        fix_color_for_erase(rb_parent(parent), parent, root);
      else
        rb_set_black(parent);
    }
  } else {
    sibling = parent->left;
    if (rb_is_red(sibling)) {
      rotate_right(parent, root);
      sibling = parent->left;
    }
    if (sibling->left && rb_is_red(sibling->left)) {
      rb_set_black(sibling->left);
      rotate_right(parent, root);
    } else if (sibling->right && rb_is_red(sibling->right)) {
      rotate_left(sibling, root);
      rotate_right(parent, root);
      rb_set_black(sibling);
    } else {
      rb_set_red(sibling);
      if (rb_parent(parent) && rb_is_black(parent))
        fix_color_for_erase(rb_parent(parent), parent, root);
      else
        rb_set_black(parent);
    }
  }
}

static inline void replace_node_with_child(ac_map_t *child, ac_map_t *node,
                                           ac_map_t **root) {
  ac_map_t *parent = rb_parent(node);
  if (parent) {
    if (parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  } else
    *root = child;

  child->parent_color = node->parent_color;
}

bool ac_map_erase(ac_map_t *node, ac_map_t **root) {
  ac_map_t *parent = rb_parent(node);
  if (!node->left) {
    if (node->right)
      replace_node_with_child(node->right, node, root);
    else {
      if (parent) {
        if (parent->left == node)
          parent->left = NULL;
        else
          parent->right = NULL;
        if (rb_is_black(node))
          fix_color_for_erase(parent, NULL, root);
      } else
        *root = NULL;
    }
  } else if (!node->right)
    replace_node_with_child(node->left, node, root);
  else {
    ac_map_t *successor = node->right;
    if (!successor->left) {
      bool black = rb_is_black(successor);
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      rb_set_parent(successor->left, successor);
      if (successor->right)
        rb_set_black(successor->right);
      else {
        if (black)
          fix_color_for_erase(successor, NULL, root);
      }
    } else {
      while (successor->left)
        successor = successor->left;

      bool black = rb_is_black(successor);
      ac_map_t *right = successor->right;
      ac_map_t *parent = rb_parent(successor);
      parent->left = right;
      if (right) {
        rb_clear_black(right);
        rb_set_parent(right, parent);
        black = false;
      }
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      rb_set_parent(successor->left, successor);
      successor->right = node->right;
      rb_set_parent(successor->right, successor);
      if (black)
        fix_color_for_erase(parent, NULL, root);
    }
  }
  return true;
}
