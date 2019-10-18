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
node_t *fill_data_structure(acpool_t *pool, const char *arg) {
  node_t *root = NULL;
  const char *s = arg;
  while (*s != 0) {
    if (!node_find(*s, root)) {
      node_t *n = node_init(pool, *s);
      if (!node_red_black_insert(n, &root)) {
        printf("Find failed for %c and insert failed as well!\n", *s);
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

void strip_string(char *s) {
  if(strlen(s) == 0)
    return;

  char *p = s + strlen(s) - 1;
  if(p >= s && (*p == '\n' || *p == '\r'))
    *p-- = 0;
  if(p >= s && (*p == '\n' || *p == '\r'))
    *p-- = 0;
}

void do_help() {
  printf( "insert <key>\n" );
  printf( "  insert key\n");
  printf( "erase <key>\n" );
  printf( "  erase key\n");
  printf( "right <key>\n" );
  printf( "  rotate to the right around key\n");
  printf( "left <key>\n" );
  printf( "  rotate to the left around key\n");
  printf( "Red <key>\n");
  printf( "  color key red\n");
  printf( "Black <key>\n");
  printf( "  color key black\n");
  printf( "color <key>\n");
  printf( "  recoloring changes the key to red and its children to black\n");
  printf( "help - print this help\n" );
  printf( "quit - exit the program\n" );
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

char *get_key(char *p, char *key) {
  while(true) {
    while(*p && *p != ' ')
      p++;
    while(*p == ' ')
      p++;
    if(*p == 0)
      break;
    if(!valid_char(*p))
      continue;
    break;
  }
  *key = *p;
  return p;
}

typedef struct tree_log_s {
  node_t *tree;
  char *op;
  struct tree_log_s *next;
} tree_log_t;

tree_log_t *head, *tail;

void post_good_tree(acpool_t *pool, node_t *root) {
  tree_log_t *n = (tree_log_t *)acpool_alloc(pool, sizeof(tree_log_t));
  n->tree = tree_copy(pool, root);
  n->op = (char *)"Valid Red Black Tree";
  n->next = NULL;
  head = tail = n;
}

bool be_quiet = false;

void do_test_red_black(acpool_t *pool, node_t *root, char *p) {
  tree_log_t *n = (tree_log_t *)acpool_alloc(pool, sizeof(tree_log_t));
  n->tree = tree_copy(pool, root);
  n->op = p;
  n->next = NULL;
  tail->next = n;
  tail = n;

  if(test_red_black_rules(pool, root)) {
    /* tree is okay */
    if(!be_quiet) {
      tree_log_t *n = head;
      while(n) {
        if(n == head) {
          printf( "Starting with a valid red black tree\n" );
          printf( "====================================\n" );
        }
        else
          printf( "Operation: %s\n", n->op );
        node_print(pool, n->tree);
        test_red_black_rules(pool, n->tree);
        printf( "\n" );
        n = n->next;
      }
    }
    printf( "The above tree is a valid red black tree\n" );
    post_good_tree(pool, root);
  }
}

void do_paint_red(acpool_t *pool, node_t *root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, root);
    if(n) {
      color_node_red(n);
      if(!replay) {
        node_print(pool, root);
        do_test_red_black(pool, root, acpool_strdupf(pool, "Red %c", key));
      }
    }
    else
      printf( "%c not found\n", key );
  }
}

void do_paint_black(acpool_t *pool, node_t *root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, root);
    if(n) {
      color_node_black(n);
      if(!replay) {
        node_print(pool, root);
        do_test_red_black(pool, root, acpool_strdupf(pool, "black %c", key));
      }
    }
    else
      printf( "%c not found\n", key );
  }
}

void do_recolor(acpool_t *pool, node_t *root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, root);
    if(n) {
      if(n->color == BLACK && n->left && n->left->color == RED && n->right && n->right->color == RED) {
        recolor(n);
        if(!replay) {
          node_print(pool, root);
          do_test_red_black(pool, root, acpool_strdupf(pool, "color %c", key));
        }
      }
      else {
        printf( "recolor is meant to turn a black node red and its children black\n" );
      }
    }
    else
      printf( "%c not found\n", key );
  }
}



void do_insert(acpool_t *pool, node_t **root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, *root);
    if(!n) {
      n = node_init(pool, key);
      color_node_red(n);
      node_insert(n, root);
      if(!replay) {
        node_print(pool, *root);
        do_test_red_black(pool, *root, acpool_strdupf(pool, "insert %c", key));
      }
    }
    else
      printf( "%c already exists!\n", key );
  }
}


void do_erase(acpool_t *pool, node_t **root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, *root);
    if(n) {
      node_erase(n, root);
      if(!replay) {
        node_print(pool, *root);
        do_test_red_black(pool, *root, acpool_strdupf(pool, "erase %c", key));
      }
    }
    else
      printf( "%c doesn\'t exists\n", key );
  }
}


void do_right_rotate(acpool_t *pool, node_t **root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, *root);
    if(n) {
      if(n->left) {
        rotate_right(n, root);
        if(!replay) {
          node_print(pool, *root);
          do_test_red_black(pool, *root, acpool_strdupf(pool, "right_rotate %c", key));
        }
      }
      else
        printf( "The node you specified does not have a left child to rotate right\n");
    }
    else
      printf( "%c doesn\'t exists\n", key );
  }
}

void do_left_rotate(acpool_t *pool, node_t **root, char *p, bool replay) {
  char key;
  p = get_key(p, &key);
  if(!key) {
    printf( "ERROR: key not specified\n\n");
    do_help();
  }
  else {
    node_t *n = node_find(key, *root);
    if(n) {
      if(n->right) {
        rotate_left(n, root);
        if(!replay) {
          node_print(pool, *root);
          do_test_red_black(pool, *root, acpool_strdupf(pool, "left_rotate %c", key));
        }
      }
      else
        printf( "The node you specified does not have a right child to rotate left\n");
    }
    else
      printf( "%c doesn\'t exists\n", key );
  }
}

void do_tagline() {
  if(!be_quiet)
    printf( "(i)nsert, (e)rase, (r)ight_rotate, (l)eft_rotate, (R)ed, (b)lack, re(c)olor, (h)elp, (q)uit\n");
}

void tree_operations(acpool_t *pool, char *args) {
  node_t *root = args ? fill_data_structure(pool, args) : NULL;
  post_good_tree(pool, root);
  if(root)
    node_print(pool, root);
  char str[1000];
  do_tagline();
  while(fgets(str, 999, stdin) != NULL) {
    strip_string(str);
    if(*str == 0)
      continue;

    char *p = str;
    if(*p == 'q')
      break;

    switch(*p) {
    case 'i':
      do_insert(pool, &root, p, false);
      break;
    case 'e':
      do_erase(pool, &root, p, false);
      break;
    case 'r':
      do_right_rotate(pool, &root, p, false);
      break;
    case 'l':
      do_left_rotate(pool, &root, p, false);
      break;
    case 'R':
      do_paint_red(pool, root, p, false);
      break;
    case 'b':
      do_paint_black(pool, root, p, false);
      break;
    case 'c':
      do_recolor(pool, root, p, false);
      break;
    default:
      do_help();
      break;
    };
    printf("\n");
    do_tagline();
  }

  printf("\n");
}

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

int main(int argc, char *argv[]) {
  if(argc > 1 && !strcmp(argv[1], "-q")) {
    argc--;
    argv++;
    be_quiet = true;
  }
  char *arg = argc > 1 ? get_valid_characters(argv[1]) : NULL;
  acpool_t *pool = acpool_init(1024);
  tree_operations(pool, arg);
  acpool_destroy(pool);
  if(arg)
    free(arg);
  return 0;
}
