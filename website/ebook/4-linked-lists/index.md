---
path: "/4-linked-lists"
posttype: "tutorial"
title: "4. Linked Lists"
---

## A data structure interface

In the data structure interface, we will explore the singly linked list, the doubly linked list, binary search trees, and finally, the red-black tree.  Each of these builds upon each other in complexity and can share a similar interface.  The code and interfaces are meant more for understanding and to show various optimizations along the way.  Linked lists and binary search trees allow for inserting, finding, erasing, and iteration.

The code that follows is found in illustrations/linked\_structures/1\_singly\_linked\_list
If you change to that directory, you will find the following Makefile.

```Makefile
DATA_STRUCTURE=singly_linked_list

all: test_data_structure examples

test_data_structure: test_data_structure.c data_structure.h $(DATA_STRUCTURE).c
	gcc test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

examples:
	@./test_data_structure AB CAB ABCDEF AFCEDB

clean:
	rm -f test_data_structure *~
```

When you run
```
make
```

The first block with a colon will be run.  In this Makefile, it is the following line.
```Makefile
all: test_data_structure examples
```

The all group simply refers to other groups to be built.  In this case, it is test\_data\_structure and examples.
```Makefile
test_data_structure: test_data_structure.c data_structure.h $(DATA_STRUCTURE).c
	gcc test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

examples:
	@./test_data_structure AB CAB ABCDEF AFCEDB
```

The lines of code after test\_data\_structure will run if the file called test\_data\_structure is older than the files after the colon.  If you edit test\_data\_structure.c, data\_structure.h or singly\_linked\_list.h (same as $(DATA_STRUCTURE).h), the program will be built using the following line of code.  $(DATA\_STRUCTURE) is converted to singly\_linked\_list.  The output is test\_data\_structure.
```Makefile
gcc test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"
```

If you didn't want to use a Makefile to build test\_data\_structure, you could do so from the command line using the following command.

```
gcc test_data_structure.c singly_linked_list.c -o test_data_structure -DDATA_STRUCTURE=\"singly_linked_list\"
```

The examples block will run every time because it doesn't have any dependencies, and examples isn't a file that exists.  If you were to create a file called examples, then the examples block would cease to run.  The @ symbol prevents the command from being printed to the screen.  By running `make`, you will effectively build test\_data\_structure if it needs to build and run the examples block.  Running `make clean` will clean up the binary.  You can run any block by specifying it.  `make all` is equivalent to running `make` as `make` runs the first block.  If you just want to run the examples block, you can run `make examples`.  

Go ahead and read the code in illustrations/3\_linked\_structures/1\_singly\_linked\_list and then continue with this book.  The code is inlined in the book as well, but it never hurts to read code and try and understand it.  To illustrate how red-black trees work, it makes sense to understand linked lists and binary search trees well first.  A great way to understand code is to have something which you can build, change, and experiment with.  This first example is a bit more involved than the typical hello world program.  However, it provides the basis for understanding more complicated work and is about as simple as I can make it.

If you run `make`, you will get the following output.

```
$ make
gcc test_data_structure.c singly_linked_list.c -o test_data_structure -DDATA_STRUCTURE=\"singly_linked_list\"
Creating singly_linked_list for AB
root -> B -> A -> NULL
print_using_iteration: BA

Creating singly_linked_list for CAB
root -> B -> A -> C -> NULL
print_using_iteration: BAC

Creating singly_linked_list for ABCDEF
root -> F -> E -> D -> C -> B -> A -> NULL
print_using_iteration: FEDCBA

Creating singly_linked_list for AFCEDB
root -> B -> D -> E -> C -> F -> A -> NULL
print_using_iteration: BDECFA
```

After the code is built, <b>./test_data_structure AB CAB ABCDEF AFCEDB</b> is run, which creates four linked lists based upon the arguments passed in.  The output shows how the input transforms into a linked list with arrows (->) for pointers and letters for each node.  The linked lists will place the items in reverse order (notice that BA becomes root -> A -> B -> NULL).  root is just a pointer and not a node.  For your data structure to be accessible, you must maintain a reference to it.  NULL indicates that B is the last node in the list and does not point to another node.

The primary goal is to build a red-black tree with an interface that looks something like the following (it'll change once we address production concerns).  The data\_structure.h is an interface that can be used for singly-linked lists, doubly linked lists, binary search trees, and the red-black tree.  The interface or header file below separates the implementation from the definition.  The definition is the only thing that test\_data\_structure.c (later) will be able to work with.  When building larger software packages, defining clean interfaces can greatly reduce complexity.

data\_structure.h
```c
#ifndef _data_structure_H
#define _data_structure_H

#include "ac_common.h"

struct node_s;
typedef struct node_s node_t;

/*
  You may or may not want to have methods like node_init, node_key, node_print,
  and node_destroy.  My purpose in having them is to provide a uniform interface
  for test_data_structure.  This is only an interface for testing.
*/
node_t *node_init(char key);
void node_destroy(node_t *n);
char node_key(node_t *n);
void node_print(node_t *root);

/*
  Given a data structure, find a node given a pointer to the root node and the key of interest.
*/
node_t *node_find(char key, node_t *root);

/*
  If the key doesn't exist, insert a node into the data structure.  The node_to_insert has 
  already been initialized by the client application (most likely through node_init).
  The node's physical location in memory will not change as a result of inserting or erasing 
  nodes.  Because the root might change, a pointer to the root is passed to node_insert.
*/
bool node_insert(node_t *node_to_insert, node_t **root);

/*
  Once finding a node, erase the node from the data structure.  The node_to_erase is 
  typically found through node_find.  The pointer to root is passed to node_erase as the root 
  might change.  node_erase doesn't free resources used by node_to_erase.  It only unlinks it 
  from the data structure.
*/
bool node_erase(node_t *node_to_erase, node_t **root);

/* Iteration
  All of the linked structures will support first and next.
  Some can support last and previous as well.
*/
bool node_previous_supported();
node_t *node_first(node_t *root);
node_t *node_last(node_t *root);
node_t *node_next(node_t *n);
node_t *node_previous(node_t *n);

/*
  To destroy a data structure, you will need to use the following pattern...
  node_t *n = node_first_to_erase(root);
  while(n) {
    node_t *next = node_next_to_erase(n);
    node_destroy(n);
    n = next;
  }

  Notice that a temporary link to the next node to erase is saved before destroying the node.
  For binary search trees, the beginning to the end isn't the same as a sorted order.  It's 
  known as postorder iteration.  Postorder iteration can be useful for fast construction and 
  destruction of binary search trees.  By having an alternate iterator for destruction, we can 
  have one interface which works for several data structure types.
*/
node_t *node_first_to_erase(node_t *root);
node_t *node_next_to_erase(node_t *n);

#endif
```

In C, you can specify a struct without indicating how it is defined in a header file.  The details can either be hidden in the implementation or declared later.  By declaring the structure in this way, outside applications won't be able to access members of the struct.  I generally try to hide implementation details, providing a minimalistic interface.
```c
struct node_s;
typedef struct node_s node_t;
```

The rest of the functions were commented inline above.

## The data structure interface test driver

In addition to having a common interface, I have created a set of functions to test the data structure.  The set of functions will include data\_structure.h and test that the code is doing what it is supposed to do and also show how to use the interface.

```c
#include "data_structure.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  arg contains several characters (or letters).  Each letter is inserted
  into the data structure, assuming that it hasn't already been inserted.
  The characters are inserted in the order that they are found within the string.
  Because this is a tool to test the data structure, I also test that if an insert 
  should work, that it does work.  Once the insert succeeds, test if the item can be found.
  After all of the inserts are done, the root of the data structure is returned.
*/
node_t *fill_data_structure(const char *arg) {
  node_t *root = NULL;
  const char *s = arg;
  while (*s != 0) {
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
    s++;
  }
  return root;
}

/*
  This function is similar to fill_data_structure, except that the characters
  are inserted in random order.
*/
node_t *fill_data_structure_randomly(const char *arg) {
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
  find_everything checks that every character in arg is properly inserted in the data structure.
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
  find_and_erase_everything iterates over all of the characters in arg in random order and erases them.
  erase should succeed if find succeeds.  If it doesn't, an error is printed.
*/
void find_and_erase_everything(const char *arg, node_t *root) {
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

/*
  The various tests will use A-Z, a-z, and 0-9 for valid keys
*/
bool valid_char(int ch) {
  if(ch >= 'A' && ch <= 'Z')
    return true;
  if(ch >= 'a' && ch <= 'z')
    return true;
  if(ch >= '0' && ch <= '9')
    return true;
  return false;
}

/*
  Create a copy of the input string (p) and only include valid characters (based upon valid_char above).  Also, check that each character is
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
  The main function expects to have two or more command-line arguments and calls
  the function test_data_structure with each argument (after extracting valid
  characters).  If there are less than two arguments, a usage statement is
  printed, and the program exits.  One useful tip from the code below is that in
  C or C++, you can chain a series of string constants together by just having spacing
  or a newline character separating the string constants.
*/
int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf( "%s <sequence of characters for tree> [sequence2] ..."
            " [sequenceN]\n",
            argv[0] );
    printf( "  Constructs data structures and runs various tests on them.\n");
    return 0;
  }
  for (int i = 1; i < argc; i++) {
		char *arg = get_valid_characters(argv[i]);
    test_data_structure(arg);
		free(arg);
	}
  return 0;
}
```

The code is commented above (it's a good idea to begin reading code from the main function).

The include order is important.  If you place data\_structure.h after the standard includes, you might mask an error where data\_structure.h doesn't include all of its dependencies properly.
```c
#include "data_structure.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
```

## The singly linked list

In the previous section, we implemented a basic interface.  Below is the implementation of the singly-linked list.

```c
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
  while(n != node_to_erase) { // no end of list check
    prev = n;
    n = n->next; // this will cause the program to crash if n is NULL (which is what we are doing anyways)
  }
  if(prev)
    prev->next = n->next;
  else
    *root = n->next;
  return true;
}

node_t *node_first(node_t *n) { return n; }

node_t *node_last(node_t *n) {
  node_t *prev = n;
  while(n) {
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
```

Singly-linked lists have a single link or pointer connecting one node to the next.  The pointer is often called next, but can be called anything.  Because the structure is hidden inside the implementation, there are functions to initialize and destroy the object.  node\_key is also needed to get the key member for the same reason.  
```c
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
```

Insertion into a singly linked list is a very fast operation typically requiring only two lines of code.  

1.  Associate the link on the node to insert it to the root node.  
2.  Set the root node pointer to point to the node to insert.
```c
bool node_insert(node_t *node_to_insert, node_t **root) {
  node_to_insert->next = *root; // line 1
  *root = node_to_insert;       // line 2
  return true;
}
```

Iterating through a linked list might look like the following:
```c
void iterate_through_linked_list(node_t *n) {
  while(n) {
    // do something
    n = n->next;
  }
}
```

To iterate through a linked list looking for a particular match, you would do the following.
```c
node_t *node_find(char key, node_t *root) {
  while (root) {
    if (key == root->key)
      return root;
    root = root->next;
  }
  return NULL;
}
```

With linked-lists, the root node is the first node.  With binary search trees, the first node is the left most node.
```c
node_t *node_first(node_t *n) { return n; }
```

```c
node_t *node_next(node_t *n) { return n->next; }
```

For singly linked lists, iterating backwards is not supported, so node\_previous\_supported should return false and node\_previous should return NULL
```c
bool node_previous_supported() { return false; }

node_t *node_previous(node_t *n) { return NULL; }
```

While singly linked lists cannot be iterated on backwards due to a lack of a previous pointer, you can keep track of the last node as you iterate through the list.  This can be useful for finding the last node in a list or erasing a node.
```c
node_t *node_last(node_t *n) {
  node_t *prev = n;
  while(n) {
    prev = n;
    n = n->next;
  }
  return prev;
}
```

Erasing a node follows a similar approach to node\_last, except the while(n) statement will also test to see if n matches the node to erase.
```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  node_t *prev = NULL;
  while(n && n != node_to_erase) {
    prev = n;
    n = n->next;
  }
```

Once the while loop has finished, if n is not NULL, then it must match the node\_to\_erase.  There are two conditions in the case that remains.  If prev is NULL, then the node to erase is at the head or root of the list.  In this case, link the root to the node that the current node to erase links to.  Otherwise, link the prev->next pointer to n's next pointer.  Finally, return true.  If n was NULL, then return false.  It is most likely that n will not be NULL (otherwise, we wouldn't have the node to erase in the first place).  Because of this, I choose to have this condition first.
```c
  if(n) {
    if(prev)
      prev->next = n->next;
    else
      *root = n->next;
    return true;
  }
  abort(); // this should never happen!
  return false;
}
```

A slightly less performant approach where we test for n being NULL first reduces nesting.  I use both approaches in practice (and sometimes don't test for n at all).
```c
  if(!n) abort();
  if(prev)
    prev->next = n->next;
  else
    *root = n->next;
  return true;
}
```

We could even consider implementing the code with the condition that node\_to\_erase must exist.  Because I'm choosing to abort if n is NULL due to incorrect usage, the version below is what ultimately ended up in our code.  This code would abort if n was NULL anyways in the while loop if the node\_to\_erase didn't exist.
```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  node_t *prev = NULL;
  while(n != node_to_erase) { // no end of list check
    prev = n;
    n = n->next; // this will cause the program to crash if n is NULL (which is what we are doing anyways)
  }
  if(prev)
    prev->next = n->next; // this will cause a crash if n is not found
  else
    *root = n->next; // this will cause a crash if n is not found
  return true;
}
```

Finally, it is fine for linked lists to use the first to last order for destroying the list.  node\_first\_to\_erase and node\_next\_to\_erase are similar to node\_first and node\_next respectively.
```c
node_t *node_first_to_erase(node_t *n) { return n; }

node_t *node_next_to_erase(node_t *n) { return n->next; }
```

Now that we are through the code, if you haven't already done it, you can run `make` in illustrations/linked\_structures/1\_singly\_linked\_list.  The output should look like the following.

```
$ make
gcc test_data_structure.c singly_linked_list.c -o test_data_structure -DDATA_STRUCTURE=\"singly_linked_list\"
Creating singly_linked_list for AB
root -> B -> A -> NULL
print_using_iteration: BA

Creating singly_linked_list for CAB
root -> B -> A -> C -> NULL
print_using_iteration: BAC

Creating singly_linked_list for ABCDEF
root -> F -> E -> D -> C -> B -> A -> NULL
print_using_iteration: FEDCBA

Creating singly_linked_list for AFCEDB
root -> B -> D -> E -> C -> F -> A -> NULL
print_using_iteration: BDECFA
```

You might notice that the linked lists are reversed.  This is because we are using the two-line insert technique and putting items at the front of the list.  When using singly-linked lists, it is common to maintain a head (or root) and a tail pointer.  The tail refers to the last node or NULL if the list is empty.  By maintaining a tail, one can efficiently insert items at the end of the list.  If you don't maintain a tail and still wish to insert items at the end of the list, you can do that by skipping to the last node and then linking it to your new node from the last node.

## The doubly linked list

Doubly linked lists introduce node pointers in both directions.  A doubly linked list will have a next and a previous pointer (the naming is up to you).  For example, in the case of a doubly-linked list for B -> A -> C, the following would hold.  We will denote a link starting from a node with a - and the direction being > next, or < previous.  You can think of the linkage as illustrated below.

```
B -> A, B -< NULL (B links to A for next pointer and NULL for the previous pointer)
A -> C, A -< B (A links to C for next pointer and B for the previous pointer)
C -> NULL, C -< A (C links to NULL for next pointer and A for the previous pointer)
```

Next pointers are the same as singly linked list.
```
root -> B -> A -> C -> NULL
```

Previous pointers are new.  Note that B (the root or head node) doesn't point back to the root pointer.  The root pointer is not a node, it is just a way of remembering where the data structure lives in memory (as it's a pointer to the first node).
```
NULL <- B <- A <- C
```

The full code for the doubly linked list is found in illustrations/linked\_structures/2\_doubly\_linked_list.  The code is identical, with the following exceptions in doubly\_linked_list.c (name changed from singly\_linked\_list.c) and the Makefile where the DATA\_STRUCTURE constant changed to doubly\_linked\_list.

The node\_s struct changes from
```c
struct node_s {
  struct node_s *next;
  char key;
};
```

To the following which includes a previous pointer.
```c
struct node_s {
  struct node_s *next;
  struct node_s *previous;
  char key;
};
```

The node\_previous\_supported and node\_previous are changed from
```c
bool node_previous_supported() { return false; }

node_t *node_previous(node_t *n) { return NULL; }
```

To the following because there is now a method for iterating through previous elements.
```c
bool node_previous_supported() { return true; }

node_t *node_previous(node_t *n) { return n->previous; }
```

The node\_init method needs to change from
```c
node_t *node_init(char key) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->next = NULL;
  n->key = key;
  return n;
}
```

To the following to initialize the previous member.  It is possibly more efficient to assign multiple variables of the same type to a single value when you are able to.  I have done this in the `n->next = n->previous = NULL;` line below.
```c
node_t *node_init(char key) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->next = n->previous = NULL;
  n->key = key;
  return n;
}
```

The insert method will change from
```c
bool node_insert(node_t *node_to_insert, node_t **root) {
  node_to_insert->next = *root;
  *root = node_to_insert;
  return true;
}
```

The following demonstrates how to initialize the previous pointer properly.  After the node\_to\_insert is added to the root or head, if there was a previous node at the root, then link that node's previous pointer to the node\_to\_insert.
```c
bool node_insert(node_t *node_to_insert, node_t **root) {
  node_to_insert->next = *root;
  if (node_to_insert->next)
    node_to_insert->next->previous = node_to_insert;
  *root = node_to_insert;
  return true;
}
```

The erase method will change from
```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  node_t *prev = NULL;
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
```

To the following because we don't need to find the node before the given node to erase the node.  The previous pointer already exists on the node, so we can unlink or erase the node from the data structure simply by relinking the nodes that are before (previous) and after (next) to the given node.  

If there is a previous node to the node to erase (n), then link the previous node's next pointer to the same value as n's next pointer.  If n's next pointer is not NULL, then link the next node's previous pointer to n's previous pointer.

If there is not a previous node to the node to erase (n), then the node that is being erased is the first node in the list.  Reset the root pointer to point to the same value as n's next pointer.  If n has a non-NULL next pointer (it isn't the only node in the list), then link the node that is pointed to by n's next pointer to NULL as the node that is pointed to by n's next pointer has become the new root or head of the list.
```c
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
```

The last function that changed is the node\_print method.  node\_print is changed to print two lines.  The first line is the same as the singly linked list in that it shows the next pointers.  The second line shows the previous pointers.  The second line prints only if the list is not empty.
```c
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
```

Next up is a brief explanation of threads, and then we will use what we've learned to create a very useful allocation object.  In this chapter, I began relaxing how much I'm explaining.  A very important skill is to learn to read code (regardless of how hard it is to read).  As this book progresses, there will be less in the way of explanations.

[Table of Contents (only if viewing on Github)](../../../README.md)
