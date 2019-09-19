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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	 arg contains a number of characters (or letters).  Each letter is inserted
	 into the data structure assuming that it hasn't already been inserted.
	 The characters are inserted in the order that they are found within the
	 string.  Because this is a tool to test the data structure, I also test that
	 if an insert should work, that it does work.  Once the insert succeeds, test
	 if the item can be found.  After all of the inserts are done, the root of
	 the data structure is returned.
*/
node_t *fill_data_structure(stla_pool_t *pool, const char *arg) {
  node_t *root = NULL;
  const char *s = arg;
  while (*s != 0) {
    if (!node_find(*s, root)) {
      node_t *n = node_init(*s);
      if (!node_insert(n, &root)) {
        printf("Find failed for %c and insert failed as well!\n", *s);
        abort();
      }
      node_test(pool, root);
      if (!node_find(*s, root)) {
        printf("Find failed for %c after insert succeeded!\n", *s);
        abort();
      }
    }
    s++;
  }
  return root;
}

/*
	This function is similar to fill_data_structure, except that the characters
	are inserted in random order.
*/
node_t *fill_data_structure_randomly(stla_pool_t *pool, const char *arg) {
  node_t *root = NULL;
  const char *p = arg;
  int len = strlen(p);
  int num_inserted = 0;
  while (num_inserted < len) {
    int pos = rand() % len;
    const char *s = p+pos;
    if (!node_find(*s, root)) {
      node_t *n = node_init(*s);
      if (!node_insert(n, &root)) {
        printf("Find failed for %c and insert failed as well!\n", *s);
        abort();
      }
      node_test(pool, root);
      num_inserted++;
      if (!node_find(*s, root)) {
        printf("Find failed for %c after insert succeeded!\n", *s);
        abort();
      }
    }
  }
  return root;
}

/*
	find_everything checks that every character in arg is properly inserted in
	the data structure.
*/
void find_everything(const char *arg, node_t *root) {
  const char *s = arg; // check that everything can still be found
  while (*s != 0) {
    if (!node_find(*s, root)) {
      printf("Find failed for %c after binary tree was constructed!\n", *s);
      abort();
    }
    s++;
  }
}

/*
	find_and_erase_everything iterates over all of the characters in arg in a
	random order and erases them.  erase should succeed if find succeeds.  If it
	doesn't an error is printed.
*/
void find_and_erase_everything(stla_pool_t *pool, const char *arg, node_t *root) {
  const char *p = arg; // find and erase all of the nodes
  int len = strlen(p);
  int num_destroyed = 0;
  while (num_destroyed < len) {
    int pos = rand() % len;
    const char *s = p+pos;
    node_t *node_to_erase = node_find(*s, root);
    if (node_to_erase) {
      if (!node_erase(node_to_erase, &root)) {
        printf("Erase failed for %c after find succeeded!\n", *s);
        abort();
      }
      node_test(pool, root);
      num_destroyed++;
      node_destroy(node_to_erase);
    }
  }
}

/*
	print the datastructure iterating over the keys using first/next
*/
void print_using_iteration(node_t *root) {
  printf("print_using_iteration: ");
  node_t *n = node_first(root);
  while (n) {
    printf("%c", node_key(n));
    n = node_next(n);
  }
  printf("\n");
}

/*
	print the datastructure backwards iterating over the keys using last/previous
*/
void print_using_reverse_iteration(node_t *root) {
  printf("print_using_reverse_iteration: ");
  node_t *n = node_last(root);
  while (n) {
    printf("%c", node_key(n));
    n = node_previous(n);
  }
  printf("\n");
}

/*
	destroy the data structure by iterating over the keys using the
	first_to_erase/next_to_erase methods
*/
void destroy_using_iteration(node_t *root) {
  node_t *n = node_first_to_erase(root);
  while (n) {
    node_t *next = node_next_to_erase(n);
    node_destroy(n);
    n = next;
  }
}

/*
1.  Create the data structure N times (100000) using the letters that are found
    in arg.  Randomize the insertion and erase order.
2.  Create the data structure using the letters that are found in arg by
	  inserting them in the order of the string.  Print the whole data structure
		using the overall print method and using iteration.
3.  Finally, destroy the structure by using the first_node_to_erase,
    node_next_to_erase methods (which may be defined to be the same as
		first/next)
*/
void test_data_structure(stla_pool_t *pool, const char *arg, int repeat) {
  printf("Creating %s for %s\n", DATA_STRUCTURE, arg);
  node_t *root;
  for( int i=0; i<repeat; i++ ) {
    root = fill_data_structure_randomly(pool, arg);
    node_print(pool, root);
    find_everything(arg, root);
    find_and_erase_everything(pool, arg, root);
  }
  root = fill_data_structure(pool, arg);
  node_print(pool, root);
  print_using_iteration(root);
  if (node_previous_supported())
    print_using_reverse_iteration(root);
  destroy_using_iteration(root);
  printf("\n");
}

/* The various tests will use A-Z, a-z, and 0-9 for valid keys */
bool valid_char(int ch) {
  if(ch >= 'A' && ch <= 'Z')
    return true;
  if(ch >= 'a' && ch <= 'z')
    return true;
  if(ch >= '0' && ch <= '9')
    return true;
  return false;
}

/* Create a copy of the input string (p) and only include characters which are
	 valid (based upon valid_char above).  Also, check that each character is
	 unique.  The wp is only updated and incremented if a character is new and is
	 valid
*/
char *get_valid_characters(const char *p) {
	char *res = strdup(p);
	char *wp = res;
	char *s;
	while(*p != 0) {
		if(valid_char(*p)) {
			s = res;
			while(s < wp) {
				if(*p == *s)
					break;
				s++;
			}
			if(s == wp)
				*wp++ = *p;
		}
		p++;
	}
	*wp = 0;
	return res;
}

/*
  The main function expects to have 2 or more command line arguments and calls
	the function test_data_structure with each argument (after extracting valid
	characters).  If there are less than two arguments, a usage statement is
	printed and the program exits.  One useful tip from the code below is that in
	C or C++ you can chain a series of string constants together by just having
	spacing or a newline character separating the string constants.
*/
int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf( "%s <sequence of characters for tree> [sequence2] ..."
            " [sequenceN]\n",
            argv[0] );
    printf( "  Constructs data structures and runs various tests on them.\n");
    return 0;
  }
  int repeat = 0;
  if(argc < 3 || sscanf(argv[2], "%d", &repeat) != 1)
    repeat = 0;

  stla_pool_t *pool = stla_pool_init(1024);
	char *arg = get_valid_characters(argv[1]);
  test_data_structure(pool, arg, repeat);
	free(arg);
  // stla_pool_destroy(pool);
  return 0;
}
