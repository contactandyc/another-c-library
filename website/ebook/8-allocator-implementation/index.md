---
path: "/8-allocator-implementation"
posttype: "tutorial"
title: "8. The Global Allocator Implementation"
---

Implementing the allocator is rather involved and uses much of what you've learned in the prior chapters. However, it should help you to understand how to code or to improve it should you desire.

- Doubly linked lists to track allocations.
- An object (which gets declared in the global space) to maintain and monitor the doubly linked list.
- A separate thread will monitor memory growth and write it to a file periodically.
- Mutexes and conditions are used to coordinate between the thread and the rest of the programs.
- Static functions are used to prevent functions from being exposed beyond the implementation file.
- An object constructor and destructor are used to initialize and destroy the global variable (new).
- Function callbacks are employed to allow other objects to implement their reporting.
- A structure is defined for objects to optionally use when debugging memory.

## Keywords used
Keyword  | Description  
--|--
\#include  | Copy/paste the contents of another file into the given file.
typedef  | Used to define one type as another.
struct  | Group one or more data types together.
const  |  Define a variable as constant (meaning it can't change).
char  | Data type consisting of one byte that is signed (-128 to 127).
int  | Data type consisting of 4 bytes that is signed (-2,147,483,648 to 2,147,483,647).
ssize\_t  | Signed data type where the number of bits equals the number of bits in CPU.
size\_t  | Unsigned data type where the number of bits equals the number of bits in CPU.
void  |  Data type that doesn't represent any bytes. Functions returning void do not return a value. A void pointer can't be dereferenced.
static  |  Defines a variable or function as only being accessible within the file that declares said function.  If it is declared in a header file and included, then the scope includes the file that includes the file.
if  | if(\<condition>) {} - If condition is true, execute block of code.
else if  | else if(\<condition>) {} - Used with if to create more than two mutually exclusive conditions. If condition is true, execute block of code.
else  |  else {} - Used with if/else if. Execute block of code if all other conditions evaluate to false.
while  |  while(\<condition>) {} - While condition is true, execute block of code.
return  | Used to return a value from a function.
NULL  | A special pointer value to indicate that the pointer is not pointing to anything.

## Symbols used
Symbol  | Description
--|--
{}  | Defines a block of code or contents of a struct.
->  | Deferences a struct member from a struct pointer: struct\_ptr->struct\_member.
.  | Reference a struct member from a struct variable: struct\_var.struct\_member.
+=  | Equivalent to x = x + N where N is on the right and x is on the left.
\>>=  | equivalent to x = x >> N where N is on the right and x is on the left.  Shifts the bits of x N times to the right.
!  | Negates a condition: !true == false and !false == true.
==  | Check for equality between two elements.
=  |  Assign value on the right to the variable on the left.
\*  | Used to define a data type as a pointer (ex. char *), and to dereference (get the value of) a pointer.
& | Used to get the address of a value or to get a pointer to a given value.   
()  | Used to define parameters of a function or the condition in an if, while, or for loop (and a few others).
//  | Single line comment.
/\* \*/  | Multi-line comment.
;  |  Marks the end of a line of code.

The full source code for ac\_allocator is found at <i>src/ac\_allocator.c</i>

The object starts by including the corresponding header file.
```c
#include "ac_allocator.h"
```

The other header files are included.  This object will create a thread and use mutexes and conditions, so it will need pthread.h.
```c
#include <pthread.h>
#include <string.h>
```

For each block of memory that is allocated, we will use a node of a doubly-linked list.  The structure of that node will be:
```c
typedef struct ac_allocator_node_s {
  const char *caller;
  ssize_t length;
  struct ac_allocator_node_s *next;
  struct ac_allocator_node_s *previous;
  ac_allocator_t *a;
} ac_allocator_node_t;
```

Notice that there isn't a pointer to the memory that the user allocated.  This memory follows just past the structure.

Caller references where the memory was allocated from.
```c
const char *caller;
```

Length is the number of bytes that the user requested.  It is a signed number because if it is negative, then the object being allocated begins with the **ac\_allocator\_dump\_t** structure.
```c
typedef void (*ac_dump_details_f)(FILE *out, const char *caller, void *p, size_t length);

typedef struct {
  ac_dump_details_f dump;
} ac_allocator_dump_t;
```

The dump structure consists of a function pointer **dump** which is of type **ac\_dump\_details\_f** defined in ac\_allocator.h.  You might wonder why I used a struct with a single member.  I reason that it allows the allocator to potentially alter the structure in the future and create minimal work for users of it.  It also is simple to cast the memory allocated to a ac\_allocator\_dump\_t type and then call the dump method.  This would only apply if the **bool custom** was set to true during the allocation of an object.  Sometimes it is useful to give extra meaning to variables to save space.  Considering that every node that is to be allocated will require the overhead of the structure and that length won't exceed 2^63 bytes, it makes sense to overload the length variable name.

The next and previous pointers are used to implement a doubly-linked list.
```c
struct ac_allocator_node_s *next;
struct ac_allocator_node_s *previous;
```

The **ac\_allocator\_t \*a** member is declared in ac\_allocator.h but not defined.
```c
struct ac_allocator_s;
typedef struct ac_allocator_s ac_allocator_t;
```

The **ac\_allocator\_t \*a** member isn't strictly necessary.  It is used by ac\_free and ac\_realloc to double-check that the memory that is about to be freed or reallocated was previously allocated.  It serves as a magic number that must exist just before the actual memory that the user used.

The typedef of **struct ac\_allocator\_s** to **ac\_allocator\_t** was defined in ac\_allocator.h, so all that remains is to define ac\_allocator\_s for use within the ac\_allocator.c file.
```c
struct ac_allocator_s {
  ac_allocator_node_t *head;
  ac_allocator_node_t *tail;
  size_t total_bytes_allocated;
  size_t total_allocations;
  const char *logfile;
  bool thread_safe;
  pthread_t thread;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  int done;
};
```

The allocator uses a doubly-linked list.  A link to the head and tail is maintained to allow new objects to appended to the end efficiently.
```c
ac_allocator_node_t *head;
ac_allocator_node_t *tail;
```

The totals are what is currently allocated.  This is informational and reported when there are memory leaks.
```c
size_t total_bytes_allocated;
size_t total_allocations;
```

The allocator allows the memory leaks to be written to a logfile.  If this is NULL, stderr will be used.  If there is a logfile specified, it is not NULL, and thread\_safe is true; a monitoring thread will be started, which will periodically write out how many allocations are currently active.
```c
const char *logfile;
```

The allocator can be initialized to be thread-safe (or not).  If it is not thread-safe, then a monitoring thread will not be started even if logfile is specified.
```c
bool thread_safe;
```

If the monitoring thread is started, a thread will maintain a reference.
```c
pthread_t thread;
```

If the monitoring thread is started, then cond, mutex, and done are used to indicate that the thread should finish.  The monitoring thread will write out the allocations periodically and then go into a condition timed wait.  In other words, after writing out the allocations, it will wait for a period of time before writing out the allocations again.  If, during that time, a condition triggers, it will check if the thread should finish.  If it is done, it will write out the allocations one last time and exit the thread.
```c
pthread_cond_t cond;
pthread_mutex_t mutex;
int done;
```

### To Be Continued

[Table of Contents (only if viewing on Github)](../../../README.md)
