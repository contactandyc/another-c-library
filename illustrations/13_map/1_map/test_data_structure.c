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

#include "another-c-library/ac_map.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  ac_map_node_t map;
  char key;
} node_t;

static inline int compare_node(node_t *v1, node_t *v2) {
  if(v1->key != v2->key)
    return (v1->key < v2->key) ? -1 : 1;
  return 0;
}

static inline int compare_node_for_find(char key, node_t *v) {
  if(key != v->key)
    return (key < v->key) ? -1 : 1;
  return 0;
}

char * print_node(ac_pool_t *pool, ac_map_node_t *n) {
  node_t *node = (node_t *)n;
  return ac_pool_strdupf(pool, "%c", node->key );
}


ac_map_insert_m(node_insert, node_t, compare_node);
ac_map_find_m(node_find, char, node_t, compare_node_for_find);

/*
	 arg contains a number of characters (or letters).  Each letter is inserted
	 into the data structure assuming that it hasn't already been inserted.
	 The characters are inserted in the order that they are found within the
	 string.  Because this is a tool to test the data structure, I also test that
	 if an insert should work, that it does work.  Once the insert succeeds, test
	 if the item can be found.  After all of the inserts are done, the root of
	 the data structure is returned.
*/
ac_map_node_t *fill_data_structure(ac_pool_t *pool, const char *arg) {
  ac_map_node_t *root = NULL;
  const char *s = arg;
  while (*s != 0) {
    if (!node_find(*s, root)) {
      node_t *n = (node_t *)ac_pool_alloc(pool, sizeof(*n));
      n->key = *s;
      if (!node_insert(n, &root)) {
        printf("Find failed for %c and insert failed as well!\n", *s);
        abort();
      }
      if(!ac_map_valid(pool, root, print_node)) {
        ac_map_print(pool, root, print_node, 0);
        abort();
      }
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
ac_map_node_t *fill_data_structure_randomly(ac_pool_t *pool, const char *arg) {
  ac_map_node_t *root = NULL;
  const char *p = arg;
  int len = strlen(p);
  int num_inserted = 0;
  while (num_inserted < len) {
    int pos = rand() % len;
    const char *s = p+pos;
    if (!node_find(*s, root)) {
      node_t *n = (node_t *)ac_pool_alloc(pool, sizeof(*n));
      n->key = *s;
      if (!node_insert(n, &root)) {
        printf("Find failed for %c and insert failed as well!\n", *s);
        abort();
      }
      if(!ac_map_valid(pool, root, print_node)) {
        ac_map_print(pool, root, print_node, 0);
        abort();
      }
      if (!node_find(*s, root)) {
        printf("Find failed for %c after insert succeeded!\n", *s);
        abort();
      }
      num_inserted++;
    }
  }
  return root;
}

/*
	find_everything checks that every character in arg is properly inserted in
	the data structure.
*/
void find_everything(const char *arg, ac_map_node_t *root) {
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
void find_and_erase_everything(ac_pool_t *pool,
                               const char *arg,
                               ac_map_node_t *root) {
  const char *p = arg; // find and erase all of the nodes
  int len = strlen(p);
  int num_destroyed = 0;
  while (num_destroyed < len) {
    int pos = rand() % len;
    const char *s = p+pos;
    node_t *node_to_erase = node_find(*s, root);
    // printf( "erasing %c\n", *s );
    // ac_map_print(pool, root, print_node, 0);
    if (node_to_erase) {
      if (!ac_map_erase((ac_map_node_t *)node_to_erase, &root)) {
        printf("Erase failed for %c after find succeeded!\n", *s);
        abort();
      }
      if(!ac_map_valid(pool, root, print_node)) {
        ac_map_print(pool, root, print_node, 0);
        abort();
      }
      num_destroyed++;
    }
  }
}

/*
	print the datastructure iterating over the keys using first/next
*/
void print_using_iteration(ac_map_node_t *root) {
  printf("print_using_iteration: ");
  ac_map_node_t *n = ac_map_first(root);
  while (n) {
    printf("%c", ((node_t *)n)->key);
    n = ac_map_next(n);
  }
  printf("\n");
}

/*
	print the datastructure backwards iterating over the keys using last/previous
*/
void print_using_reverse_iteration(ac_map_node_t *root) {
  printf("print_using_reverse_iteration: ");
  ac_map_node_t *n = ac_map_last(root);
  while (n) {
    printf("%c", ((node_t *)n)->key);
    n = ac_map_previous(n);
  }
  printf("\n");
}

/*
	destroy the data structure by iterating over the keys using the
	first_to_erase/next_to_erase methods
*/
void destroy_using_iteration(ac_map_node_t *root) {
  ac_map_node_t *n = ac_map_postorder_first(root);
  while (n) {
    ac_map_node_t *next = ac_map_postorder_next(n);
    // nothing really to do since we used the pool!
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
void test_data_structure(ac_pool_t *pool, const char *arg, int repeat) {
  printf("Creating %s using map\n", arg);
  ac_map_node_t *root;
  for( int i=0; i<repeat; i++ ) {
    root = fill_data_structure_randomly(pool, arg);
    // ac_map_print(pool, root, print_node, 0);
    find_everything(arg, root);
    find_and_erase_everything(pool, arg, root);
  }
  root = fill_data_structure(pool, arg);
  ac_map_print(pool, root, print_node, 0);
  // print_using_iteration(root);
  //if (node_previous_supported())
  //  print_using_reverse_iteration(root);
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


ac_pool_t *gpool = NULL;
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

  ac_pool_t *pool = ac_pool_init(1024);
  gpool = pool;
	char *arg = get_valid_characters(argv[1]);
  test_data_structure(pool, arg, repeat);
	free(arg);
  ac_pool_destroy(pool);
  return 0;
}
