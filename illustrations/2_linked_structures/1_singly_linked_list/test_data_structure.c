#include "data_structure.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

node_t *fill_data_structure(const char *arg) {
  node_t *root = NULL;
  const char *s = arg;
  while (*s != 0) {
    if (*s >= 'A' && *s <= 'Z') {
      if (!node_find(*s, root)) {
        node_t *n = node_init(*s);
        if (!node_insert(n, &root)) {
          printf("Find failed for %c and insert failed as well!\n", *s);
          abort();
        }
        if (!node_find(*s, root)) {
          printf("Find failed for %c after insert succeeded!\n", *s);
          abort();
        }
      }
    }
    s++;
  }
  return root;
}

void find_everything(const char *arg, node_t *root) {
  const char *s = arg; // check that everything can still be found
  while (*s != 0) {
    if (*s >= 'A' && *s <= 'Z') {
      if (!node_find(*s, root)) {
        printf("Find failed for %c after binary tree was constructed!\n", *s);
        abort();
      }
    }
    s++;
  }
}

void find_and_erase_everything(const char *arg, node_t *root) {
  const char *s = arg; // find and erase all of the nodes
  while (*s != 0) {
    if (*s >= 'A' && *s <= 'Z') {
      node_t *node_to_erase = node_find(*s, root);
      if (node_to_erase) {
        if (!node_erase(node_to_erase, &root)) {
          printf("Erase failed for %c after find succeeded!\n", *s);
          abort();
        }
        node_destroy(node_to_erase);
      }
    }
    s++;
  }
}

void print_using_iteration(node_t *root) {
  printf("print_using_iteration: ");
  node_t *n = node_first(root);
  while (n) {
    printf("%c", node_key(n));
    n = node_next(n);
  }
  printf("\n");
}

void print_using_reverse_iteration(node_t *root) {
  printf("print_using_reverse_iteration: ");
  node_t *n = node_last(root);
  while (n) {
    printf("%c", node_key(n));
    n = node_previous(n);
  }
  printf("\n");
}

void destroy_using_iteration(node_t *root) {
  node_t *n = node_first_to_erase(root);
  while (n) {
    node_t *next = node_next_to_erase(n);
    node_destroy(n);
    n = next;
  }
}

void test_data_structure(const char *arg) {
  printf("Creating %s for %s\n", DATA_STRUCTURE, arg);
  node_t *root = fill_data_structure(arg);
  node_print(root);
  find_everything(arg, root);
  find_and_erase_everything(arg, root);
  root = fill_data_structure(arg);
  print_using_iteration(root);
  if (node_previous_supported())
    print_using_reverse_iteration(root);
  destroy_using_iteration(root);
  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("%s <sequence of uppercase characters for tree> [sequence2] ... "
           "[sequenceN]\n",
           argv[0]);
    printf("  Constructs data structures and runs various tests on them.\n");
    return 0;
  }
  for (int i = 1; i < argc; i++)
    test_data_structure(argv[i]);
  return 0;
}
