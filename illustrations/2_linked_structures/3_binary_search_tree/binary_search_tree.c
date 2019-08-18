#include "buffer.h"
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
  node_t *node;
  bool spacer;
} node_print_item_t;

void node_print(node_t *root) {
  buffer_t *new_nodes_line = buffer_init(100);
  buffer_t *new_slashes_line = buffer_init(100);
  buffer_t *print_items = buffer_init(100);
  buffer_t *next_items = buffer_init(100);

  node_print_item_t item;
  item.position = 0;
  item.node = root;
  item.spacer = false;
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
    for (size_t i = 0; i < num_items; i++) {
      // add leading whitespace
      buffer_appendn(new_nodes_line, ' ',
                     items[i].position - buffer_length(new_nodes_line));
      buffer_appendn(new_slashes_line, ' ',
                     items[i].position - buffer_length(new_slashes_line));

      if (!items[i].spacer)
        buffer_appendc(new_nodes_line, items[i].node->key);
      else
        buffer_appendc(new_nodes_line, '|');

      if (items[i].spacer || items[i].node->left) {
        node_t *left_node =
            items[i].spacer ? items[i].node : items[i].node->left;
        size_t num_next_items =
            buffer_length(next_items) / sizeof(node_print_item_t);
        node_print_item_t *_next_items =
            (node_print_item_t *)buffer_data(next_items);
        while (num_next_items > 0) {
          num_next_items--;
          if (_next_items[num_next_items].position + 3 < items[i].position)
            break;
          _next_items[num_next_items].spacer = true;
        }

        buffer_appendc(new_slashes_line, '|');
        item.position = items[i].position;
        item.node = left_node;
        item.spacer = items[i].spacer;
        buffer_append(next_items, &item, sizeof(item));
      } else
        buffer_appendc(new_slashes_line, ' ');

      buffer_appendn(new_slashes_line, ' ',
                     buffer_length(new_nodes_line) -
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
