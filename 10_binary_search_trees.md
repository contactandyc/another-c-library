# Binary Search Trees

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

The code that follows can be found in illustrations/10_binary_search_trees/1_binary_search_tree

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

The last function to build is a method to print the tree.  It presented a different kind of challenge that programmers often face and so I'm separating it out.  In order to print the binary search tree, I've implemented an approach that requires copying the tree into a different structure.  Because we have built the stla_allocator and the stla_pool.  I've decided to go ahead and use them.  To demonstrate how they can be useful, I've commented out the free and stla_pool_destroy calls.  Before getting into how to print, let's review the changes to test_data_structure.c and data_structure.h.

data_structure.h includes "stla_pool.h"
```c
#include "stla_pool.h"
```

and node_print takes a pointer to stla_pool_t *pool
```c
void node_print(node_t *root);
```

becomes
```c
void node_print(stla_pool_t *pool, node_t *root);
```

The test_data_structure.c needs to incorporate the pool.

In the main function, the pool needs initialized and destroyed.  Also, the free call gets changed to stla_free.  I've commented the free and destroy call on purpose.
```c
stla_pool_t *pool = stla_pool_init(1024);
char *arg = get_valid_characters(argv[1]);
test_data_structure(pool, arg, repeat);
// stla_free(arg);
// stla_pool_destroy(pool);
```

get_valid_characters has a call to strdup which changes from
```c
char *res = strdup(p);
```

to
```c
char *res = stla_strdup(p);
```

The test_data_structure definition changes from
```c
void test_data_structure(const char *arg, int repeat) {
```

to
```c
void test_data_structure(stla_pool_t *pool, const char *arg, int repeat) {
```

All of the calls to node_print now take the pool parameter.
```c
node_print(root);
```

becomes
```c
node_print(pool, root);
```

The Makefile also includes the src library to incorporate the stla_pool object.

The following code is found in <i>illustrations/10_binary_search_trees/1_binary_search_tree</i>
```bash
cd $stla/illustrations/10_binary_search_trees/1_binary_search_tree
```

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

2527 byte(s) allocated in 17 allocations (680 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2152
test_data_structure.c:204: 15
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
binary_search_tree.c:14: 32
../../../src/stla_pool.c:121: 1040
```

Notice that just before the program ended, that it printed 17 lines where allocations happened.  These are allocations which were not properly freed.  Let's check out the lines around 14 of binary_search_tree.c.

```c
node_t *node_init(char key) {
  node_t *n = (node_t *)stla_malloc(sizeof(node_t));
  n->left = n->right = n->parent = NULL;
  n->key = key;
  return n;
}

void node_destroy(node_t *n) {
  // stla_free(n);
}
```

Notice that the stla_free is commented out.  Let's uncomment it and run make again.

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

2079 byte(s) allocated in 3 allocations (120 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2152
test_data_structure.c:204: 15
../../../src/stla_pool.c:121: 1040
```

There are three lines left.  Let's consider the second one (test_data_structure.c:204).  The lines around 204 of test_data_structure.c are below.
```c
char *get_valid_characters(const char *p) {
	char *res = stla_strdup(p);
	char *wp = res;
```

Line 204 is a stla_strdup call which ultimately gets returned from get_valid_characters.  If we look for the get_valid_characters call, we find it in the main function.  The arg is not freed (stla_free is commented out).  

```c
stla_pool_t *pool = stla_pool_init(1024);
char *arg = get_valid_characters(argv[1]);
test_data_structure(pool, arg, repeat);
// stla_free(arg);
// stla_pool_destroy(pool);
return 0;
```

Let's uncomment the stla_free(arg); line and run again.
```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

2064 byte(s) allocated in 2 allocations (80 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2152
../../../src/stla_pool.c:121: 1040
```

The test_data_structure program allwos us to print multiple binary search trees if we pass a third argument which is the number of random trees to print (in addition to the one that is in order at the end).

```c
$ ./test_data_structure PDCBAEMLOQTRYZ 2
Creating binary_search_tree for PDCBAEMLOQTRYZ
L1
| \
|  Q2
|  | \
|  |  R3
|  |    \
|  M3    T4
|    \     \
C2    O4    Z5
| \     \   |
|  D3    P5 Y6
|    \        
A3    E4
  \     
   B4

Q1
| \
|  R2
|    \
B2    Y3
| \   | \
A3 |  T4 Z4
   |       
   M3
   | \
   |  P4
   |  |
   |  O5
   |    
   C4
     \
      D5
        \
         E6
           \
            L7

P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA

4144 byte(s) allocated in 4 allocations (160 byte(s) overhead)
test_data_structure.c:244 [stla_pool] size: 3150, max_size: 3150, initial_size: 1024 used: 4232
../../../src/stla_pool.c:121: 1040
../../../src/stla_pool.c:121: 1040
../../../src/stla_pool.c:121: 1040
```

Notice that since we printed 3 trees, the total bytes allocated increased to 4140 bytes.  Let's review the test_data_structure function for a moment.

```c
void test_data_structure(stla_pool_t *pool, const char *arg, int repeat) {
  printf("Creating %s for %s\n", DATA_STRUCTURE, arg);
  node_t *root;
  for( int i=0; i<repeat; i++ ) {
    // stla_pool_clear(pool);
    root = fill_data_structure_randomly(arg);
    node_print(pool, root);
    find_everything(arg, root);
    find_and_erase_everything(arg, root);
  }
  // stla_pool_clear(pool);
  root = fill_data_structure(arg);
  node_print(pool, root);
  print_using_iteration(root);
  if (node_previous_supported())
    print_using_reverse_iteration(root);
  destroy_using_iteration(root);
  printf("\n");
}
```

For each call of node_print, the pool uses more memory.  However, we can clear the pool just after each print since the pool is only used for allocation within the node_print call.  More specifically, we can clear the pool at the beginning of the for loop to repeat building, printing, and destroying the tree, and just before we recreate the tree one last time.  If you uncomment the stla_pool_clear calls, you can go ahead and rebuild.

Rebuild
```bash
make
```

and run again
```bash
$ ./test_data_structure PDCBAEMLOQTRYZ 2
.
.
.
2064 byte(s) allocated in 2 allocations (80 byte(s) overhead)
test_data_structure.c:246 [stla_pool] size: 1050, max_size: 1050, initial_size: 1024 used: 2064
../../../src/stla_pool.c:121: 1040
```

You should see that the memory usage didn't grow this time.  If we uncomment the stla_pool_destroy call in the main function, we will see the following output.

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
P1
| \
|  Q2
|    \
D2    T3
| \   | \
C3 |  R4 Y4
|  |       \
B4 E3       Z5
|    \        
A5    M4
      | \
      L5 O5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA
```

There are no longer any memory leaks and the pool is being used efficiently.  It's worth noting that sometimes it is useful to comment out the destroy call to see how the pool is performing.

Before moving on, it is worth looking at how unbalanced binary search trees can become.  The simplest way to do it is to put all of the keys in sequence.

For example,
```bash
$ ./test_data_structure abcdefgh
Creating binary_search_tree for abcdefgh
a1
  \
   b2
     \
      c3
        \
         d4
           \
            e5
              \
               f6
                 \
                  g7
                    \
                     h8

print_using_iteration: abcdefgh
print_using_reverse_iteration: hgfedcba
```

or
```bash
$ ./test_data_structure hgfedcba
Creating binary_search_tree for hgfedcba
h1
|
g2
|
f3
|
e4
|
d5
|
c6
|
b7
|
a8

print_using_iteration: abcdefgh
print_using_reverse_iteration: hgfedcba
```

To really understand how to print a binary search tree, I've made a slight modification to the node printing function so that it can print the key a random number of times followed by the depth.

The code that follows can be found in illustrations/10_binary_search_trees/2_binary_search_tree

```bash
cd $stla/illustrations/10_binary_search_trees/2_binary_search_tree
```

```bash
$ make
gcc -g -O3 -I../../../src -D_STLA_DEBUG_MEMORY_=NULL ../../../src/stla_timer.c ../../../src/stla_allocator.c ../../../src/stla_buffer.c ../../../src/stla_pool.c test_data_structure.c binary_search_tree.c -o test_data_structure -DDATA_STRUCTURE=\"binary_search_tree\"
Creating binary_search_tree for PDCBAEMLOQTRYZ
PPPPPPPP1
|        \
DDDDD2    QQQQQ2
|     \         \
|      EEE3      T3
|          \     | \
CCCCCCCCC3  |    |  YYYYYYYYYYYYY4
|           |    |                \
BBBBBBBBB4  |    RRRRRR4           ZZZ5
|           |                          
|           MMMMMMMMMM4
|           |          \
|           LLLLLLLLL5  OOOOOOOOO5
|                                 
AAAAAAAAAAA5

print_using_iteration: ABCDELMOPQRTYZ
print_using_reverse_iteration: ZYTRQPOMLEDCBA
```

In the example above, AAAAAAAAAAA5 is pushed down until LLLLLLLLL5 is printed.  CCCCCCCCC3 is pushed down because it conflict with EEE3.  RRRRRR4 is pushed down because it would conflict with YYYYYYYYYYYYY4.

The way that I have designed the print function is to print the left nodes vertically below the first character of the key and the right nodes using a back slash spaced one character after the parent key.  If a node conflicts with the next node at the same level, that node is pushed down vertically until the conflict is resolved.  The algorithm may not be the most optimal solution, but it does seem to work.  Technically, we don't need a print function to be able to use a binary search tree, but they are nice to visualize what is happening.  In addition to this, the solution is interesting and may lead to other solutions.
