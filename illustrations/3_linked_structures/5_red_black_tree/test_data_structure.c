#include "data_structure.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool valid_char(int ch) {
  if(ch >= 'A' && ch <= 'Z')
    return true;
  if(ch >= 'a' && ch <= 'z')
    return true;
  if(ch >= '0' && ch <= '9')
    return true;
  return false;
}


node_t *fill_data_structure(const char *arg) {
  node_t *root = NULL;
  const char *s = arg;
  while (*s != 0) {
    if (valid_char(*s)) {
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

node_t *fill_data_structure_randomly(const char *arg) {
  node_t *root = NULL;
  const char *p = arg;
  int len = strlen(p);
  int num_inserted = 0;
  while (num_inserted < len) {
    int pos = rand() % len;
    const char *s = p+pos;
    if (valid_char(*s)) {
      if (!node_find(*s, root)) {
        node_t *n = node_init(*s);
        printf( "%c", *s );
        if (!node_insert(n, &root)) {
          printf("Find failed for %c and insert failed as well!\n", *s);
          abort();
        }
        num_inserted++;
        if (!node_find(*s, root)) {
          printf("Find failed for %c after insert succeeded!\n", *s);
          abort();
        }
      }
    }
  }
  printf( "\n");
  return root;
}

void find_everything(const char *arg, node_t *root) {
  const char *s = arg; // check that everything can still be found
  while (*s != 0) {
    if (valid_char(*s)) {
      if (!node_find(*s, root)) {
        printf("Find failed for %c after binary tree was constructed!\n", *s);
        abort();
      }
    }
    s++;
  }
}

void find_and_erase_everything(const char *arg, node_t *root) {
  const char *p = arg; // find and erase all of the nodes
  int len = strlen(p);
  int num_destroyed = 0;
  while (num_destroyed < len) {
    int pos = rand() % len;
    const char *s = p+pos;
    if (valid_char(*s)) {
      node_t *node_to_erase = node_find(*s, root);
      if (node_to_erase) {
        printf( "%c\n", *s );
        if (!node_erase(node_to_erase, &root)) {
          printf("Erase failed for %c after find succeeded!\n", *s);
          abort();
        }
        num_destroyed++;
        node_destroy(node_to_erase);
      }
    }
    // s++;
  }
  printf( "\n" );
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
  printf("destroy_using_iteration: ");
  node_t *n = node_first_to_erase(root);
  while (n) {
    node_t *next = node_next_to_erase(n);
    printf("%c", node_key(n));
    node_destroy(n);
    n = next;
  }
  printf("\n");
}

void test_data_structure(const char *arg) {
  printf("Creating %s for %s\n", DATA_STRUCTURE, arg);
  node_t *root;
  for( int i=0; i<100000; i++ ) {
    root = fill_data_structure_randomly(arg);
    // node_print(root);
    find_everything(arg, root);
    find_and_erase_everything(arg, root);
  }
  root = fill_data_structure(arg);
  node_print(root);
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
