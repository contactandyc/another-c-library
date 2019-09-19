#include "data_structure.h"

#include <stdio.h>
#include <stdlib.h>

struct node_s {
  struct node_s *parent;
  struct node_s *left;
  struct node_s *right;
  char key;
};

node_t *node_init(char key) {
  node_t *n = (node_t *)stla_malloc(sizeof(node_t));
  n->left = n->right = n->parent = NULL;
  n->key = key;
  return n;
}

void node_destroy(node_t *n) {
  stla_free(n);
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
  return true;
}



typedef struct node_print_item_s {
  size_t position;
  char *printed_key;
  size_t length;
  int depth;
  struct node_print_item_s *parent;
  struct node_print_item_s *left, *right;
} node_print_item_t;

static int get_depth(node_t *n) {
  int depth = 0;
  while (n) {
    depth++;
    n = n->parent;
  }
  return depth;
}

char *get_printed_key(stla_pool_t *pool, node_t *n ) {
  int depth=get_depth(n);
  int r=rand() % 15;
  char *res = (char *)stla_pool_ualloc(pool, r+4);
  for( int i=0; i<=r; i++ )
    res[i] = n->key;
  sprintf(res+r+1, "%d", depth);
  return res;
}

void copy_tree(stla_pool_t *pool, node_t *node,
               node_print_item_t **res, node_print_item_t *parent ) {
  node_print_item_t *copy = (node_print_item_t *)stla_pool_alloc(pool, sizeof(node_print_item_t));
  *res = copy;

  copy->printed_key = get_printed_key(pool, node);
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

node_print_item_t *find_left_parent_with_right_child( node_print_item_t * item, int *depth ) {
  while(item->parent && (item->parent->right == item || !item->parent->right)) {
    *depth += item->depth;
    item = item->parent;
  }
  *depth += item->depth;
  return item->parent;
}

node_print_item_t *find_left_most_at_depth( node_print_item_t * item, int depth ) {
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

node_print_item_t *find_next_peer( node_print_item_t * item, int depth ) {
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

int get_node_depth( node_print_item_t *item ) {
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

  node_print_item_t *sn,*n,*n2,*n3;
  int actual_depth, depth2;
  int depth=1;
  while(true) {
    int position = 0;
    sn = find_left_most_at_depth(printable, depth);
    if(!sn)
      break;
    n = sn;
    while(n) {
      for( ; position<n->position; position++ )
        printf( " ");
      actual_depth=get_node_depth(n);
      if(actual_depth == depth) {
        n2 = find_next_peer(n, 0);
        if(n2) {
          depth2 = get_node_depth(n2);
        }
        int extra = 0;
        if(n->right)
          extra = 2;

        if(n2 && (n->position+n->length+1+extra > n2->position)) {
          n->depth++;
          printf( "|");
          position++;
        }
        else {
          printf("%s", n->printed_key);
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
      n2 = find_next_peer(n, depth-actual_depth);
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
      n = n2;
    }
    printf( "\n");
    depth++;
  }
}

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
