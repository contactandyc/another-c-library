#include "buffer.h"
#include "data_structure.h"

#include <stdio.h>
#include <stdlib.h>

struct node_s {
  struct node_s *parent;
  struct node_s *left;
  struct node_s *right;
  size_t color;
  char key;
};

#define RED 0
#define BLACK 1

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
  node_to_insert->left = node_to_insert->right = NULL;
  *n = node_to_insert;
  red_black_insert(node_to_insert, root);
  return true;
}

typedef struct node_print_item_s {
  size_t position;
  node_t *node;
  bool spacer;
} node_print_item_t;

static int get_depth(node_t *n) {
  int depth = 0;
  while (n) {
    if(n->color == BLACK)
      depth++;
    n = n->parent;
  }
  return depth;
}

static int _get_right_depth(node_t *n, int depth) {
  if(n->right)
    depth = _get_right_depth(n->right, depth+1);
  else if(n->left)
    depth = _get_right_depth(n->left, depth);
  return depth;
}

static int get_right_depth(node_t *n) {
  return get_depth(n) + _get_right_depth(n, 0);
}

static node_t *get_root(node_t *n) {
  while(n->parent)
    n = n->parent;
  return n;
}

static int _get_left_depth(node_t *n, int depth) {
  int m = depth;
  if(n->left) {
    int d = _get_left_depth(n->left, depth+1);
    if(d > m)
      m = d;
  }
  if(n->right) {
    int d = _get_left_depth(n->right, depth );
    if(d > m)
      m = d;
  }
  return m;
}

static int get_left_height(node_t *n) {
  return _get_left_depth(n, 0);
}

size_t get_print_length(buffer_t *bh) {
  char *p = buffer_data(bh);
  char *ep = p + buffer_length(bh);
  size_t res = 0;
  while(p < ep) {
    if(*p == 0x1B) {
      p++;
      while(*p && *p != 'm')
        p++;
      if(*p == 'm')
        p++;
    }
    else {
      p++;
      res++;
    }
  }
  return res;
}

void node_print(node_t *root) {
  if (!root)
    return;
  buffer_t *new_nodes_line = buffer_init(100);
  buffer_t *new_slashes_line = buffer_init(100);
  buffer_t *print_items = buffer_init(100);
  buffer_t *next_items = buffer_init(100);

  node_print_item_t item;
  item.position = 0;
  item.node = root;
  item.spacer = false;
  int depth = 0;
  buffer_set(print_items, &item, sizeof(item));
  while (buffer_length(print_items) > 0) {
    // line of text containing node labels
    buffer_clear(new_nodes_line);

    // line of text containing slashes
    buffer_clear(new_slashes_line);

    // for the next loop
    buffer_clear(next_items);
    node_print_item_t *items = (node_print_item_t *)buffer_data(print_items);
    size_t num_items = buffer_length(print_items) / sizeof(node_print_item_t);
    depth++;

    for (size_t i = 0; i < num_items; i++) {
      items[i].spacer = false;
      size_t total = 0;
      for (size_t j = num_items-1; j > i; j--) {
        /* not exactly sure what +2 to +4 does, experimentally found */
        total += get_left_height(items[j].node)+4;
      }
      size_t total2 = get_left_height(get_root(items[i].node));
      if(total && total2 <= total)
        items[i].spacer = true;

      // add leading whitespace
      buffer_appendn(new_nodes_line, ' ',
                     items[i].position - get_print_length(new_nodes_line));
      buffer_appendn(new_slashes_line, ' ',
                     items[i].position - buffer_length(new_slashes_line));

      if (!items[i].spacer) {
        char key = items[i].node->key;
        const char *scode = "";
        const char *ecode = "";
        if(items[i].node->color == RED) {
          scode = "\x1B[31m";
          ecode = "\x1B[0m";
        }
        buffer_appendf(new_nodes_line, "%s%c%d%s", scode, key,
                       get_depth(items[i].node), ecode );
      } else
        buffer_appendc(new_nodes_line, '|');

      if (items[i].spacer || items[i].node->left) {
        node_t *left_node =
            items[i].spacer ? items[i].node : items[i].node->left;
        buffer_appendc(new_slashes_line, '|');
        item.position = items[i].position;
        item.node = left_node;
        item.spacer = items[i].spacer;
        buffer_append(next_items, &item, sizeof(item));
      } else
        buffer_appendc(new_slashes_line, ' ');

      buffer_appendn(new_slashes_line, ' ',
                     get_print_length(new_nodes_line) -
                         buffer_length(new_slashes_line));

      if (!items[i].spacer && items[i].node->right) {
        buffer_appendc(new_slashes_line, '\\');
        item.position = buffer_length(new_slashes_line);
        item.node = items[i].node->right;
        item.spacer = false;
        buffer_append(next_items, &item, sizeof(item));
      } else
        buffer_appendc(new_slashes_line, ' ');
    }
    printf("%s\n", buffer_data(new_nodes_line));
    printf("%s\n", buffer_data(new_slashes_line));
    // swap next_items and print_items
    buffer_t *tmp = print_items;
    print_items = next_items;
    next_items = tmp;
  }
  buffer_destroy(print_items);
  buffer_destroy(next_items);
  buffer_destroy(new_nodes_line);
  buffer_destroy(new_slashes_line);
}

void _debug_node_print(const char *function, int line, node_t *root) {
  printf( "%s:%d\n", function, line );
  node_print(root);
}

#define debug_node_print(x) _debug_node_print(__FUNCTION__, __LINE__, x)

bool node_erase(node_t *node_to_erase, node_t **root) {
  if (node_to_erase->left) {
    if (node_to_erase->right) {
      // case 3
      node_t *next = node_next(node_to_erase);
      node_erase(next, root);
      next->left = node_to_erase->left;
      next->right = node_to_erase->right;
      next->parent = node_to_erase->parent;
      if (next->left)
        next->left->parent = next;
      if (next->right)
        next->right->parent = next;
      if (!next->parent)
        *root = next;
      else {
        if (next->parent->left == node_to_erase)
          next->parent->left = next;
        else
          next->parent->right = next;
      }
    } else {
      // case 2
      node_to_erase->left->parent = node_to_erase->parent;
      if (node_to_erase->parent) {
        if (node_to_erase->parent->left == node_to_erase)
          node_to_erase->parent->left = node_to_erase->left;
        else
          node_to_erase->parent->right = node_to_erase->left;
      } else
        *root = node_to_erase->left;
    }
  } else if (node_to_erase->right) {
    // case 2
    node_to_erase->right->parent = node_to_erase->parent;
    if (node_to_erase->parent) {
      if (node_to_erase->parent->left == node_to_erase) {
        node_to_erase->parent->left = node_to_erase->right;
      } else {
        node_to_erase->parent->right = node_to_erase->right;
      }
    } else
      *root = node_to_erase->right;
  } else {
    // case 1
    if (node_to_erase->parent) {
      if (node_to_erase->parent->left == node_to_erase)
        node_to_erase->parent->left = NULL;
      else
        node_to_erase->parent->right = NULL;
    } else
      *root = NULL;
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
6. If a node has one child, it must be red
7. The parent of a red node must be black
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

void test_red_black_tree(node_t *root) {
  /* an empty tree is valid */
  if(!root)
    return;
  /* the root is black */
  if(root->color != BLACK) {
    printf( "The root is not black!\n" );
    node_print(root);
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
      node_print(root);
      abort();
    }
    if(!n->left && n->right && n->right->color != RED) {
      printf( "Node(%c) has one right child and it isn't red (%c)\n", n->key, n->right->key );
      node_print(root);
      abort();
    }
    if(n->color == RED) {
      if(n->left && n->left->color == RED) {
        printf( "The red node(%c) has a red left child(%c)\n", n->key, n->left->key );
        node_print(root);
        abort();
      }
      if(n->right && n->right->color == RED) {
        printf( "The red node(%c) has a red right child(%c)\n", n->key, n->right->key );
        node_print(root);
        abort();
      }
      if(n->parent && n->parent->color == RED) {
        printf( "The red node(%c) has a red parent(%c)\n", n->key, n->parent->key );
        node_print(root);
        abort();
      }
    }
    if(!n->left && !n->right) { /* only consider leaf nodes */
      int black_nodes2 = count_black_nodes(n);
      if(!black_nodes)
        black_nodes = black_nodes2;
      if(black_nodes != black_nodes2) {
        printf( "Black node mismatch(%c): %d != %d\n", n->key, black_nodes, black_nodes2 );
        node_print(root);
        abort();
      }
    }
    n = node_next(n);
  }
}

static inline void _swap_nodes(node_t *dest,
                               node_t *src,
                               node_t **root) {
  node_t *parent = src->parent;
  if(parent) {
    if(parent->left == src)
      parent->left = dest;
    else
      parent->right = dest;
    dest->parent = parent;
  }
  else {
    dest->parent = NULL;
    *root = dest;
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
  _swap_nodes(new_root, A, root);

  node_t *tmp = new_root->left;
  new_root->left = A;
  A->parent = new_root;

  A->right = tmp;
  if(A->right)
    A->right->parent = A;
}

void rotate_right(node_t *A, node_t **root) {
  node_t *new_root = A->left;
  _swap_nodes(new_root, A, root);

  node_t *tmp = new_root->right;
  new_root->right = A;
  A->parent = new_root;

  A->left = tmp;
  if(A->left)
    A->left->parent = A;
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


void paint_self_red_and_children_black(node_t *n) {
  n->color = RED;
  n->left->color = n->right->color = BLACK;
}

void red_black_insert(node_t *node, node_t **root) {
  /*
    It is assumed that node is linked into it's proper parent and that the
    node is a leaf node.  The red black tree always initially paints the
    given node red.
  */
  node->color = RED;
  node->left = node->right = NULL;
  node_t *parent, *grandparent, *uncle;

  while (1) {
    parent = node->parent;
    if(!parent) {
      /* if the node is the root node, color it black */
      node->parent = NULL;
      node->color = BLACK;
      break;
    }

    /* if the parent is black, the node is red so we're done */
    if(parent->color == BLACK)
      break;
    /* The parent and the node are both red.  It is a violation of
       the red black tree to have two red nodes in a row.

       At this point, the parent must be red and the parent's
       parent would be black as it is a violation of the red
       black tree to have two red nodes in a row.  The red black
       tree insert operation consider's the node's uncle's color.
       The uncle would be the grandparent's other child.
    */
    grandparent = parent->parent;
    if(grandparent->left == parent) {
      /* If the parent is the grandparent's left node, the uncle
         is the right node. */
      uncle = grandparent->right;
      if(uncle && uncle->color == RED) {
        /* If the uncle is red, then the parent and the uncle are
           both red.  The red black tree needs to maintain a
           constant black height.

                   A
                 /   \
                B     C
              /   \
             d     e
            /
           n

           Prior to inserting node (n) which is red, notice that the
           leaf nodes d, e, and C all have a black height of 2 meaning
           that there are only 2 black nodes in the path from the root
           to each of the leaf nodes.  If you recolor d and e black and
           change B to be red, it doesn't change the black height of any
           of the leaf nodes.

                   A
                 /   \
                b     C
              /   \
             D     E
            /
           n

           Notice that in this case, the recoloring created a valid red
           black tree.  The black height is 2 to every leaf node.  The root is
           black.  There are not two red nodes in a row.  In the one case where
           a node only has a single child, the child is red.

           To recap, if the parent and uncle are red, paint the parent and uncle
           black and the grandparent red.  It's possible that the grandparent's
           parent was also red.  To handle this case, we can repeat all of the
           tests recursively (since the recursion is simple, continuing in a
           while loop works by changing the node to the grandparent). */
        grandparent->color = RED;
        parent->color = uncle->color = BLACK;
        node = grandparent;
        continue;
      }
      /* The uncle is black (NULL in this case), then rotate to the right around
         the grandparent.  Notice that while swapping colors during the rotate,
         that the placement of the black node doesn't change. This maintains the
         proper black height.

                 A
               /   \
              B     C
            /
           d
          /
         n

         becomes

               A
             /   \
            D     C
          /   \
         n     b

         using a right rotation around the grandparent (B).

         If n was the right child of the parent, then do an extra left rotation
         to make the tree look like the case above before doing the right
         rotation (there's no need for color swapping because they're both red).

                 A
               /   \
              B     C
            /
           d
            \
             n

         becomes

                 A
               /   \
              B     C
            /
           n
          /
         d

         through a left rotation around the parent (d) and then with a right
         rotation it ultimately becomes.

                 A
               /   \
              N     C
            /   \
           d     b

         using a right rotation around the grandparent (B).
        */
      if(parent->right == node)
        rotate_left(parent, NULL);
      rotate_right_and_swap_colors(grandparent, root);
      break;
    }
    else {
      /* This is the same as the case above where the uncle was the right node,
         except every left is swapped for right and vice versa. */
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
  test_red_black_tree(*root);
}
