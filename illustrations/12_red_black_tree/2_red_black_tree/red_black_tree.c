#include "data_structure.h"

#include <stdio.h>
#include <stdlib.h>

#define RED 0
#define BLACK 1

struct node_s {
  struct node_s *parent;
  struct node_s *left;
  struct node_s *right;
  size_t color;
  char key;
};

node_t *node_init(char key) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->left = n->right = n->parent = NULL;
  n->key = key;
  return n;
}

void node_destroy(node_t *n) { free(n); }

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

  node_to_insert->parent = parent;
  *n = node_to_insert;
  red_black_insert(node_to_insert, root);
  return true;
}



typedef struct node_print_item_s {
  size_t position;
  char *printed_key;
  bool black;
  size_t length;
  int depth;
  struct node_print_item_s *parent;
  struct node_print_item_s *left, *right;
} node_print_item_t;

static int get_black_height(node_t *n) {
  int black_height = 0;
  while (n) {
    if(n->color == BLACK)
      black_height++;
    n = n->parent;
  }
  return black_height;
}

static char *get_printed_key(stla_pool_t *pool, node_t *n ) {
  return stla_pool_strdupf(pool, "%c%d%s", n->key, get_black_height(n), n->color == BLACK ? "" : "r");
}

static void copy_tree(stla_pool_t *pool, node_t *node,
                      node_print_item_t **res, node_print_item_t *parent ) {
  node_print_item_t *copy = (node_print_item_t *)stla_pool_alloc(pool, sizeof(node_print_item_t));
  *res = copy;

  copy->printed_key = get_printed_key(pool, node);
  copy->black = node->color == BLACK ? true : false;
  copy->length = strlen(copy->printed_key);
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


void node_print(stla_pool_t *pool, node_t *root) {
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
  while (n->parent && n->parent->right == n)
    n = n->parent;
  return n->parent;
}

bool node_previous_supported() { return true; }

node_t *node_previous(node_t *n) {
  if (n->left) {
    n = n->left;
    while (n->right)
      n = n->right;
    return n;
  }
  while (n->parent && n->parent->left == n)
    n = n->parent;
  return n->parent;
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
  node_t *parent = n->parent;
  if (parent && n == parent->left && parent->right)
    return left_deepest_node(parent->right);
  else
    return parent;
}

/*
Red–black tree Properties (https://en.wikipedia.org/wiki/Red–black_tree)

1. Each node is either red or black.
2. The root is black. This rule is sometimes omitted. Since the root can always
  be changed from red to black, but not necessarily vice versa, this rule has
  little effect on analysis.
3. All leaves (NIL) are black.
4. If a node is red, then both its children are black.
5. Every path from a given node to any of its descendant NIL nodes contains the
  same number of black nodes.

My additional rules for clarification which are based upon the first 5 rules.
- If a node has one child, the child must be red
- If a node has two children, the children may be black or red
- The parent of a red node must be black
- The black height of any leaf node must be the same (another way of stating 5)
- A red black tree often will have many more black nodes than red nodes.  This is okay and expected.  The red node is an indication that the tree may be somehow out of balance.  It is possible to have more red nodes than black nodes, but it isn't typical.
- A red black tree has a worst case of a 2logN depth, but is likely to maintain a logN depth or be very close to it.
*/

int count_black_nodes(node_t *n) {
  int black_nodes = 0;
  while(n) {
    if(n->color == BLACK)
      black_nodes++;
    n = n->parent;
  }
  return black_nodes;
}

void node_test(stla_pool_t *pool, node_t *root) {
  /* an empty tree is valid */
  if(!root)
    return;
  /* the root is black */
  if(root->color != BLACK) {
    printf( "The root is not black!\n" );
    node_print(pool, root);
    abort();
  }
  node_t *n = node_first(root);
  int black_nodes = 0;
  while(n) {
    /* this is just a general test of binary tree */
    if(n->parent && n->parent->parent == n) {
      printf( "Node(%c) is the same as it's grandparent(%c)\n", n->key, n->parent->parent->key );
      abort();
    }
    /* this is just a general test of binary tree */
    if(n->parent && n->parent->parent && n->parent->parent->parent == n->parent) {
      printf( "Node->Parent(%c) is the same as Node->Parent\'s grandparent\n", n->parent->key);
      abort();
    }
    /* check if one child and that child is red */
    if(n->left && !n->right && n->left->color != RED) {
      printf( "Node(%c) has one left child and it isn't red (%c)\n", n->key, n->left->key );
      node_print(pool, root);
      abort();
    }
    if(!n->left && n->right && n->right->color != RED) {
      printf( "Node(%c) has one right child and it isn't red (%c)\n", n->key, n->right->key );
      node_print(pool, root);
      abort();
    }
    if(n->color == RED) {
      if(n->left && n->left->color == RED) {
        printf( "The red node(%c) has a red left child(%c)\n", n->key, n->left->key );
        node_print(pool, root);
        abort();
      }
      if(n->right && n->right->color == RED) {
        printf( "The red node(%c) has a red right child(%c)\n", n->key, n->right->key );
        node_print(pool, root);
        abort();
      }
      if(n->parent && n->parent->color == RED) {
        printf( "The red node(%c) has a red parent(%c)\n", n->key, n->parent->key );
        node_print(pool, root);
        abort();
      }
    }
    if(!n->left && !n->right) { /* only consider leaf nodes */
      int black_nodes2 = count_black_nodes(n);
      if(!black_nodes)
        black_nodes = black_nodes2;
      if(black_nodes != black_nodes2) {
        printf( "Black node mismatch(%c): %d != %d\n", n->key, black_nodes, black_nodes2 );
        node_print(pool, root);
        abort();
      }
    }
    n = node_next(n);
  }
}

/*
rotate_right
A
| \
B  C
| \
D  E

becomes
B
| \
D  A
   | \
   C  E

In other words,
A:LEFT becomes new root
A's left child becomes the new root's right child
A becomes right child of the new root
The new root's left child remains left child of the new root (nothing)
A's right child remains right child of A (nothing)

Because this has parent linkage, the following also applies
The new_root's parent is set to A's parent
The new_root's parent left or right is modified to reference new_root (as
  opposed to A).
A's parent becomes new_root
A's left child's parent becomes A
*/

void rotate_left(node_t *A, node_t **root) {
  node_t *new_root = A->right;
  node_t *parent = A->parent;
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
    new_root->parent = parent;
  }
  else {
    new_root->parent = NULL;
    *root = new_root;
  }

  node_t *tmp = new_root->left;
  new_root->left = A;
  A->parent = new_root;

  A->right = tmp;
  if(tmp)
    tmp->parent = A;
}

void rotate_right(node_t *A, node_t **root) {
  node_t *new_root = A->left;
  node_t *parent = A->parent;
  if(parent) {
    if(parent->left == A)
      parent->left = new_root;
    else
      parent->right = new_root;
    new_root->parent = parent;
  }
  else {
    new_root->parent = NULL;
    *root = new_root;
  }

  node_t *tmp = new_root->right;
  new_root->right = A;
  A->parent = new_root;

  A->left = tmp;
  if(tmp)
    tmp->parent = A;
}

void rotate_right_and_swap_colors(node_t *A, node_t **root) {
  size_t tmp = A->color;
  A->color = A->left->color;
  A->left->color = tmp;
  rotate_right(A, root);
}

void rotate_left_and_swap_colors(node_t *A, node_t **root) {
  size_t tmp = A->color;
  A->color = A->right->color;
  A->right->color = tmp;
  rotate_left(A, root);
}

void red_black_insert(node_t *node, node_t **root) {
  node->left = node->right = NULL;
  node->color = RED;
  node_t *parent, *grandparent, *uncle;

  while (true) {
    parent = node->parent;
    if(!parent) {
      node->parent = NULL;
      node->color = BLACK;
      break;
    }

    if(parent->color == BLACK)
      break;

    grandparent = parent->parent;
    if(grandparent->left == parent) {
      uncle = grandparent->right;
      if(uncle && uncle->color == RED) {
        grandparent->color = RED;
        parent->color = uncle->color = BLACK;
        node = grandparent;
        continue;
      }

      if(parent->right == node)
        rotate_left(parent, NULL);
      rotate_right_and_swap_colors(grandparent, root);
      break;
    }
    else {
      uncle = grandparent->left;
      if(uncle && uncle->color == RED) {
        grandparent->color = RED;
        parent->color = uncle->color = BLACK;
        node = grandparent;
        continue;
      }

      if(parent->left == node)
        rotate_right(parent, NULL);
      rotate_left_and_swap_colors(grandparent, root);
      break;
    }
  }
}


void node_fix_color(node_t *parent, node_t *node, node_t **root) {
  /*
    When a node is inserted, the red black tree coloring is fixed by looking
    at the node's uncle (the parent's sibling).  When a node is erased or
    removed, the tree coloring is fixed by looking at the color of the node's
    sibling.

    Initially, the node to erase is NULL as it has been unlinked from the tree.
    Only the parent is known and the parent is known to have exactly one node
    which will be the sibling.  Just like with insert where recursion happens
    and when it does, the node to erase may be a valid node.  Below, we will
    consider cases and refer to N as the node even if it is erased.  If a node
    can be either color, upper and lowercase will be shown (Gg) for example.

    If the sibling is red, then it's children and parent must be black.
    Rotating the tree so that the sibling becomes the parent will look like
    the following.

        P
       / \
      N   s
         / \
        A   B

    becomes

          S
         / \
        P   B
       / \
      N   A

    Using this rotation, the new sibling A is black and the process can
    continue.  Also notice that the sibling remains on the same side.

    From this point, the sibling is black, then the tree will look like
    one of the following.  All of the examples will assume that the sibling
    is on the right side.  All of the lefts and rights are swapped if the
    the sibling is on the left side.

    1.  The parent and sibling is black and has no children or both children
        are black.

            Gg           Gg
           /  \         /  \
          Oo   P  =>  Oo    P    => treat g as the parent and P as the node
              / \            \      to find the sibling, If P is left child of
             N   S            s     g, then the sibling will be the right child
                                    of g.  Repeat the process of fixing the
                                    color with g and the current node being P.

          If the sibling is black and has no children, the sibling is colored
          red and the parent is colored black.  If the parent was already black,
          then the parent becomes double black and the process is repeated with
          the parent's parent and the parent.  If the parent's parent is NULL or
          it is red, simply color the parent black.

          sibling->color = RED;
          if(parent->parent && parent->color == BLACK) {
            node = parent;
            parent = parent->parent;
            continue;
          }
          else // the parent is the root or it is red
            parent->color = BLACK;

      2.  The sibling has a red child in the same direction as the sibling (
          ex. The sibling is to the right of the parent and the right child of
          the sibling is red). This is solved by doing a left rotate after
          painting the right node of the sibling black.

            Pp                Ss
           / \               / \
          N   S       =>    P   R
             / \             \
            Ll  r             Ll

       3.  The sibling has a red child in the opposite direction as the sibling.
           This is solved by right rotating the left child around the sibling
           and swapping colors.  Similar to 2, it is finished by doing a left
           rotate after painting the right node of L to be black.

             Pp          Pp              Ll
            /  \         / \            /  \
           N    S  =>   N   L     =>   P    S
               /             \
              l               s
  */

  node_t *sibling;
  if(parent->right != node) {
    sibling = parent->right;
    if(sibling->color == RED) {
      /* by rotating, the sibling will become black which allows for the other
         cases to be tested. */
      rotate_left_and_swap_colors(parent, root);
      sibling = parent->right;
    }
    if(sibling->left) {
      if(sibling->right) {
        if(sibling->right->color == RED) {
          /* the right sibling is black and the right child of sibling
             is red.  Rotate to the left swapping colors with parent and
             parent's right node setting the sibling->right to BLACK
             prior to rotation. */
          sibling->right->color = BLACK;
          rotate_left_and_swap_colors(parent, root);
        }
        else {
          if(sibling->left->color == RED) {
            rotate_right_and_swap_colors(sibling, root);
            rotate_left_and_swap_colors(parent, root);
            sibling->color = BLACK;
          }
          else {
            /* sibling is black and both children are black */
            sibling->color = RED;
            if(parent->parent && parent->color == BLACK)
              node_fix_color(parent->parent, parent, root);
            else
              parent->color = BLACK;
          }
        }
      }
      else { // must be RED because only child right / left
        rotate_right_and_swap_colors(sibling, root);
        rotate_left_and_swap_colors(parent, root);
        sibling->color = BLACK;
      }
    } else if(sibling->right) { // must be red because only child
      sibling->right->color = BLACK;
      rotate_left_and_swap_colors(parent, root);
    } else {
      sibling->color = RED;
      if(parent->parent && parent->color == BLACK)
        node_fix_color(parent->parent, parent, root);
      else
        parent->color = BLACK;
    }
  }
  else {
    sibling = parent->left;
    if(sibling->color == RED) {
      rotate_right_and_swap_colors(parent, root);
      sibling = parent->left;
    }
    if(sibling->right) {
      if(sibling->left) {
        if(sibling->left->color == RED) {
          sibling->left->color = BLACK;
          rotate_right_and_swap_colors(parent, root);
        }
        else {
          if(sibling->right->color == RED) {
            rotate_left_and_swap_colors(sibling, root);
            rotate_right_and_swap_colors(parent, root);
            sibling->color = BLACK;
          }
          else {
            sibling->color = RED;
            if(parent->parent && parent->color == BLACK)
              node_fix_color(parent->parent, parent, root);
            else
              parent->color = BLACK;
          }
        }
      }
      else {
        rotate_left_and_swap_colors(sibling, root);
        rotate_right_and_swap_colors(parent, root);
        sibling->color = BLACK;
      }
    } else if(sibling->left) { // must be red because only child
      sibling->left->color = BLACK;
      rotate_right_and_swap_colors(parent, root);
    } else {
      sibling->color = RED;
      if(parent->parent && parent->color == BLACK)
        node_fix_color(parent->parent, parent, root);
      else
        parent->color = BLACK;
    }
  }
}

static inline void replace_node_with_child(node_t *child, node_t *node, node_t **root ) {
  node_t *parent = node->parent;
  if(parent) {
    if(parent->left == node)
      parent->left = child;
    else
      parent->right = child;
  }
  else
    *root = child;
  if(child) {
    child->parent = parent;
    child->color = node->color;
  }
}

bool node_erase(node_t *node, node_t **root) {
  if(!node->left) {
    if(node->right) { /* node has one right child */
      replace_node_with_child(node->right, node, root);
    }
    else { /* node has no children, unlink from parent */
      if(node->parent) {
        node_t *parent = node->parent;
        if(parent->left == node)
          parent->left = NULL;
        else
          parent->right = NULL;
        if(node->color == BLACK)
          node_fix_color(parent, NULL, root);
      }
      else { /* no children, no parent, tree is now empty */
        *root = NULL;
      }
    }
  } else if(!node->right) { /* node has one left child */
    replace_node_with_child(node->left, node, root );
  } else { /* node has left and right child */
    node_t *successor = node->right;
    if(!successor->left) { /* successor is to the right of node */
      size_t color = successor->color;
      replace_node_with_child(successor, node, root);
      successor->left = node->left;
      successor->left->parent = successor;
      if(successor->right)
        successor->right->color = BLACK;
      else {
        if(color == BLACK)
          node_fix_color(successor, NULL, root);
      }
    }
    else {
      while(successor->left)
        successor = successor->left;

      if(successor->right) { /* successor is to the left of the right child
                                of node and has a right child */
        /* successor's parent must link to successor on the left */
        successor->parent->left = successor->right;
        successor->right->parent = successor->parent;

        /* replace node with successor */
        replace_node_with_child(successor, node, root);
        successor->left = node->left;
        successor->left->parent = successor;
        successor->right = node->right;
        successor->right->parent = successor;
      }
      else {
        /* unlink successor from its parent */
        node_t *tmp = successor->parent;
        if(tmp->left == successor)
          tmp->left = NULL;
        else
          tmp->right = NULL;
        if(successor->color == RED)
          tmp = NULL;
        size_t color = successor->color;

        /* replace node with successor */
        replace_node_with_child(successor, node, root);
        successor->left = node->left;
        successor->left->parent = successor;
        successor->right = node->right;
        successor->right->parent = successor;

        if(color == BLACK)
          node_fix_color(tmp, NULL, root);
      }
    }
  }
  return true;
}
