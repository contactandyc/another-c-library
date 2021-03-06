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
  struct node_s *next;
  char key;
};

node_t *node_init(char key) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->next = NULL;
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
  *root = node_to_insert;
  return true;
}

void node_print(node_t *n) {
  printf("root");
  while (n) {
    printf(" -> %c", n->key);
    n = n->next;
  }
  printf(" -> NULL");
  printf("\n");
}

bool node_erase(node_t *node_to_erase, node_t **root) {
  node_t *prev = NULL;
  node_t *n = (*root);
  while (n != node_to_erase) { // no end of list check
    prev = n;
    n = n->next; // this will cause the program to crash if n is NULL (which is
                 // what we are doing anyways)
  }
  if (prev)
    prev->next = n->next;
  else
    *root = n->next;
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

bool node_previous_supported() { return false; }

node_t *node_previous(node_t *n) { return NULL; }

node_t *node_first_to_erase(node_t *n) { return n; }

node_t *node_next_to_erase(node_t *n) { return n->next; }
