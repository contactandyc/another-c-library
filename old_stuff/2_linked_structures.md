# Linked Structures

## A Data Structure Interface

We will explore the singly linked list, the doubly linked list, binary search trees, and finally the red black tree.  Each of these build upon each other in complexity and can actually share a similar interface.  The code and interfaces are meant more for the purpose of understanding and to show various optimizations along the way.  A linked list and binary search trees allow for inserting, finding, erasing, and iteration.

This code that follows is found in illustrations/linked_structures/1_singly_linked_list<br/>
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
```bash
make
```

The first block with a colon will be run.  In this Makefile, it is the following line.
```Makefile
all: test_data_structure examples
```

The all group simply refers to other groups to be built.  In this case it is test_data_structure and examples.
```Makefile
test_data_structure: test_data_structure.c data_structure.h $(DATA_STRUCTURE).c
	gcc test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"

examples:
	@./test_data_structure AB CAB ABCDEF AFCEDB
```

The lines of code after test_data_structure will run if the file called test_data_structure is older than the files after the colon.  If you edit test_data_structure.c, data_structure.h or singly_linked_list.h (same as $(DATA_STRUCTURE).h), the program will be built using the following line of code.  $(DATA_STRUCTURE) is converted to singly_linked_list.  The output is test_data_structure.
```Makefile
gcc test_data_structure.c $(DATA_STRUCTURE).c -o test_data_structure -DDATA_STRUCTURE=\"$(DATA_STRUCTURE)\"
```

If you didn't want to use a Makefile to build test_data_structure, you could do so from the command line using the following command.

```bash
gcc test_data_structure.c singly_linked_list.c -o test_data_structure -DDATA_STRUCTURE=\"singly_linked_list\"
```

The examples block will run everytime because it doesn't have any dependencies and examples isn't a file that exists.  If you were to create a file called examples, then the examples block would cease to run.  The @ symbol prevents the command from being printed to the screen.  By running <b>make</b>, you will effectively build test_data_structure if it needs built and run the examples block.  Running <b>make clean</b> will clean up the binary.  You can run any block by specifying it.  <b>make all</b> is equivalent to running <b>make</b> as <b>make</b> runs the first block.  If you just want to run the examples block, you can by running <b>make examples</b>.  

Go ahead and read the code in illustrations/linked_structures/1_singly_linked_list and then continue with this book.  The code is inlined in the book as well, but it never hurts to read code and try and understand it.  To illustrate how red black trees work, it makes sense to understand linked lists and binary search trees well first.  A great way to understand code is to have something which you can build, change, and experiment with.  This first example is obviously a bit more involved than the typical hello world program.  However, it provides the basis for understanding more complicated work and is about as simple as I can make it.

If you run <b>make</b> you will get the following output.

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

After the code is built, <b>./test_data_structure AB CAB ABCDEF AFCEDB</b> is run which creates 4 linked lists based upon the arguments passed in.  The output shows how the input transforms into a linked list with arrows (->) for pointers and letters for each node.  The linked lists will place the items in reverse order (notice that BA becomes root -> A -> B -> NULL).  root is just a pointer and not a node.  In order for your data structure to be accessible, you must have a reference to it.  NULL is to indicate that B doesn't point to a node.  This doesn't have to be the case, but to show off linked lists, its good to know the 2 instruction insertion method which is demonstrated below.  

The primary goal is to build a red black tree with an interface that looks something like the following (it'll change once we address production concerns).  The data_structure.h is an interface that can be used for singly linked lists, doubly linked lists, binary search trees, and the red black tree.  The interface or header file below separates the implementation from the definition.  The definition is the only thing that test_data_structure.c (later) will be able to work with.  When building larger software packages, defining clean interfaces can greatly reduce complexity.
data_structure.h
```c
#ifndef _data_structure_H
#define _data_structure_H

#include <stdbool.h>

struct node_s;
typedef struct node_s node_t;

/* You may or may not want to have methods like node_init, node_key, node_print,
  and node_destroy.  My purpose in having them is to provide a uniform interface
  for test_data_structure.
*/
node_t *node_init(char key);
void node_destroy(node_t *n);
char node_key(node_t *n);
void node_print(node_t *root);

node_t *node_find(char key, node_t *root);
bool node_insert(node_t *node_to_insert, node_t **root);
bool node_erase(node_t *node_to_erase, node_t **root);

/* Iteration
   All of the linked structures will support first and next.
   Some can support previous as well.
*/
bool node_previous_supported();
node_t *node_first(node_t *root);
node_t *node_last(node_t *root);
node_t *node_next(node_t *n);
node_t *node_previous(node_t *n);

/* To destroy a data structure, you will need to use the following pattern...

   node_t *n = node_first_to_erase(root);
   while(n) {
     node_t *next = node_next_to_erase(n);
     node_destroy(n);
     n = next;
   }

   It is important to get the next item prior to destroying it.  Some data
   structures also require nodes to be destroyed in a certain order.
*/
node_t *node_first_to_erase(node_t *root);
node_t *node_next_to_erase(node_t *n);

#endif
```

Almost every header file in C is wrapped with the following #ifndef/#define/#endif statement.  When other files include the file, this prevents the file's contents from being consumed multiple times.  One gotcha with this is that if another file were to define the same thing.  This would block the file from being included the first time.
```c
#ifndef _data_structure_H
#define _data_structure_H

...

#endif
```

C by default doesn't include the bool type or true and false as values.  stdbool.h includes that type and values.
```c
#include <stdbool.h>
```

In C, you can specify a struct without indicating how it is defined in a header file.  The details can be hidden in the implementation or declared later.  By declaring the structure in this way, outside applications won't be able to access members of the struct.  I generally try and hide implementation details in the implementation and provide as minimal interface as possible.
```c
struct node_s;
typedef struct node_s node_t;
```

Since the node structure's details are hidden, special functions exist to allocate, initialize, free, get the key from, and print.  This is only the interface.  The implementation will vary depending upon whether we are implementing a singly linked list or a red black tree.
```c
node_t *node_init(char key);
void node_destroy(node_t *n);
char node_key(node_t *n);
void node_print(node_t *root);
```

Given a data structure, one typically is able to find a given node given a pointer to the root and the value of interest.
```c
node_t *node_find(char key, node_t *root);
```

If the value doesn't exist, an insert method exists to add it to the data structure.  The node_to_insert has already been initialized by the client application (most likely through node_init).  The node's physical location in memory will not change as a result of inserting or erasing nodes.  Because the root might change, a pointer to the root is passed to node_insert.
```c
bool node_insert(node_t *node_to_insert, node_t **root);
```

Once finding a node, one may wish to erase the node from the data structure.  The node_to_erase is typically found through node_find.  Again the pointer to root is passed to node_erase as the root might change.  node_erase doesn't free resources used by node_to_erase.  It only unlinks it from the data structure.
```c
bool node_erase(node_t *node_to_erase, node_t **root);
```

Data structures typically support some type of iteration.  Singly linked lists don't support finding the previous node without extra information.  Doubly linked lists, binary search trees with a parent node (the type we will be building), and red black trees all support finding the previous node given a node.
```c
bool node_previous_supported();
```

Basic iteration involves finding the first, the last, the next, and the previous nodes given a node or root node.
```c
node_t *node_first(node_t *root);
node_t *node_last(node_t *root);
node_t *node_next(node_t *n);
node_t *node_previous(node_t *n);
```

It is often the case that you can destroy a data structure by iterating over it in a certain manner very efficiently.  The data structure destruction often looks like the following code.

```c
node_t *n = node_first_to_erase(root);
while(n) {
  node_t *next = node_next_to_erase(n);
  node_destroy(n);
  n = next;
}
```

Notice that a temporary link to the next node to erase is saved prior to destroying the node.  For binary search trees, the beginning to the end isn't the same as a sorted order.  It's known as postorder iteration.  Post order iteration can be useful for fast construction and destruction of binary search trees.  By having an alternate iterator for destruction, we can have one interface which works for a number of data structure types.

## The Data Structure Interface Test Driver

In addition to having a common interface, I have created a set of functions to test the data structure.  The set of functions will include data_structure.h and test that the code is doing what it is supposed to do and show how to use the interface.

```c
#include "data_structure.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

node_t *fill_data_structure(const char *s) {
  node_t *root = NULL;
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

void find_everything(const char *s, node_t *root) {
  while (*s != 0) {
    if (*s >= 'A' && *s <= 'Z') {
      if (!node_find(*s, root)) {
        printf("Find failed for %c after binary search tree was constructed!\n", *s);
        abort();
      }
    }
    s++;
  }
}

void find_and_erase_everything(const char *s, node_t *root) {
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
  if(argc < 2) {
    printf( "%s <sequence of uppercase characters for tree> [sequence2] ..."
            " [sequenceN]\n",
            argv[0] );
    printf( "  Constructs data structures and runs various tests on them.\n");
    return 0;
  }
  for (int i = 1; i < argc; i++)
    test_data_structure(argv[i]);
  return 0;
}
```

In my opinion, you should read a C (and a number of other languages) program starting with the main function.  The main function expects to have 2 or more command line arguments and calls test_data_structure with each argument.  If there are less than two arguments, a usage statement is printed and the program exits.  One useful tip from the code below is that in C or C++ you can chain a series of string constants together by just having spacing or newline characters.
```c
int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf( "%s <sequence of uppercase characters for tree> [sequence2] ..."
            " [sequenceN]\n",
            argv[0] );
    printf( "  Constructs data structures and runs various tests on them.\n");
    return 0;
  }
  for (int i = 1; i < argc; i++)
    test_data_structure(argv[i]);
  return 0;
}
```

The test_data_structure function expects a string of characters.  DATA_STRUCTURE is defined in the Makefile.  I've intentionally left out many of the comments in the code above.  Most of the code that you will encounter will not be heavily commented.  If it is heavily commented, sometimes the comments can be wrong.  Finally, it can actually make code harder to read in the end if there are too many comments.
```c
void test_data_structure(const char *arg) {
  // DATA_STRUCTURE is defined in the Makefile
  printf("Creating %s for %s\n", DATA_STRUCTURE, arg);
  // This function will insert all of the letters found in the string of characters
  T node_t *root = fill_data_structure(arg);
  // Print the whole data structure.
  node_print(root);
  // Find all of the letters in the string of characters in the data structure
  find_everything(arg, root);
  // Find each of the letters in the string of characters and erase them one by one
  find_and_erase_everything(arg, root);
  // Insert all of the letters in the string of characters again
  root = fill_data_structure(arg);
  // Print all of the keys in the data structure by iterating from first to the end
  print_using_iteration(root);
  // If you can iterate backwards, print all of the keys in the data structure
  // by iterating from the last to the beginning.
  if (node_previous_supported())
    print_using_reverse_iteration(root);
  // Destroy the tree using the postorder iterators
  destroy_using_iteration(root);
  // Print an extra new line to create some separation should this function be
  // called again.
  printf("\n");
}
```


The first significant function is fill_data_structure.  The pseudocode for the function is below.
```
  iterate over each character in the input string looking for upper case characters
  if the uppercase character is not found in the data structure
    create a node with the given character and attempt to insert it.
    if the insert fails, print an error and abort
    check to see if find works after a successful insert.  If it fails print an
      error and abort

  return a pointer to the data structure with all of the newly inserted characters
```

The actual code is below and I will comment it.  A common exercise that I may resort to is to code in english and then code in C.  Sometimes it is helpful to convert code to english and then back again.  It can work out bugs in the code and errors in logic.
```c
node_t *fill_data_structure(const char *s) {
  // create a pointer to an empty data structure
  node_t *root = NULL;
  // it is faster to advance pointers through addition than to figure out the
  // length of a string and then reference each character by [index].
  // while the end of the string has not been reached
  while (*s != 0) {
    // is the current character between A and Z
    if (*s >= 'A' && *s <= 'Z') {
      // does the current character exist in the data structure
      if (!node_find(*s, root)) {
        // if not, create a new node with the current character as its key
        node_t *n = node_init(*s);
        // insert the new node into the structure.  node_insert will only
        // return false if the node already exists.  If it does return false,
        // this would be a serious error and that is why it aborts.
        if (!node_insert(n, &root)) {
          printf("Find failed for %c and insert failed as well!\n", *s);
          abort();
        }
        // After a success insert, find should work.  If it doesn't that is
        // a serious error.
        if (!node_find(*s, root)) {
          printf("Find failed for %c after insert succeeded!\n", *s);
          abort();
        }
      }
    }
    // advance to the next character in the string.
    s++;
  }
  // return a pointer to the data structure
  return root;
}
```

Once the data structure is fully formed, another good test to run is to make sure that you can still find all of the keys.  find_everything does this and should look very similar to fill_data_structure except that nothing gets inserted.
```c
void find_everything(const char *s, node_t *root) {
  while (*s != 0) {
    if (*s >= 'A' && *s <= 'Z') {
      if (!node_find(*s, root)) {
        printf("Find failed for %c after binary search tree was constructed!\n", *s);
        abort();
      }
    }
    s++;
  }
}
```

There are several ways of destroying a data structure.  If you know all of the elements in the data structure from some other source (as we do with the string), then you can find each node and erase them one by one.  As you find each node, erase and then destroy them.
```c
void find_and_erase_everything(const char *s, node_t *root) {
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
```

Find the first node and then iterate through all of the rest of the nodes using node_next.  Print each key.
```c
void print_using_iteration(node_t *root) {
  printf("print_using_iteration: ");
  node_t *n = node_first(root);
  while (n) {
    printf("%c", node_key(n));
    n = node_next(n);
  }
  printf("\n");
}
```

Find the last node and then iterate through all of the rest of the nodes using node_previous.  Print each key.
```c
void print_using_reverse_iteration(node_t *root) {
  printf("print_using_reverse_iteration: ");
  node_t *n = node_last(root);
  while (n) {
    printf("%c", node_key(n));
    n = node_previous(n);
  }
  printf("\n");
}
```

Find the first node to erase and then iterate over the nodes in the proper order so that they can be destroyed without messing up the internal structure of the data structure.  Notice that this doesn't unlink nodes or erase them.  It is expected that the data structure will be fully destroyed with this function.
```c
void destroy_using_iteration(node_t *root) {
  node_t *n = node_first_to_erase(root);
  while (n) {
    node_t *next = node_next_to_erase(n);
    node_destroy(n);
    n = next;
  }
}
```

The include order is important.  If you place data_structure.h after the standard includes, you might mask an error where data_structure.h doesn't include all of its dependencies properly.
```c
#include "data_structure.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
```

# The Singly Linked List

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


Singly linked lists have a single link or pointer connecting one node to the next.  The pointer is often called next, but can really be anything.  Because the structure is hidden inside the implementation, there are functions to initialize and destroy the object.  node_key is also needed to get the key member for the same reason.  
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

Insertion into a singly linked list is a very fast operation typically requiring two lines of code.  1.  Associate the link on the node to insert to the root node.  2.  Set the root node pointer to point to the node to insert.
```c
bool node_insert(node_t *node_to_insert, node_t **root) {
  node_to_insert->next = *root; // line 1
  *root = node_to_insert;       // line 2
  return true;
}
```

Iterating through a linked list might look like the following..
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

With lists, the node you pass in is the first node.  With binary search trees, the first node is the left most node.
```c
node_t *node_first(node_t *n) { return n; }
```

```c
node_t *node_next(node_t *n) { return n->next; }
```

For the singly linked list, going backwards doesn't work, so node_previous_supported should return false and node_previous should return NULL
```c
bool node_previous_supported() { return false; }

node_t *node_previous(node_t *n) { return NULL; }
```

While singly linked lists don't have the ability to go backwards due to a lack of a previous pointer, you can keep track of the last node as you iterate through the list.  This can be useful for finding the last node in a list or erasing a node.
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

Erasing a node follows a similar approach to begin, except the while(n) statement will also test to see if n matches the node to erase.
```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  node_t *prev = NULL;
  while(n && n != node_to_erase) {
    prev = n;
    n = n->next;
  }
```

Once the while loop has finished, if n is not NULL, then it must match the node_to_erase.  There are two conditions in the case that remain.  If prev is NULL, then the node to erase is at the head or root of the list.  In this case, link the root to the node that the current node to erase links to.  Otherwise, link the prev->next pointer to n's next pointer.  Finally, return true.  If n was NULL, then return false.  It is most likely that n will not be NULL (otherwise we wouldn't have the node to erase in the first place).  Because of this, I choose to have this condition first.
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

A slightly less performant approach where we test for n being NULL first, reduces nesting.  I use both approaches in practice.
```c
  if(!n) abort();
  if(prev)
    prev->next = n->next;
  else
    *root = n->next;
  return true;
}
```

We could even consider implementing the code assuming that node_to_erase must exist like the following.  Because I'm choosing to abort if n is NULL due to incorrect usage, the version below is what ultimately ended up in our code.  This code would abort if n was NULL anyways in the while loop if the node_to_erase didn't exist.
```c
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
```



Finally, it is fine for linked lists to use the first to last order for destroying the list.  node_first_to_erase and node_next_to_erase are similar to node_first and node_next respectively.
```c
node_t *node_first_to_erase(node_t *n) { return n; }

node_t *node_next_to_erase(node_t *n) { return n->next; }
```

Now that we are through the code, if you haven't done it, you can run <b>make</b> in illustrations/linked_structures/1_singly_linked_list.  The output should look like the following.

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

You might notice that the linked lists are reversed.  This is because we are using the two line insert technique and putting items at the front of the list.  When using singly linked lists, it is common to maintain a head (or root) and a tail pointer.  The tail refers to the last node or NULL if the list is empty.  By maintaining a tail, one can efficiently insert items at the end of the list.  If you don't maintain a tail and still wish to insert items at the end of the list, you can do that by skipping to the last node and then linking to your new node from the last node.

# The Doubly Linked List

Doubly linked lists introduce link nodes in both directions.  A doubly linked list will have a next and a previous pointer (the naming is up to you).  For example, in the case of a doubly linked list for B -> A -> C, the following would hold true.  We will denote a link starting from a node with a - and the direction being > next or < previous.  You can think of the linkage like the following illustrations.

```
B -> A, B -< NULL (B links to A for next pointer and NULL for previous pointer)
A -> C, A -< B (A links to C for next pointer and B for previous pointer)
C -> NULL, C -< A (C links to NULL for next pointer and A for previous pointer)
```

Next pointers are the same as singly linked list.
```
root -> B -> A -> C -> NULL
```

Previous pointers are new.  Note that B (the root or head node) doesn't point back to the root pointer.  The root pointer is not a node.  It is just a way of remembering where the data structure lives in memory (as its a pointer to the first node).
```
NULL <- B <- A <- C
```

The full code for the doubly linked list is found in illustrations/linked_structures/2_doubly_linked_list.  The code is identical with the following exceptions in doubly_linked_list.c (name changed from singly_linked_list.c) and the Makefile where the DATA_STRUCTURE constant changed to doubly_linked_list.

The node_s struct changes from
```c
struct node_s {
  struct node_s *next;
  char key;
};
```

To the following to include a previous pointer.
```c
struct node_s {
  struct node_s *next;
  struct node_s *previous;
  char key;
};
```

The node_previous_supported and node_previous are changed from
```c
bool node_previous_supported() { return false; }

node_t *node_previous(node_t *n) { return NULL; }
```

To the following because there is now a method for iterating through previous elements.
```c
bool node_previous_supported() { return true; }

node_t *node_previous(node_t *n) { return n->previous; }
```

The node_init method needs to change from
```c
node_t *node_init(char key) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->next = NULL;
  n->key = key;
  return n;
}
```

To the following to initialize the previous member.  It is possibly more efficient to assign multiple variables of the same type to a single value when you are able as I do in the <b>n->next = n->previous = NULL;</b> line below.
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

To the following to initialize the previous pointer properly.  After the node_to_insert is added to the root or head, if there was a previous node at the root, then link that node's previous pointer to the node_to_insert.
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

To the following because we don't need to find the node prior to the given node to erase the node.  The previous pointer already exists on the node, so we can unlink or erase the node from the data structure simply by relinking the nodes that are before (previous) and after (next) to the given node.  

If there is a previous node to the node to erase (n), then link the previous node's next pointer to the same value as n's next pointer.  If n's next pointer is not NULL, then link the next node's previous pointer to n's previous pointer.

If there is not a previous node to the node to erase (n), then the node that is being erased is the first node in the list.  Reset the root pointer to point the same value as n's next pointer.  If n has a non NULL next pointer (it isn't the only node in the list), then link the node that is pointed to by n's next pointer to NULL as the node that is pointed to by n's next pointer has become the new root or head of the list.
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

The last function that changed is the node_print method.  node_print is changed to print two lines.  The first line is the same as the singly linked list in that it shows next pointers.  The second line shows previous pointers.  The second line is only printed if the list is not empty.
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


# The Binary Search Tree

A binary search tree is similar to a linked list in that nodes are linked together.  The difference is that each node has two children.  The children are generally understood as being left and right where the left is less than the given node and the right is greater.  Trees in computer science generally grow from a root down (unlike trees in nature which grow up from the root).  This is likely due to the fact that in a binary search tree, the root is considered the top of the tree.  In binary search trees, each child node is aware of its parent node.  In the example below, B is aware of D as a parent, A as a left child, and C as a right child.

```
                D
              /   \
             B     H
            / \   / \
           A   C G   I
                /
               E
                 \
                   F     
```

In the example above, the root is D and the left of D is B, right is H.  Each node refers to up to three other nodes.  For example, the B node refers to D, A, and C.  D is B's parent.  A is B's left child and C is B's right child.  Before continuing too much with theory, we will build out a binary search tree.  Just like before, the only files that will be changed are doubly_linked_list.c will be renamed binary_search_tree.c and the Makefile will have DATA_STRUCTURE defined as binary_search_tree.  The binary search tree is much simpler than the red black tree, but it isn't entirely without complexity.

The code that follows can be found in illustrations/linked_structures/3_binary_search_tree

First, let's define the structure of the node.
```c
typedef struct node_s {
  struct node_s *parent;
  struct node_s *left;
  struct node_s *right;
  char key;
} node_t;
```

For a binary search tree, instead of thinking in next and previous, we think in terms of two children and a parent.  The children are considered left and right.  We could name the nodes left_child and right_child and that would indeed be a more precise name.  However, left and right have become common names of the left and right children, so we will use left and right.   A linked list can be sorted if the insert method inserts the node in the right place to maintain a sorted list.  It doesn't have to be and indeed it frequently is not sorted.  The binary search tree is sorted.  In a binary search tree, the root might not be the first element in the list of nodes (if you are considering sort order).  In the example above, D is the root, but A would be the first node in the sorted set.  Linked lists always maintain that the root node is the first node in the list.  In a binary search tree, it is rare that the root node is the first node within the sorted set.  It only happens when the root is the left most node in the tree.

A binary search tree can be empty, much like a linked list can be empty.  If it is empty, it doesn't have any nodes.  Just like a linked list, where you need to create a reference to some part of the list (typically the beginning), you need to maintain a link to the root of the tree.  This is just a pointer to the current root node which can change depending upon your tree operations.  Initializing the root node to NULL, indicates that the tree is empty.

```c
node_t *root = NULL;
```

To build the tree, we will need functionality to insert new nodes.  For now, we will assume that our tree maintains unique keys (no duplicate letters).  In a binary search tree insert method, you first find the place where the node would go, and then if it doesn't already exist, you add the node.  It is often a good idea to search for the node prior to inserting.  This will eliminate the need to create and destroy the node needlessly, if it already existed.  It also is easier to understand the find method, so it comes next.

```c
node_t *node_find(char key, node_t *root) {
  while(root) {
    if(key < root->key)
      root = root->left;
    else if(key > root->key)
      root = root->right;
    else
      return root;
  }
  return NULL;
}
```

You may notice in the code above that the root is no longer the root once it is assigned to its left or right node.  In a binary search tree, you can call the find method with any node in the tree and it will search that node and all of its children.  This fact generally isn't useful, but it is good to understand it as you think about how binary search trees work.  The node_find method above can be converted to a recursive function.

```c
node_t *node_find(char key, node_t *root) {
  if(key < root->key)
    return node_find(key, root->left);
  else if(key > root->key)
    return node_find(key, root->right);
  else
    return root;
}
```

The recursive method requires less code, but is less efficient as the recursion requires more stack manipulation.  Recursive functions are extremely powerful, but when they can be represented as a non-recursive method that is more efficient, it is usually best to use the non-recursive method.

The node_insert method will require a pointer to the root pointer so that it can potentially modify what the root is. Insert works much like find, except that it needs to find the spot where the new node can fit.  To find that spot, the parent of that spot must be saved and the actual spot itself.  Because the spot where the new node can fit will be NULL, you can use the parent pointer from a given node.  Once a place is found, the node to insert's parent is to set to the saved parent, the left and right pointers are set to NULL, and the spot is set to reference the node.
```c
#include <stdbool.h>

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
```

Before getting to erasing nodes, I'm going to cover basic iteration.  Iteration is needed to solve one case of erasing nodes, so we will cover iteration first.

To find the first node in a tree, you simply traverse all of the left pointers until there are no more.  
```c
node_t *node_first(node_t *n) {
  if (!n)
    return NULL;
  while (n->left)
    n = n->left;
  return n;
}
```

Similarly, to find the last node in a tree, you traverse all of the right pointers until there are no more.
```c
node_t *node_last(node_t *n) {
  if (!n)
    return NULL;
  while (n->right)
    n = n->right;
  return n;
}
```

To find the next node, consider if the current node has a right node.  If it does, the find the left most node from the right node.  Otherwise find the first parent where the child is on the left.
```c
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
```

To find the previous node, swap left and right from node_next.  Also note that previous is supported.
```c
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
```

Erasing nodes is a bit more complicated than insertion because the tree may need to be shuffled a bit for nodes which have children.  Consider our example from before.

```
                D
              /   \
             B     H
            / \   / \
           A   C G   I
                /
               E
                 \
                   F     
```

Erasing leaf nodes (nodes without any children are easy).  A, C, F, and I can be erased by simply removing the link from their parent to them.  Erasing nodes which have only one child are relatively simple.  You just replace the child.  To erase E, you would replace E with F.  Replacing nodes with two children isn't too difficult.  Consider removing H.  If you replaced H with I, the tree would still be in tact.  Replacing D is a bit more challenging as H has two children, and if you replaced D with H, how could H point to B, I, and G.  It isn't that big of a problem.  If you reconsider what happened when replacing H with I, you might discover that you were replacing H with the next inorder value (which would be I).  If we look for the next inorder value after D, you would see that it is E.  E has a child F, so F will first take E's place.  Next E can take D's place.  The rules are ...

```
1.  If a node is a leaf, unlink it from it parent
2.  If a node only has one child, replace the node with the child
3.  If a node has two children, find the next inorder node and unlink it first
    using rules 1 and 2.  Next, replace the node with the node that was just
    unlinked.
```

```c
bool node_erase(node_t *node_to_erase, node_t **root) {
  if(node_to_erase->left) {
    if(node_to_erase->right) {
      // case 3
      node_t *next = node_next(node_to_erase);
      node_erase(next, root);
      next->left = node_to_erase->left;
      next->right = node_to_erase->right;
      next->parent = node_to_erase->parent;
      next->left->parent = next;
      next->right->parent = next;
      if(!next->parent)
        *root = next;      
    }
    else {
      // case 2
      node_to_erase->left->parent = node_to_erase->parent;
      if(node_to_erase->parent) {
        if(node_to_erase->parent->left == node_to_erase)
          node_to_erase->parent->left = node_to_erase->left;
        else
          node_to_erase->parent->right = node_to_erase->left;
      }
      else
        *root = node_to_erase->left;           
    }
  }
  else if(node_to_erase->right) {
    // case 2
    node_to_erase->right->parent = node_to_erase->parent;
    if(node_to_erase->parent) {
      if(node_to_erase->parent->left == node_to_erase)
        node_to_erase->parent->left = node_to_erase->right;
      else
        node_to_erase->parent->right = node_to_erase->right;
    }
    else
      *root = node_to_erase->right;     
  }
  else {
    // case 1
    if(node_to_erase->parent) {
      if(node_to_erase->parent->left == node_to_erase)
        node_to_erase->parent->left = NULL;
      else
        node_to_erase->parent->right = NULL;
    }
    else
      *root = NULL;    
  }
  return true;
}
```


Postorder iteration means to iterate through a tree visiting the left and the right nodes before each root.  Postorder starts from the left deepest node of the tree and then iterates through children before parents.  Given the following example, the iteration will look like.

```
        D9
     /      \
    B3       H8
  /   \    /   \
 A1    C2 G6     I7
         /
        E5
          \
           F4
```

or

```
ACBEGFD
```

```
Start with the left deepest node which is A.
Since A is the left child of B, look for the left deepest node under C which is C
Since C is the right child of B, B is next
Since D is the left child of B, look for the left deepest node under H which is F
Since F is the right child of E, E is next
Since G is the left child of G and there isn't a right child, G is next
Since G is the left child of H, look for the left deepest node under I which is I
Since I is the right child of H, H is next
Since H is the right child of D, D is next
Since D has no parent, we are done
```

The left deepest node is where you always choose a child if it exists preferring the left child over the right.  Because this is an internal function, it is safe to assume that (n) will not be NULL.  The function is made static because it is only used within the c file and inline for potential performance.  For example, F is the left deepest node of H.  If a node has no children, that node is returned.
```c
static inline node_t *left_deepest_node(node_t *n) {
  while(true) {
    if(n->left)
      n = n->left;
    else if(n->right)
      n = n->right;
    else
      return n;
  }
}
```

In explaining the traversal above, there are 4 patterns that exist.
```
1.  Start with left deepest node
2.  If node is a left child of parent and there is a right child, look for the
    left deepest node under right child, otherwise use parent.
3.  If node is right child, use parent
4.  If node has no parent, we are done
```

Statements 2-4 are used for finding next nodes in postorder iteration.  Statement 3 and statement 4 are actually the same in as much as every node has a parent, it just may be that the parent is NULL.  The otherwise element of statement 2 also returns the parent.  The first part of statement 2 is the only case where something other than the parent is returned.  Given this, we can state the following about find next nodes in postorder iteration.

```
If node's parent is not NULL and the node is a left child of its parent and
there is a right child, return the left deepest node of the right child of the
node's parent, otherwise return the node's parent.
```

If a tree is destroyed using postorder iteration, all of the children are destroyed before parents, so the tree remains in tact throughout the destruction.  The node_first_to_erase (or first in postorder iteration) calls the left_deepest_node method above if n is not NULL.

```c
node_t *node_first_to_erase(node_t *n) {
  if (!n)
    return NULL;
  return left_deepest_node(n);
}
```

The node_next_to_erase (or next in postorder iteration) uses our statement above.
```c
node_t *node_next_to_erase(node_t *n) {
  node_t *parent = n->parent;
  if (parent && n == parent->left && parent->right)
    return left_deepest_node(parent->right);
  else
    return parent;
}
```

The last function to build is a method to print the tree.  It presented a different kind of challenge that programmers often face and so I'm separating it out.  


# Printing a Binary Search Tree

I didn't know how to print the tree and so I resorted to the internet to find a solution.  I found one on stack overflow that fit my needs but it was written in the wrong language.  Strictly speaking, printing a tree in a graphical way isn't required for one to use or even build one.  However, printing a tree is very important in gaining an intuitive sense of how the tree is functioning.  I had to do some research and was only able to find example code on the internet in python.  The code was found at the following URL and written by <b>Steve Landey</b>.  

https://stackoverflow.com/questions/13674772/printing-out-a-binary-search-tree-with-slashes

He used | instead of / for the left node so he didn't have to worry about spacing at all on the left side.
```
15
| \
3  16
|\   \
2 4   19
|  \  | \
1   | 17 28
|   |      \
0   12      31
    | \
    11 14
```

```python
from collections import namedtuple

# simple node representation. sorry for the mess, but it does represent the
# tree example you gave.
Node = namedtuple('Node', ('label', 'left', 'right'))
def makenode(n, left=None, right=None):
    return Node(str(n), left, right)
root = makenode(
    15,
    makenode(
        3,
        makenode(2, makenode(1, makenode(0))),
        makenode(4, None, makenode(12, makenode(11), makenode(14)))),
    makenode(16, None, makenode(19, makenode(17),
                                makenode(28, None, makenode(31)))))

# takes a dict of {line position: node} and returns a list of lines to print
def print_levels(print_items, lines=None):
    if lines is None:
        lines = []
    if not print_items:
        return lines

    # working position - where we are in the line
    pos = 0

    # line of text containing node labels
    new_nodes_line = []

    # line of text containing slashes
    new_slashes_line = []

    # args for recursive call
    next_items = {}

    # sort dictionary by key and put them in a list of pairs of (position,
    # node)
    sorted_pos_and_node = [
        (k, print_items[k]) for k in sorted(print_items.keys())]

    for position, node in sorted_pos_and_node:
        # add leading whitespace
        while len(new_nodes_line) < position:
            new_nodes_line.append(' ')
        while len(new_slashes_line) < position:
            new_slashes_line.append(' ')

        # update working position
        pos = position
        # add node label to string, as separate characters so list length
        # matches string length
        new_nodes_line.extend(list(node.label))

        # add left child if any
        if node.left is not None:
            # if we're close to overlapping another node, push that node down
            # by adding a parent with label '|' which will make it look like a
            # line dropping down
            for collision in [pos - i for i in range(3)]:
                if collision in next_items:
                    next_items[collision] = makenode(
                        '|', next_items[collision])

            # add the slash and the node to the appropriate places
            new_slashes_line.append('|')
            next_items[position] = node.left
        else:
            new_slashes_line.append(' ')

        # update working position
        len_num = len(node.label)
        pos += len_num

        # add some more whitespace
        while len(new_slashes_line) < position + len_num:
            new_slashes_line.append(' ')

        # and take care of the right child
        if node.right is not None:
            new_slashes_line.append('\\')
            next_items[position + len_num + 1] = node.right
        else:
            new_slashes_line.append(' ')

    # concatenate each line's components and append them to the list
    lines.append(''.join(new_nodes_line))
    lines.append(''.join(new_slashes_line))

    # do it again!
    return print_levels(next_items, lines)

lines = print_levels({0: root})
print '\n'.join(lines)
```

To convert the python to C, you first need to understand what the code is doing.  It is helpful to first convert it to english.  To simplify the process, I will separate out the print_levels function and talk about it next.  I will also remove his comments where it makes sense.  Sometimes it makes sense to also embed some C in the explanation.

The namedtuple is equivalent to a C structure.  We won't need this for our implementation.
```python
from collections import namedtuple
```

Create a Node structure which consists of a label, left, and right members.  Based upon the makenode function, label seems to be a string.
```python
Node = namedtuple('Node', ('label', 'left', 'right'))
````

The makenode function is an interesting way of constructing a tree.  It is very much like the React createElement method in that it allows you to construct a tree by simply embedding children in a nested call.
```
def makenode(n, left=None, right=None):
    return Node(str(n), left, right)

root = makenode(
    15,
    makenode(
        3,
        makenode(2, makenode(1, makenode(0))),
        makenode(4, None, makenode(12, makenode(11), makenode(14)))),
    makenode(16, None, makenode(19, makenode(17),
                                makenode(28, None, makenode(31)))))
```

We can emulate this by carefully ordering our insertion.  If we think about postorder iteration, we can emulate a similar list.
```
15,3,2,1,0,4,12,11,14,16,19,17,28,31
```

We can convert those numbers to letters by adding the letter 'A' to each number and making 28 be 24, and 31 be 25.


Using the following table we can convert those numbers
```
ABCDEFGHIJKLMNOPQRSTUVWXYZ
01234567890123456789012345
```

We can convert those numbers to
```
PDCBAEMLOQTRYZ
```


In python, you don't need a main function.  You can just execute the code in the global scope.  Once the tree is defined, lines of text are computed by the function print_levels.  The next call simply joins all of the lines together with newline characters and prints them.  [AC???] At the moment, I'm not entirely sure why {0: root} is passed as a parameter.  A quick note about [AC???].  I use that notation if I don't know something, but want to note that I don't know something.  It's a great practice when reading other people's code to have some type of signature like that.
```python
lines = print_levels({0: root})
print '\n'.join(lines)
```

Basically, makenode constructed a binary tree without the use of comparisons.  Because this was hand-coded, the comparisons were done by the engineer.  The print_levels expects a binary tree (it doesn't have to be sorted).  It also expects to be able to modify it.  The binary tree in this case doesn't require a parent pointer, so the first action prior to printing would be to convert the binary search tree into a binary tree that is compatible with print_levels.  print_levels will modify the tree by adding '|' as nodes in cases of tree collisions.  To make things simpler, we can just make a copy of the binary search tree that is allowed to be modified by print_levels and not worry about setting the parent pointer.

The only time print_levels is called where lines==None is above for the top level or root of the tree.  This would mean that lines starts as an array and only returns when there are not any print_items.  This is a recursive function, so it is helpful to look for all instances of return to understand the nature of the recursion.
```python
def print_levels(print_items, lines=None):
    if lines is None:
        lines = []
    if not print_items:
        return lines

    ...
    return print_levels(next_items, lines)
```

A number of variables are setup as being zero or empty sets.  It seems that pos isn't really needed and that it can be entirely represented by position.
```python
    # working position - where we are in the line
    pos = 0

    # line of text containing node labels
    new_nodes_line = []

    # line of text containing slashes
    new_slashes_line = []

    # args for recursive call
    next_items = {}
```

The first time print_levels is called, it is called with {0: root}, so the key would be 0, and sorted_pos_and_node would be equal to [(0, root)].  Considering that this is the last time print_items is used in the function, maybe it doesn't need to be a dictionary???
```python
    # sort dictionary by key and put them in a list of pairs of (position,
    # node)
    sorted_pos_and_node = [
        (k, print_items[k]) for k in sorted(print_items.keys())]
```

Iterate over each element in the list.
```python
    for position, node in sorted_pos_and_node:
```

Each element in the list has an initial position.  Pad new_nodes_line and new_slashes_line with spaces until length is equal to position.
```python
        # add leading whitespace
        while len(new_nodes_line) < position:
            new_nodes_line.append(' ')
        while len(new_slashes_line) < position:
            new_slashes_line.append(' ')
```

Append the key (or the characters of the string label) to new_nodes_line.  pos is temporarily set to the position of the key or label in new_nodes_line.  I don't think that position really helps here.
```python
        # update working position
        pos = position
        # add node label to string, as separate characters so list length
        # matches string length
        new_nodes_line.extend(list(node.label))
```

The collision logic states that if there is a node in next_items that is within 3 spaces prior to the current position, then move the prior node down using a '|' as a node.  Add the '|' to the new_slashes_line and the node to the given position.  If there isn't a left node, just append a space.  Since the left node is represented by a vertical line, it is less complicated than the right node.
```
        # add left child if any
        if node.left is not None:
            # if we're close to overlapping another node, push that node down
            # by adding a parent with label '|' which will make it look like a
            # line dropping down
            for collision in [pos - i for i in range(3)]:
                if collision in next_items:
                    next_items[collision] = makenode(
                        '|', next_items[collision])

            # add the slash and the node to the appropriate places
            new_slashes_line.append('|')
            next_items[position] = node.left
        else:
            new_slashes_line.append(' ')
```

Add the length of the label (or key) to pos.  I think it can just be position.
```python
        # update working position
        len_num = len(node.label)
        pos += len_num
```

I will suggest that we change the lines above to
```python
        position += len(node.label)
```

The following code will change as well
```
        # add some more whitespace
        while len(new_slashes_line) < position + len_num:
            new_slashes_line.append(' ')

        # and take care of the right child
        if node.right is not None:
            new_slashes_line.append('\\')
            next_items[position + len_num + 1] = node.right
        else:
            new_slashes_line.append(' ')
```

To

```python
        # add some more whitespace
        while len(new_slashes_line) < position:
            new_slashes_line.append(' ')

        # and take care of the right child
        if node.right is not None:
            new_slashes_line.append('\\')
            next_items[position + 1] = node.right
        else:
            new_slashes_line.append(' ')
```

The new_slashes_line needs to be caught up with whitespaces to represent the label (or key) so that they match up.  If the right node exists, append a '\\' to new_slashes_line just after the label or key in new_nodes_line.  If the right label doesn't exist, then just append a space to new_slashes_line.  Append the two lines (the first line will have labels, the second will have left and right pointer markers).  next_items will represent all of the left and right nodes that existed from the current level.  If next_items is empty because there aren't any children at the given level, the recursive function will end and lines will be returned.

```python
    # concatenate each line's components and append them to the list
    lines.append(''.join(new_nodes_line))
    lines.append(''.join(new_slashes_line))

    # do it again!
    return print_levels(next_items, lines)
```

Because the left child is represented as a vertical bar '|', we can assume that the root and all of its left most children will be in the first column. The left child sits beneath the first character of the key.  The right children are referenced by a forward slash '\', but in some instances they can collide.  To avoid this, you can replace right children with a new key '|' and then link the right child to the left side of the '|'.  This has the effect of giving space for the collision as the right child that is relinked with a '|', left child combination will move straight down two lines.  You may have noticed before calling print_levels recursively, the lines are appended.  This is fine to continue with, but one could just print those lines (or add them to a list) and assign next_items in a loop to the current items.  In other words, this function doesn't have to be recursive.  

Sometimes when converting code from one language to another, it is helpful to first rewrite the code after reviewing it.  The primary purpose of rewriting the original code is to make it more translatable and not to necessarily optimize it.  

First, pos as a variable wasn't needed.

Upon reviewing the original code, it seemed to me that we could alter the type of new_nodes_line and new_slashes_line from an array to a string.  There are 3 places where a number of spaces are added to the string (or array).  I'll create a function called spaces which takes in the number of spaces to create and if it is > 0, returns that number of spaces.  Otherwise, it will return an empty string.  

Instead of code like this ...
```python
new_slashes_line = []

...

while len(new_slashes_line) < position + len_num:
    new_slashes_line.append(' ')
```

It could look like this
```python
new_slashes_line = ''

...

new_slashes_line += spaces((position + len_num)-len(new_slashes_line))
```

given a spaces function as described above
```python
def spaces(num_spaces):
    if num_spaces > 0:
        return ' ' * num_spaces
    return ''
```

In the original code, the author aligned the lengths of both rows by filling them with spaces.  There was never a case where the author undid an addition to new_slashes_line or new_nodes_line.  

The print_items and next_items were both treated as dictionaries and then sorted and converted to an array.  I think the items are inserted in sorted order.  If they were not, the lines would cross.  Considering this, print_items and next_items could be treated as sorted arrays.  The original implementation consisted of a position, node pair.  A new node was inserted into the list to replace nodes which need pushed down with only the left side filled.  Instead of inserting a new node with only the left side filled and a value of '|', we can convert the pair to a triple with the added parameter spacer which would default unless it was a spacer meant to push down a node.  In the case that the triple is a spacer, node would just point to the node it replaced.

The logic relating to the collision changes a bit if you use an array instead of a dictionary.  In python, you can use -1 as an index to get the last element, but that doesn't exist in C.  To make the code easier to convert, I will use positive indices.  The gist of the original code was to push down any element that was found to be within 3 spaces of the current position.  The items in the list only need to change.  Notice that the code isn't significantly more complex.

The old code...
```python
for collision in [pos - i for i in range(3)]:
    if collision in next_items:
        next_items[collision] = makenode(
            '|', next_items[collision])
```

The new code...

```python
pos = len(next_items)
while pos > 0:
    pos -= 1
    if next_items[pos][0] + 3 < position:
        break
    next_items[pos][2] = True
```

<b>A quick but important note about the following line...</b>
```python
if next_items[pos][0] + 3 < position:
```

This line can be seen as equivalent to
```python
if next_items[pos][0] < position - 3:
```

However, I tend to use addition if possible because it significantly reduces the changes of unsigned integers flipping to a high value.  0-1 = the largest number the type can hold.  In general, by keeping the + 3 version, you will be far less likely to have the integer flip to the high value and your code will work as expected.

Putting all of the changes together below results in the following code which is easier to map directly to C.
```python
def spaces(num_spaces):
    if num_spaces > 0:
        return ' ' * num_spaces
    return ''

# takes a root node as input and print the tree
def print_binary_search_tree(root):
    print_items = [ (0, root, False) ]
    while len(print_items):
        # line of text containing node labels
        new_nodes_line = ''

        # line of text containing slashes
        new_slashes_line = ''

        # for the next loop
        next_items = []

        for position, node, spacer in print_items:
            # add leading whitespace
            new_nodes_line += spaces(position-len(new_nodes_line))
            new_slashes_line += spaces(position-len(new_slashes_line))

            if spacer is False:
                new_nodes_line += node.label
            else:
                new_nodes_line += '|';

            if spacer is True or node.left is not None:
                left_node = spacer ? node : node.left
                pos = len(next_items)
                while pos > 0:
                    pos -= 1
                    if next_items[pos][0] + 3 < position:
                        break
                    next_items[pos][2] = True

                # add the slash and the node to the appropriate places
                new_slashes_line += '|'
                next_items.append((position, left_node, spacer))
            else:
                new_slashes_line += ' '

            new_slashes_line += spaces(len(new_nodes_line)-len(new_slashes_line))

            # and take care of the right child
            if spacer is False and node.right is not None:
                new_slashes_line += '\\'
                next_items.append((len(new_slashes_line), node.right, False))
            else:
                new_slashes_line += ' '

        print(new_nodes_line)
        print(new_slashes_line)
        print_items = next_items
```

Python has a string object which makes it easy to add strings together.  C doesn't provide this abstraction, but we can build it.  In the above code, there are two strings and an array.  A string is really just an array of bytes, so we could consider that we need to build an object to support arrays.  In the code above, the arrays (the strings or the actual array) are appended to (or they are growing).  The arrays are reset to being empty in each iteration of the loop.  An array is just like a string except that the items in the array may be larger than a single byte or character.  When you add to the string or array, the position of the whole array or string may be moved if there isn't enough memory reserved for the addition plus the original.  

The translated C is relatively straight forward and should look fairly similar to the python above.  It is far easier to write given the buffer object.

```c
typedef struct node_print_item_s {
  size_t position;
  node_t *node;
  bool spacer;
};

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
  while(buffer_length(print_items) > 0) {
    // line of text containing node labels
    buffer_clear(new_nodes_line);

    // line of text containing slashes
    buffer_clear(new_slashes_line);

    // for the next loop
    buffer_clear(next_items);
    node_print_item_t *items = (node_print_item_t *)buffer_data(print_items);
    size_t num_items = buffer_length(print_items) / sizeof(node_print_item_t);
    for( size_t i=0; i<num_items; i++ ) {
      // add leading whitespace
      buffer_appendn(new_nodes_line, ' ', items[i].position-buffer_length(new_nodes_line));
      buffer_appendn(new_slashes_line, ' ', items[i].position-buffer_length(new_slashes_line));

      if(!items[i].spacer)
        buffer_appendc(new_nodes_line, items[i].node->key);
      else
        buffer_appendc(new_nodes_line, '|');

      if(items[i].spacer || items[i].node->left) {
        node_t *left_node = items[i].spacer ? items[i].node : items[i].node->left;
        size_t num_next_items = buffer_length(next_items) / sizeof(node_print_item_t);
        node_print_item_t *_next_items = (node_print_item_t *)buffer_data(next_items);
        while(num_next_items > 0) {
          num_next_items--;
          if(_next_items[num_next_items].position+3 < items[i].position)
            break;
          _next_items[num_next_items].spacer = true;
        }

        buffer_appendc(new_slashes_line, '|');
        item.position = items[i].position;
        item.node = left_node;
        item.spacer = items[i].spacer;
        buffer_append(next_items, &item, sizeof(item));
      }
      else
        buffer_appendc(new_slashes_line, ' ');

      buffer_appendn(new_slashes_line, ' ', buffer_length(new_nodes_line)-buffer_length(new_slashes_line));

      if(!items[i].spacer && items[i].node->right) {
        buffer_appendc(new_slashes_line, '\\');
        item.position = buffer_length(new_slashes_line);
        item.node = items[i].node->right;
        item.spacer = false;
        buffer_append(next_items, &item, sizeof(item));        
      }
      else
        buffer_appendc(new_slashes_line, ' ');
    }
    printf( "%s\n", buffer_data(new_nodes_line));
    printf( "%s\n", buffer_data(new_slashes_line));
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
```

If we run make, you should see an output like the following.
```
$ make
Creating binary_search_tree for PDCBAEMLOQ
P
|\
D Q
|\  
C E
|  \
B   M
|   |\
A   L O

print_using_iteration: ABCDELMOPQ
print_using_reverse_iteration: QPOMLEDCBA
destroy_using_iteration: ABCLOMEDQP

# PDCBAEMLOQTRYZ
```

So far looking good, except what if you try running the program with PDCBAEMLOQTRYZ?  It will run forever.
```
$ ./test_data_structure PDCBAEMLOQTRYZ | head -n20
Creating binary_search_tree for PDCBAEMLOQTRYZ
P
|\
D Q
|\ \
C E T
|  \|\
B   |RY
|   |    \
A   |     Z
    |       
    |
```

This output is messed up.  It turns out that either I translated the code wrong or there might have been a bug in the code that I found on stack overflow.  Fixing the bug turned out to be fairly simple.  Basically, if you consider that the left line of nodes should be pushed down to prevent the string of nodes from branching towards the right and interfering, then you can imagine spacing downwards if trees to the right on the line have a height greater than 1.  To fix this, we will create a get_height method.

This code that follows is found in illustrations/linked_structures/4_binary_search_tree<br/>

```c
static int get_height(node_t *n) {
  if (!n)
    return 0;
  int left_height = get_height(n->left);
  int right_height = get_height(n->right);

  return (left_height > right_height) ? (left_height + 1) : (right_height + 1);
}
```

Once there is a get_height method, the following bit of code will get inserted at the beginning of the loop over the items.
```c
items[i].spacer = false;
for (size_t j = i + 1; j < num_items; j++) {
	if (!items[j].spacer && get_height(items[j].node) > 1) {
		items[i].spacer = true;
		break;
	}
}
```

With this logic in place, we can get rid of the collision logic that previously existed.
```c
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
```

One additional change that is useful is we will suffix the keys that are printed with the given node's height.
```c
static int get_depth(node_t *n) {
  int depth = 0;
  while (n) {
    depth++;
    n = n->parent;
  }
  return depth;
}
```
And

```c
if (!items[i].spacer)
	buffer_appendc(new_nodes_line, items[i].node->key);
```

Becomes

```c
if (!items[i].spacer) {
	buffer_appendf(new_nodes_line, "%c%d", items[i].node->key,
								 get_depth(items[i].node));
```

Now when we run <b>make</b>, we get the following output.
```
$ make
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
|     T3
|     | \
|     |  Y4
|     |    \
D2    R4    Z5
| \            
|  E3
|    \
|     M4
|     | \
C3    L5 O5
|           
B4
|  
A5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA
destroy_using_iteration: ABCLOMEDRZYTQP
```
