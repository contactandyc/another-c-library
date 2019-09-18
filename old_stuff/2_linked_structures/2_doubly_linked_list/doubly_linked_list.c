#include "data_structure.h"

#include <stdio.h>
#include <stdlib.h>

struct node_s {
  struct node_s *next;
  struct node_s *previous;
  char key;
};

node_t *node_init(char key) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->next = n->previous = NULL;
  n->key = key;
  return n;
}

void node_destroy(node_t *n) { free(n); }

char node_key(node_t *n) { return n->key; }

node_t *node_find(char key, node_t *root) {
  while (root) {
    if (key == root->key)
      return root;
    root = root->next;
  }
  return NULL;
}

bool node_insert(node_t *node_to_insert, node_t **root) {
  node_to_insert->next = *root;
  if (node_to_insert->next)
    node_to_insert->next->previous = node_to_insert;
  *root = node_to_insert;
  return true;
}

void node_print(node_t *np) {
  node_t *n = np;
  printf("root");
  while (n) {
    printf(" -> %c", n->key);
    n = n->next;
  }
  printf(" -> NULL");
  printf("\n");
  if (np) {
    n = np;
    printf("NULL");
    while (n) {
      printf(" <- %c", n->key);
      n = n->next;
    }
    printf("\n");
  }
}

bool node_erase(node_t *n, node_t **root) {
  if (n->previous) {
    n->previous->next = n->next;
    if (n->next)
      n->next->previous = n->previous;
  } else {
    *root = n->next;
    if (n->next)
      n->next->previous = NULL;
  }
  return true;
}

node_t *node_first(node_t *n) { return n; }

node_t *node_last(node_t *n) {
  node_t *prev = n;
  while (n) {
    prev = n;
    n = n->next;
  }
  return prev;
}

node_t *node_next(node_t *n) { return n->next; }

bool node_previous_supported() { return true; }

node_t *node_previous(node_t *n) { return n->previous; }

node_t *node_first_to_erase(node_t *n) { return n; }

node_t *node_next_to_erase(node_t *n) { return n->next; }
