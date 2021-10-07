---
path: "/2-timing"
posttype: "tutorial"
title: "2. Timing Your Code (First Project)"
---

## A brief introduction to C

In the project, there is an illustrations folder that contains most of the code.  There is also a src directory where the final code exists.  Normally, one would start with a hello world project. That exists later in a section called Hello Buffer.  If you are lost, hopefully, it will make more sense once you get to the Hello Buffer section. I would recommend reading and working through the examples in this chapter and then coming back after working through Hello Buffer.  The code for this chapter is located in <i>illustrations/2\_timing</i>

At various points in this project, we will be timing code in an attempt to optimize it.  Our first object is going to be simple but will illustrate how I plan to maintain separation between interfaces and their respective implementation.

The following code is found in <i>illustrations/2\_timing/1\_timer</i>
```
cd $ac/illustrations/2_timing/1_timer
```

```
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 49.0980ns
Reverse => esreveR
time_spent: 23.1360ns
overall time_spent: 72.2340ns
```

test_timer.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void reverse_string( char *s ) {
  size_t len = strlen(s);
  char *e = s+len-1;
  while(s < e) {
    char tmp = *s;
    *s = *e;
    *e = tmp;
    s++;
    e--;
  }
}

long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  long overall_time = 0;
  for( int i=1; i<argc; i++ ) {
    char *s = (char *)malloc(strlen(argv[i])+1);
    long test_t1 = get_time();
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s);
    }
    long test_t2 = get_time();
    overall_time += (test_t2-test_t1);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", (test_t2-test_t1)*1000.0/(repeat_test*1.0));

    free(s);
  }
  printf( "overall time_spent: %0.4fns\n", overall_time*1000.0/(repeat_test*1.0));
  return 0;
}
```

Every C program that is capable of being executed has a main function.  The main function typically has the following two parameters to pass in command-line arguments to the program. `argc` represents the number of arguments. The name of the program is the 1st argument. `argv` represents the arguments. `argv[0]` references the name of the program as called from the command line. The main function returns an integer. Successful execution should return 0, which lets the command shell know that the program finished normally.

```c
int main( int argc, char *argv[] ) {
  ...
  return 0;
}
```

C has a few built-in data types:

| Data Type | Description |
|---|---|
| char | a single byte |
| short | two bytes |
| int | four bytes |
| long | typically eight bytes |
| size_t | on a 64-bit system, 8 bytes (or 64 bits), on a 32-bit system, 4 bytes (or 32 bits) |
| ssize_t | signed size_t |
| bool | can be different sizes, but only has two states (true and false) |
| float | four-byte decimal (I try to avoid this type as it quickly loses precision) |
| double | eight-byte decimal |
| void | this does not have a size and is a special type |

A byte represents 8 bits.  The range of 1 bit would be 0-1, two bits 0-3, etc. For 8 bits, the range is 0-255.  The short has a range of 0-((256*256)-1) or 0-65535.  C counts from 0 (all bits off) instead of 1.  Types can be signed (the default) or unsigned.  If the data type is unsigned, the number range will start with zero.  Otherwise, the number range will be between -(2^(number of bits-1)) and (2^(number of bits-1))-1.  A signed char will range from -128 to 127.  A signed char and a char are the same things.

`size_t` is a type that is defined in C to represent the number of bits that the CPU is and is unsigned (meaning it cannot be negative). `ssize_t` is a signed alternative.  On a 64-bit system, a long and `ssize_t` are equivalent.

You can overflow a type.

```c
unsigned char a = 255;
a = a + 1;  // a will become zero
a = a - 1;  // a will become 255 again
char b = 127;
b = b + 1; // b will become -128
b = b - 1; // b will become 127 again
```

`bool` is another type defined in C (you must include `stdbool.h` to get it). `bool` is defined as having the value true or false. This type can make code more readable in that it makes it clear to the reader that there are only two possible states.

`double` (and `float`) are used for decimals.  Because `float` only uses 32 bits, it loses precision quickly.  I do not tend to use it for anything.  Instead, I opt to use `double`.

`typedef` can be utilized to define your own type using the following syntax:

```
typedef <existing type> <new type name>;
```

For example, the following would define a new type called number_t, which has an underlying type of unsigned int.
```c
typedef unsigned int number_t;

number_t a = 100;
```

Every data type in C has a size.  The size of a pointer is always the same (it is the same as the size\_t type). The sizeof() operator determines the size of a type or variable. sizeof(number\_t) finds the size of the number\_t type (and returns four since unsigned int is four bytes). sizeof(void) is not allowed as that does not make sense.

A variable can be cast from one type to another, either implicitly or explicitly.  Imagine you want to convert an int to a double or vice versa.

```c
int x = 100;
double y = x;
y = y + 0.05;
x = y;
printf( "%d\n", x ); // would print 100
```

When the casting happens, precision is lost if the new type cannot accommodate the value.  The above example shows casting happening implicitly.  Below is an example of casting happening explicitly.

```c
int x = 100;
double y = (double)x; // explicit cast
y = y + 0.05;
x = y;
printf( "%d\n", x ); // would print 100
```

Variables are typically declared as
```c
datatype variable_name = value;
```

or
```c
datatype variable_name;
```

In the main function declaration, `argv` is declared uniquely.

```c
char *argv[];
```

The asterisk means that the variable named `argv` is a pointer to the type `char`, which is a single byte and is signed (signed is the default datatype prefix).  The `[]` after the `argv` indicates that `argv` is referencing an array of pointers.

In the following array
```
0123456789012345678901234567
./test_timer This is a test
```

`argv` would be an array of pointers<br/>
`argv[0] => .` (at byte 0)<br/>
`argv[1] => T` (at byte 13)<br/>
`argv[2] => i` (at byte 18)<br/>
`argv[3] => a` (at byte 21)<br/>
`argv[4] => t` (at byte 23)<br/>
`argv[5] => NULL` (pointing to NULL means that the pointer is not pointing at anything)<br/>

Notice that the pointers refer to individual bytes.  In C (and any language), a pointer refers to a location in memory (typically RAM or random access memory).  Strings or sequences of characters are defined by looking for a terminating character (a non-printable 0).  The above example is not technically correct in that what would happen is before the arguments getting to the main function; they would split into five strings.

`argv` would be an array of pointers<br/>
`argv[0] => "./test_timer"`<br/>
`argv[1] => "This"`<br/>
`argv[2] => "is"`<br/>
`argv[3] => "a"`<br/>
`argv[4] => "test"`<br/>
`argv[5] => NULL` (pointing to NULL means that the pointer is not pointing at anything)<br/>

The string "This" above is 4 bytes long but requires 5 bytes to be represented.  To determine the length of a string in C, you would advance a pointer until the zero is referenced.  Simple implementation to find the length of a string might look like:

```c
int strlen(char *s) {
  char *p = s;
  while(*p != 0)
    p++;
  return p-s;
}
```

This function returns an integer (so it will only support finding the length of strings that are 2,147,483,647 or fewer bytes).  The function's name is `strlen`.  It takes as an input a single pointer to a byte in RAM.  The pointer is of type char.  Imagine that s pointed to the first character in the string "This" (or the letter T).  This function seeks to find a pointer to the byte that is just past the end of the string (or just after the letter s) and then subtracts the two pointers.

```
This
^   ^
|   |
s   p
```

The value of subtracting s from p is 4.  

The steps to finding the length of a string in C are
```
1. given a pointer to a character, make a copy of the pointer.
2. while the pointer does not point to the non-printable zero, increment the pointer by 1.
3. return the difference between p and s.
```

Pointers always reference specific bytes in RAM.  In most languages, the pointer is defined to reference a type.  The appropriate number of bytes would be advanced depending on whether the pointer is incremented, subtracted, added, etc. A `char *` is a pointer that references a sequence of bytes that are of type `char`.  The `char` is a single byte, so advancing a pointer to a char by one is the same as incrementing the pointer by 1 byte.

Make a copy of the pointer s and call it p.
```c
char *p = s;
```


```c
while(*p != 0)
  p++;
```

C allows for repeated logic until a given condition evaluates true.  For example, a while loop is technically defined as

```c
while(<condition>)
  do_something;
```

Alternatively, if there are multiple lines of code which need to be executed within the loop:

```c
while(<condition>) {
  do_something1;
  do_something2;
  ...
  do_somethingN;
}
```

It is essential to recognize the difference between comparisons and conditions.  A condition can be true or false.  Comparisons require two objects and can evaluate to true or false.  In most (every?) computer language, loops, and if logic will use conditions as opposed to comparisons.  Comparisons are a subset of conditions.  In C, true is non-zero, and false is zero.

The following would never do_something:

```c
while(0) {
  do_something;
}
```

The following would never stop executing do_something:

```c
while(1) {
  do_something;
}
```

Back to the original example,
```c
while(*p != 0)
  p++;
```

Pointers are declared using an asterisk. Pointers point at values obtained through a process called dereferencing. Dereferencing occurs when an asterisk precedes the pointer. The first time through this loop, assuming that s pointed to "Test", p would be pointing at a value T.  *p would result in the single character 'T'.  In C, single quotes define single characters.  Double-quotes define longer strings.

```c
while(*p != 0) /* first time *p == 'T', condition is true ('T' != 0) */
  p++; /* advance p to point to the next character 'h' */

while(*p != 0) /* second time *p == 'h', condition is true ('h' != 0) */
  p++; /* advance p to point to the next character 'i' */

while(*p != 0) /* third time *p == 'i', condition is true ('i' != 0) */
  p++; /* advance p to point to the next character 's' */

while(*p != 0) /* fourth time *p == 's', condition is true ('s' != 0) */
  p++; /* advance p to point to the next character (non printable zero) */

while(*p != 0) /* fifth time *p == 0, so condition is no longer true */
```

At this point, p points to just past the letter s and has advanced four times, so the length is four.

It is common for developers to use `int` as a return type. However, it is generally more efficient and less bug-prone to use `size_t`.  For a 64 bit CPU to work with an `int`, it must split a register since the CPU is meant to work with 64-bit integers.  This split is not cheap.  Additionally, if you use `size_t` or (`ssize_t` for signed numbers), the program will be more portable to 64-bit systems where a string might be longer than 2 billion bytes.  A better implementation of `strlen` might look like the following.  The only difference is that the `strlen` returns a type `size_t`.  Strings cannot be negative in length, so returning an unsigned number also helps people using the function to understand that.

```c
size_t strlen(char *s) {
  char *p = s;
  while(*p != 0)
    p++;
  return p-s;
}
```

C allows for variables to be declared as constant, meaning that they cannot change.  Constants are particularly useful in functions because the function can indicate that the input will not change.  Above, the string remains unmodified while determining the length.  Adding `const` will indicate to the user (and compiler) that what s points to will not be changed.

```c
size_t strlen(const char *s) {
  const char *p = s; // p must also be declared with const
  while(*p != 0)
    p++;
  return p-s;
}
```

Another minor optimization to the function above is to look at the while loop. The while loop expects a condition.

```c
while(*p != 0)
```

can be changed to:

```c
while(*p)
```

because the check for 0 is redundant.  It doesn't hurt anything to add the != 0, and sometimes it makes code easier to read.

### The void type

A function with a return type of void means that the function does not expect to return anything. An example might be:

```c
void print_hello(const char *name) {
  printf("Hello %s\n", name);
}
```

This function prints something to the screen and does not return anything.

A void pointer: `void *`, is a special type of pointer that must be cast to another type before implementation.

Before describing the void pointer, I want to show an example of casting pointers.

The following would produce a warning:

```c
size_t strlen(const char *s) {
  char *p = s;
  ...
```

```
warning: initializing 'char *' with an expression of type 'const char *' discards qualifiers
```

To avoid such a warning, either change the type of p to `const char *` or cast s to `char *`
```c
char *p = (char *)s;
```

Imagine you have a function that will print the value of different types of variables, such as the following.

Code found in <i>illustrations/2_timing/1\_timer</i>

void_pointers.c:
```c
#include <stdio.h>

void print_value_of_pointer(char type, void *p) {
  if(type == 'c') {
    char *vp = (char *)p;
    printf( "char type: %d (%c)\n", *vp, *vp );
  }
  else if(type == 'C') {
    unsigned char *vp = (unsigned char *)p;
    printf( "unsigned char type: %u\n", *vp );
  }
  else if(type == 's') {
    short *vp = (short *)p;
    printf( "short type: %d\n", *vp );
  }
  else if(type == 'S') {
    unsigned short *vp = (unsigned short *)p;
    printf( "unsigned short type: %u\n", *vp );
  }
  else if(type == 'i') {
    int *vp = (int *)p;
    printf( "int type: %d\n", *vp);
  }
  else if(type == 'I') {
    unsigned int *vp = (unsigned int *)p;
    printf( "unsigned int type: %u\n", *vp );
  }
  else if(type == 'l') {
    long *vp = (long *)p;
    printf( "long type: %ld\n", *vp);
  }
  else if(type == 'L') {
    unsigned long *vp = (unsigned long *)p;
    printf( "unsigned long type: %lu\n", *vp );
  }
  else if(type == 'd') {
    double *vp = (double *)p;
    printf( "double type (with 4 decimal places): %0.4f\n", *vp );
  }
}

int main( int argc, char *argv[]) {
  char a = '0';
  print_value_of_pointer('c', &a); // notice that printable 0 is actually 48
  unsigned char b = 150;
  print_value_of_pointer('C', &b);
  short c = -5000;
  print_value_of_pointer('s', &c);
  unsigned short d = 5000;
  print_value_of_pointer('S', &d);
  int e = -500000;
  print_value_of_pointer('i', &e);
  unsigned int f = 500000;
  print_value_of_pointer('I', &f);
  long g = -500000;
  print_value_of_pointer('l', &g);
  unsigned long h = 500000;
  print_value_of_pointer('L', &h);
  double i = 1.5;
  print_value_of_pointer('d', &i);
  return 0;
}
```

If you build the program using
```
gcc void_pointers.c -o void_pointers -Wall
```

and run it
```
./void_pointers
```

You should see the following output.
```
char type: 48 (0)
unsigned char type: 150
short type: -5000
unsigned short type: 5000
int type: -500000
unsigned int type: 500000
long type: -500000
unsigned long type: 500000
double type (with four decimal places): 1.5000
```

In the program above, the void pointer cannot be used directly.  It must be converted to a different pointer type before the value that the pointer is pointing to can be referenced.  

The program above introduced `if`, `else if`, and the `&` operator.  If statements have similar syntax as while statements.  

```
if(<condition>) {
  zero or more statements;
}
else if(<condition>) {
  zero or more statements;  
}
else {
  zero or more statements;
}
```

The curly braces are not needed if there is exactly one statement. It is also essential to realize the difference between = and ==.  A single equal statement is used for assignment.  A double equal statement indicates an equality test.

The `if` statement works in conjunction with all the `else if` statements that immediately follow it.  `else if` statements are only reached and evaluated if the `if` or `else if` statements above it have not evaluated to a true condition.  Finally, `else` does not expect a condition and essentially becomes the default block to run if all other conditions were evaluated to be false.  This type of logic is known as mutual exclusivity, meaning that only one block of code is executed per pass.

In some cases, conditional, mutual exclusivity may also be achieved in a slightly optimized form by using what is known as a `switch`.  The syntax of a `switch` is demonstrated below by converting the `if/else if` logic from the `print_value_of_pointer` function

```c
...
switch(type) {
  case 'c':
    char *vp = (char *)p;
    printf( "char type: %d (%c)\n", *vp, *vp );
    break;
  case 'C':
    unsigned char *vp = (unsigned char *)p;
    printf( "unsigned char type: %u\n", *vp );
    break;
  case 's':
    short *vp = (short *)p;
    printf( "short type: %d\n", *vp );
    break;
  case 'S':
    unsigned short *vp = (unsigned short *)p;
    printf( "unsigned short type: %u\n", *vp );
    break;
  case 'i':
    int *vp = (int *)p;
    printf( "int type: %d\n", *vp);
    break;
  case 'I':
    unsigned int *vp = (unsigned int *)p;
    printf( "unsigned int type: %u\n", *vp );
    break;
  case 'l':
    long *vp = (long *)p;
    printf( "long type: %ld\n", *vp);
    break;
  case 'L':
    unsigned long *vp = (unsigned long *)p;
    printf( "unsigned long type: %lu\n", *vp );
    break;
  case 'd':
    double *vp = (double *)p;
    printf( "double type (with 4 decimal places): %0.4f\n", *vp );
    break; // optional because this is the last case and there is no default
}
...
```

The logic of a `switch` is less extensible than `if/else if/else` statements.  A `switch` may only evaluate for equality against constants on one value, but there are some advantages.  During compilation, the cases of a `switch` statement are mapped to a table which allows for immediate branching to the block of code associated with the true `case`.  As an example, consider a scenario where `type = 'L'`.  With the `if/else if` implementation of this logic, every condition up to and including `type == 'L'` must be evaluated before the appropriate block of code is executed.  In the case of using a `switch`, the `case 'L':` is immediately jumped to and the associated block of code is executed without having to first evaluate if `type` is equal to 'c', 'C', 's', 'S', etc.

The generalized form of a switch statement is as follows

```
switch(<value>) {
  case <constant value>:
    zero or more statements;
    break;
  case <constant value>:
    zero or more statements;
    break;
  ...
  default:
    zero or more statements;
}
```

If a `case` evaluates to true and there is no `break` statement within the `case` block, 'fall-through' occurs and each subsequent `case` is also executed until a `break` statement is encountered. The `default` case is optional, acts as a catch-all if none of the previous cases are true, and is very similar to a trailing `else` statement. The `default` case can also be 'fallen-into' if a `break` statement is not encountered.

Back to void pointers.

In code below, p is of type `void *`.  It must be converted before it can be dereferenced (to get the value of what p is pointing at).

```c
unsigned char *vp = (unsigned char *)p;
printf( "unsigned char type: %u\n", *vp );
```

The code above can be shortened to:
```c
printf( "unsigned char type: %u\n", *(unsigned char *)p );
```

You will find the above shortening often.  I used the two-line version to make what was happening clear.
Placing an asterisk before a pointer results in getting the value of what the pointer is pointing at.  Placing an ampersand before a value results in a pointer to the value.  For example:

```c
double a = 1.5;
double *p = &a;  // p points to the value 1.5
```

## What happens during compilation

In C, code converts to binary (which is generally very hard for humans to read).  C does not use line separation to separate code (except for compiler directives (lines that start with #)).  In C, there are two types of comments.  /* */ can be multiline comments.  // comments run to the end of the line.  The first thing a compiler does is remove comments.


```c
#include <stdio.h>
#include <string.h>

/* This function returns the length of a string */
size_t strlen(const char *s) {
  const char *p = s; // p must also be declared with const
  while(*p != 0)
    p++;
  return p-s;
}

int main(int argc, char *argv[]) {
  printf("Hello World (%ld)\n", strlen("Hello World"));
  return 0;
}
```

becomes

```c
#include <stdio.h>
#include <string.h>

size_t strlen(const char *s) {
  const char *p = s;
  while(*p != 0)
    p++;
  return p-s;
}

int main(int argc, char *argv[]) {
  printf("Hello World (%ld)\n", strlen("Hello World"));
  return 0;
}
```

Next, it will look for each individual statement, function declarations, loops, etc.  We can remove all of the spacing of the function above.
```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){const char *p=s;while(*p!=0)p++;return p-s;}int main(int argc, char *argv[]){printf("Hello World (%ld)\n",strlen("Hello World"));return 0;}
```

If you look at the above code, you should be able to see that functions follow the syntax of
```
<return type> <function name>([arguments]){[statements]}
```

Semicolons form natural breaks.  If the while loop only has one statement, it does not need curly braces.  Lets slowly introduce space back into the code above.

```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){const char *p=s;while(*p!=0)p++;return p-s;}int main(int argc, char *argv[]){printf("Hello World (%ld)\n",strlen("Hello World"));return 0;}
```

Introduce new lines after the opening and closing braces of functions.
```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){
const char *p=s;while(*p!=0)p++;return p-s;
}
int main(int argc, char *argv[]){
printf("Hello World (%ld)\n",strlen("Hello World"));return 0;
}
```

Indent code within the braces by two spaces
```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){
  const char *p=s;while(*p!=0)p++;return p-s;
}
int main(int argc, char *argv[]){
  printf("Hello World (%ld)\n",strlen("Hello World"));return 0;
}
```

Place a new line after each semicolon.
```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){
  const char *p=s;
  while(*p!=0)p++;
  return p-s;
}
int main(int argc, char *argv[]){
  printf("Hello World (%ld)\n",strlen("Hello World"));
  return 0;
}
```

Place a new line after the while statement
```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){
  const char *p=s;
  while(*p!=0)
  p++;
  return p-s;
}
int main(int argc, char *argv[]){
  printf("Hello World (%ld)\n",strlen("Hello World"));
  return 0;
}
```

Indent the line following the while statement to make it clear that that line belongs to the loop.
```c
#include <stdio.h>
#include <string.h>
size_t strlen(const char *s){
  const char *p=s;
  while(*p!=0)
    p++;
  return p-s;
}
int main(int argc, char *argv[]){
  printf("Hello World (%ld)\n",strlen("Hello World"));
  return 0;
}
```

Introduce extra spacing to separate the functions and the include statements.
```c
#include <stdio.h>
#include <string.h>

size_t strlen(const char *s){
  const char *p=s;
  while(*p != 0)
    p++;
  return p-s;
}

int main(int argc, char *argv[]){
  printf("Hello World (%ld)\n", strlen("Hello World"));
  return 0;
}
```

The syntax is necessary for the C compiler to understand where one statement ends and the next starts.  All of the above examples compile and will work the same.  The last example is easier to read.  C does not force you to make the code easy to read.  It is just an excellent idea to do so!


There are different types of loops in C.  The following defines the `for` loop:

```c
for( [initialization_code]; [condition]; [post_loop code] ) {

}
```

There are three components of the `for` loop: the initialization_code, condition, and post_loop code.  Notice the enclosure in square brackets `[]` instead of lesser/greater than signs `<>`.  Square brackets indicate the bit of code is optional.

If there is no initialization_code, condition, or post_loop code, the for loop will continue forever.  The following example would print the string "Hello World!" followed by a new line character continuously forever (or until the program was stopped).
```c
for(;;) {
  printf( "Hello World!\n" );
}
```

This code will loop through the arguments skipping the name of the program (as it is the first argument in the argv array).
```c
for( int i=1; i<argc; i++ ) { }
```

## How to time code

The example is going to reverse strings several times while timing the process.  The program should not modify the arguments that are passed into your program.  If you wish to modify an argument, you should first allocate memory for your program to use and then copy the argument into the newly allocated memory.  The `malloc` function will allocate the number of bytes requested for use.  You can read about it by running the following command.

```
$ man malloc
``` 

Programs that require extra memory to work with must request that memory from the operating system.  `malloc` is one of the core ways to complete this.  Memory requested should later be freed using the `free` call.  The `malloc` function can return `NULL`, meaning that the memory was not available.  If a pointer is pointing at `NULL`, any attempts to access what it is pointing at will cause your program to crash.  You can check for the error, or just allow the program to not so gracefully crash.  In my examples, I am going to allow the program to crash if `NULL` is returned.  The only other reasonable option would be to have the program fail early, which effectively is the same thing.  In the example below, there are a few functions called.  The early writers of C decided to shorten the names of the functions.

```
malloc - memory allocate
strlen - string length
strcpy - string copy
```

In the following example, on each iteration of the loop, `s` is pointed at newly allocated memory which is the string length of `argv[i]` plus one to make room for the zero terminator. Next, a string copy of the argument is performed so that `s` points to a copy of the given argument.

```c
for( int i=1; i<argc; i++ ) {
  char *s = (char *)malloc(strlen(argv[i])+1);
  ...
  strcpy(s, argv[i]);
  ...
  free(s);
```

At the beginning of the program, there were a few `#include` statements.  `#include` effectively copies the contents of the specified filename into the current program.  To use functions like `malloc`, `strcpy`, and `free`, the proper file must be included.  Files with a suffix ".h" are called header files and typically define how to call a function.  

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
```

I include the include statements above because of the following function calls:

| Function | Description |
|---|---|
| stdio.h | printf, NULL, and many other io related functions |
| stdlib.h | malloc, free, and many other standard library-related functions |
| string.h | strlen, strcpy, and many other string related functions |
| sys/time.h | gettimeofday |
| time.h | sometimes an alternate location of gettimeofday |

This program is going to time how long the reverse string function takes to run.  To get the amount of time that a process takes to complete, one might get a start time and an end time then subtract the start time from the end time.  The following function will get the time in microseconds (millionths of seconds).  Passing NULL to `gettimeofday` will cause `gettimeofday` to return the current time.  The `timeval` structure consists of two members, the number of seconds and the number of microseconds.  

```c
long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}
```

A quick note on using parenthesis:  I strongly believe in making code easier to read.  Technically, I could have skipped the parenthesis without harm due to the order of operations, but the code becomes simpler to read by adding the parenthesis.

Reversing a string takes a minimal amount of time.  It is so small that to accurately measure it, you need to repeat the test a million times to yield anything approaching an accurate average.  In each loop, the `strcpy` resets `s` such that it has a copy of the ith argument so that it can be reversed.  The time that the process takes is `test_t2 - test_t1`.

```c
int repeat_test = 1000000;
...
long test_t1 = get_time();
for( int j=0; j<repeat_test; j++ ) {
  strcpy(s, argv[i]);
  reverse_string(s);
}
long test_t2 = get_time();
long time_spent = test_t2-test_t1;
overall_time += time_spent;
```

After timing the `reverse_string call`, `printf` is used to print the string on the terminal before it was reversed and the new form of the string (s).  `printf` allows for format specifiers to match arguments after the first parameter (also known as the format string).  `%s` indicates that there must be a string for the given argument.  `%0.4f` expects a floating-point number and prints four decimal places.  `test_t2` and `test_t1` are both measured in microseconds.  Multiplying the difference by 1000 will change the unit type to nanoseconds.  Since the test was repeated 1 million times, the overall time needs to be divided by 1 million.  By multiplying or dividing a number by a decimal, it converts the evaluated type to a decimal.

```c
    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", ((test_t2-test_t1)*1000.0)/(repeat_test*1.0));
    ...

  printf( "overall time_spent: %0.4fns\n", overall_time*1000.0/(repeat_test*1.0));
```

## Reversing a string

The last function to examine is the `reverse_string` call.

```c
void reverse_string( char *s ) {
  size_t len = strlen(s);
  char *e = s+len-1;
  while(s < e) {
    char tmp = *s;
    *s = *e;
    *e = tmp;
    s++;
    e--;
  }
}
```

The above code works in the following way.

```
1.  Set a pointer e to the last character in the string.  The last character
    is found by determining the length of the string s and pointing to the
    length-1 char beyond s.
2.  while s is less than e
      swap the value that is pointed to by s and e.
      advance s by one and decrement e by one.
```

Imagine the string "Reverse".  The `strlen` or length of "Reverse" is 7. The difference between the pointer referencing the null terminator and the pointer that points to the beginning of the string is 7.

```
01234567
Reverse
^      ^
s      e
```

To reverse the string, we need the end pointer to point to the last character 'e', e.g. subtract one byte.

```
01234567
Reverse
^     ^
s     e
```

The first step is to swap R and e.  In order to swap R and e, a temporary variable is needed.

```c
char tmp = *s;  // *s == R
*s = *e;        // *e == e, so now string is equal to eeverse
*e = tmp;       // tmp = R, so now string is equal to eeversR
s++;            // s points to the second e
e--;            // e points to s
```

The while loop will continue because `s` is less than `e`.

```
01234567
eeversR
 ^   ^
 s   e
```

```c
char tmp = *s;  // *s == e
*s = *e;        // *e == s, so now string is equal to esverse
*e = tmp;       // tmp = e, so now string is equal to esvereR
s++;            // s points to v
e--;            // e points to r
```

The while loop will continue because `s` is less than `e`.

```
01234567
esvereR
  ^ ^
  s e
```

```c
char tmp = *s;  // *s == v
*s = *e;        // *e == r, so now string is equal to esrereR
*e = tmp;       // tmp = e, so now string is equal to esreveR
s++;            // s points to the middle e
e--;            // e points to the middle e
```

The while loop will NOT continue because `s` is not less than `e`.

```
01234567
esvereR
   ^
   s
   e
```

The string is now esreveR, which is the reverse of Reverse.

## The basic Makefile

In the <i>illustrations/2_timing/1\_timer</i> directory, you will find the following Makefile.  

```Makefile
all: test_timer examples

test_timer: test_timer.c
	gcc test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
```

When you run

```
make
```

The first block with a colon will run.  In this Makefile, it is the following line:
```Makefile
all: test_timer examples
```

The all group refers to the other groups to be built.  In this case, it is test_timer and examples.

```Makefile
test_timer: test_timer.c
	gcc test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
```

The lines of code after test\_timer will run if the file called test\_timer is older than the files after the colon.  If you edit test\_timer.c, the program will be built using the following line of code.  The output is test\_timer.
```Makefile
gcc test_timer.c -o test_timer
```

If you do not want to use a Makefile to build test_timer, you could do so from the command line using the following command.

```
$ gcc test_timer.c -o test_timer
```

The examples block will run every time because it does not have any dependencies, and examples is not a file that exists.  If you were to create a file called examples, then the examples block would cease to run.  By running `make`, you will effectively build test_timer if it needs to build and run the examples block.  Running  `make clean` will clean up the binary file.  You can run any block by specifying it. `make all` is equivalent to running `make`.  If you just want to run the examples block, you can by running `make examples`.  

## More accurately timing code

In the last section, we explored how to time the reverse\_string function.  In this section, we will explore how to time the function better.  One thing you may have noticed is that there are a million calls to both `reverse_string` and `strcpy`.  There is also the overhead of the loop.  To do the timing correctly, we should have timed the `strcpy` and the loop then subtracted that from the loop that has the reverse\_string function called.

The timing for the work in <i>illustrations/2\_timing/1\_timer</i> was:

```
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 46.4650ns
Reverse => esreveR
time_spent: 20.5820ns
overall time_spent: 67.0470ns
```

This section's code is found in <i>illustrations/2\_timing/2\_timer</i>

```
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 35.8950ns
Reverse => esreveR
time_spent: 13.7250ns
overall time_spent: 49.6200ns
```

Running `make` yields a 17.5 nanosecond improvement.  The following test demonstrates the difference between 1_timer and 2_timer:

```
$ diff test_timer.c ../1_timer/test_timer.c
30,35d29
<     long copy_t1 = get_time();
<     for( int j=0; j<repeat_test; j++ ) {
<       strcpy(s, argv[i]);
<     }
<     long copy_t2 = get_time();
<
42c36
<     long time_spent = (test_t2-test_t1) - (copy_t2-copy_t1);
---
>     long time_spent = test_t2-test_t1;
```

The less than symbols mean that the change is in the file in the first parameter (test\_timer.c).  The greater than symbol indicates that the change is in the file in the second parameter (../1\_timer/test\_timer.c).  You should also notice three dashes (---) between the two lines indicating time\_spent.  The 30,35d29 means that the lines were added after line 29 as lines 30-35.  The 42c36 means that line 42 in test\_timer.c was compared with line 36 in ../1\_timer/test\_timer.c.

As shown above, there are two changes.  The first times everything but reverse_string.

```c
  long copy_t1 = get_time();
  for( int j=0; j<repeat_test; j++ ) {
     strcpy(s, argv[i]);
  }
  long copy_t2 = get_time();
```

The second change subtracts the time spent doing everything except the `reverse_string` calls in the reverse string loop.

```c
long time_spent = (test_t2-test_t1) - (copy_t2-copy_t1);
```

### Doing a better job of timing continued

In the last section, we eliminated the cost of the `strcpy` and loop from the timing.  Another thing to do is to reconsider our `reverse_string` function.  The `reverse_string` calls `strlen` to get the length of the string `s`.  We could try to pass the length of `s` into the call.  We can get the length of the argument outside of the repeated, timed test.  For completeness, we will compare the timing of 2\_timer with 3\_timer.

The timing for the work in <i>illustrations/2\_timing/2\_timer</i> was...

```
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 37.4880ns
Reverse => esreveR
time_spent: 12.5070ns
overall time_spent: 49.9950ns
```

This section's code is found in <i>illustrations/2\_timing/3\_timer</i>

```
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 28.8140ns
Reverse => esreveR
time_spent: 6.5320ns
overall time_spent: 35.3460ns
```

Running make yields a 14.5 nanosecond improvement.  The difference between 2\_timer and 3\_timer can be found by running...

```
$ diff test_timer.c ../2_timer/test_timer.c
7c7,8
< void reverse_string( char *s, size_t len ) {
---
> void reverse_string( char *s ) {
>   size_t len = strlen(s);
28,30c29
<     size_t len = strlen(argv[i]);
<     char *s = (char *)malloc(len+1);
<
---
>     char *s = (char *)malloc(strlen(argv[i])+1);
40c39
<       reverse_string(s, len);
---
>       reverse_string(s);
```

Line 7 replaces lines 7-8 in the previous test_timer.c.  Lines 28-30 replace line 29.  Line 40 replaces line 39.

Lines 7-8

```c
void reverse_string( char *s ) {
  size_t len = strlen(s);
```

are replaced with this line at line 7

```c
void reverse_string( char *s, size_t len ) {
```

Instead of getting the string length of `s` in every call, reverse_string now expects the length of the string to be passed into it.

Line 29

```c
char *s = (char *)malloc(strlen(argv[i])+1);
```

is replaced with lines 28-30

```c
size_t len = strlen(argv[i]);
char *s = (char *)malloc(len+1);

```

We get the length of the ith argument one time before calling malloc and use that length in the malloc call.

Line 39

```c
reverse_string(s);
```

is replaced by line 40

```c
reverse_string(s, len);
```

The length is passed into the `reverse_string` call so that `reverse_string` doesn't have to calculate it.  This optimization yielded another 14.5 nanosecond improvement.

## Compiler optimizations

You can sometimes see an improvement in runtime through compiler optimization. This is accomplished by passing the -O3 flag to gcc.

The timing for the work in <i>illustrations/2\_timing/3\_timer</i> was...

```
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 28.8140ns
Reverse => esreveR
time_spent: 6.5320ns
overall time_spent: 35.3460ns
```

This section's code is found in <i>illustrations/2\_timing/4\_timer</i>

```
$ make
gcc -O3 test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 9.7730ns
Reverse => esreveR
time_spent: 2.5150ns
overall time_spent: 12.2880ns
```

Running make yields a 23 nanosecond improvement!  The only difference is in the Makefile.

```
$ diff . ../3_timer/
diff ./Makefile ../3_timer/Makefile
4c4
< 	gcc -O3 test_timer.c -o test_timer
---
> 	gcc test_timer.c -o test_timer
```

The -O3 optimization (full optimization) was turned on in gcc.  It is good practice to compare times with both optimizations turned on and off. Sometimes the fastest code is slower once compiled with the -O3 flag.

# Splitting up code into multiple files

This section's code is found in <i>illustrations/2\_timing/5\_timer</i>

If you run diff -q . ../4_timer/, a brief summary of what changed will be displayed.

```
$ diff -q . ../4_timer/
Files ./Makefile and ../4_timer/Makefile differ
Only in .: file2.c
Files ./test_timer.c and ../4_timer/test_timer.c differ
```

In the test_timer.c, there were two functions besides the main function.  To illustrate how you can break your code up into multiple files, I moved those functions to file2.c

file2.c:

```c
void reverse_string( char *s, size_t len ) {
  char *e = s+len-1;
  while(s < e) {
    char tmp = *s;
    *s = *e;
    *e = tmp;
    s++;
    e--;
  }
}

long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}
```

The next change is to include file2.c in test_timer.c

```c
...
#include <time.h>

#include "file2.c"

int main( int argc, char *argv[]) {
...
```

The compiler inserts the contents of file2.c into test_timer.c.  At this point, if there was another source file that wanted to use these functions, all they would have to do is `#include "file2.c"`.  For example:

test_reverse.c

```c
#include "file2.c"

#include <stdlib.h>
#include <stdio.h>

int main( int argc, char *argv[] ) {
  for( int i=1; i<argc; i++ ) {
    char *s = strdup(argv[i]); // string duplicate
    reverse_string(s);
    printf( "%s => %s\n", argv[i], s );
    free(s);
  }
  return 0;
}
```

```
$ gcc test_reverse.c -o test_reverse
In file included from test_reverse.c:1:
./file2.c:1:31: error: unknown type name 'size_t'
void reverse_string( char *s, size_t len ) {
                              ^
./file2.c:13:18: error: variable has incomplete type 'struct timeval'
  struct timeval tv;
                 ^
./file2.c:13:10: note: forward declaration of 'struct timeval'
  struct timeval tv;
         ^
./file2.c:14:3: warning: implicit declaration of function 'gettimeofday' is invalid in C99 [-Wimplicit-function-declaration]
  gettimeofday(&tv, NULL);
  ^
./file2.c:14:21: error: use of undeclared identifier 'NULL'
  gettimeofday(&tv, NULL);
                    ^
test_reverse.c:8:15: warning: implicitly declaring library function 'strdup' with type 'char *(const char *)' [-Wimplicit-function-declaration]
    char *s = strdup(argv[i]); // string duplicate
              ^
test_reverse.c:8:15: note: include the header <string.h> or explicitly provide a declaration for 'strdup'
2 warnings and 3 errors generated.
```

As you can see, there are lots of errors.  It's generally best to work through errors starting with the first one, and working down.  This is because the first error may be causing subsequent errors to occur.

The first error is

```
In file included from test_reverse.c:1:
./file2.c:1:31: error: unknown type name 'size_t'
void reverse_string( char *s, size_t len ) {
```

The error indicates that the type size_t is not known.  Use your favorite search engine and query "error: unknown type name 'size_t'".  You should be able to find that it is defined in stdio.h, so the solution is to include stdio.h.  Notice that we do include stdio.h in test_reverse.c, but it is after we include file2.c.  Ideally, file2.c would be complete, so let's modify file2.c to include stdio.h and rerun gcc.  You will have to add the following line to the beginning of file2.c.

```c
#include <stdio.h>
```

and then run gcc again.

```
$ gcc test_reverse.c -o test_reverse
In file included from test_reverse.c:1:
./file2.c:15:18: error: variable has incomplete type 'struct timeval'
  struct timeval tv;
                 ^
./file2.c:15:10: note: forward declaration of 'struct timeval'
  struct timeval tv;
         ^
./file2.c:16:3: warning: implicit declaration of function 'gettimeofday' is invalid in C99 [-Wimplicit-function-declaration]
  gettimeofday(&tv, NULL);
  ^
test_reverse.c:8:15: warning: implicitly declaring library function 'strdup' with type 'char *(const char *)' [-Wimplicit-function-declaration]
    char *s = strdup(argv[i]); // string duplicate
              ^
test_reverse.c:8:15: note: include the header <string.h> or explicitly provide a declaration for 'strdup'
test_reverse.c:9:21: error: too few arguments to function call, expected 2, have 1
    reverse_string(s);
    ~~~~~~~~~~~~~~  ^
./file2.c:3:1: note: 'reverse_string' declared here
void reverse_string( char *s, size_t len ) {
^
2 warnings and 2 errors generated.
```

The following two errors are missing now...

```
In file included from test_reverse.c:1:
./file2.c:1:31: error: unknown type name 'size_t'
void reverse_string( char *s, size_t len ) {

./file2.c:14:21: error: use of undeclared identifier 'NULL'
  gettimeofday(&tv, NULL);
```

NULL was also defined in stdio.h, so by fixing the first error, we fixed another error.  This is a big reason that I fix errors from the beginning to the end. Often the fixes fix other problems.

The next error is

```c
In file included from test_reverse.c:1:
./file2.c:15:18: error: variable has incomplete type 'struct timeval'
  struct timeval tv;
```

Again, use your favorite search engine and query "error: variable has incomplete type 'struct timeval'".  I found the answer was to `#include <sys/time.h>` on stack overflow.  Again, let's add this to file2.c right after the `#include <stdio.h>` call.

Add this right after `#include <stdio.h>` to file2.c

```c
#include <sys/time.h>
```

Again, run gcc to check for more errors

```
$ gcc test_reverse.c -o test_reverse
test_reverse.c:8:15: warning: implicitly declaring library function 'strdup' with type 'char *(const char *)' [-Wimplicit-function-declaration]
    char *s = strdup(argv[i]); // string duplicate
              ^
test_reverse.c:8:15: note: include the header <string.h> or explicitly provide a declaration for 'strdup'
test_reverse.c:9:21: error: too few arguments to function call, expected 2, have 1
    reverse_string(s);
    ~~~~~~~~~~~~~~  ^
./file2.c:4:1: note: 'reverse_string' declared here
void reverse_string( char *s, size_t len ) {
^
1 warning and 1 error generated.
```

Again, by fixing the one error, we fixed more.  Let's continue with the next warning.

```
test_reverse.c:8:15: warning: implicitly declaring library function 'strdup' with type 'char *(const char *)' [-Wimplicit-function-declaration]
    char *s = strdup(argv[i]); // string duplicate
              ^
test_reverse.c:8:15: note: include the header <string.h> or explicitly provide a declaration for 'strdup'
```

Notice that this time, gcc actually gives us a hint (include the header <string.h>).  Let's add this to test_reverse.c

test_reverse.c:

```c
#include "file2.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] ) {
  for( int i=1; i<argc; i++ ) {
    char *s = strdup(argv[i]); // string duplicate
    reverse_string(s);
    printf( "%s => %s\n", argv[i], s );
    free(s);
  }
  return 0;
}
```

We don't have to recompile every time.  We can go ahead and look at the next error and try and fix more than one.

```
test_reverse.c:9:21: error: too few arguments to function call, expected 2, have 1
    reverse_string(s);
    ~~~~~~~~~~~~~~  ^
./file2.c:4:1: note: 'reverse_string' declared here
void reverse_string( char *s, size_t len ) {
^
```

Again, gcc is helping us by pointing out that `reverse_string` requires two arguments and what they should be.  I forgot to pass the length of the string as the second argument.  We can do that with `strlen`.

test_reverse.c:

```c
#include "file2.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] ) {
  for( int i=1; i<argc; i++ ) {
    char *s = strdup(argv[i]); // string duplicate
    reverse_string(s, strlen(s));
    printf( "%s => %s\n", argv[i], s );
    free(s);
  }
  return 0;
}
```

Now if you run gcc:

```
$ gcc test_reverse.c -o test_reverse
$
```

All of the errors and warnings are gone.

We can run test_reverse as follows...

```
$ ./test_reverse This is a test
This => sihT
is => si
a => a
test => tset
```

If you were following along and changing file2.c, you can run `$ make` again to see if the changes work in the original test_timer.

```
$ make
gcc -O3 test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 11.9100ns
Reverse => esreveR
time_spent: 3.1390ns
overall time_spent: 15.0490ns
```

The changes work.

Before continuing, you should note that there was one important change in Makefile

```
test_timer: test_timer.c
```

changed to

```
test_timer: test_timer.c file2.c
```

This change is helpful because it says that the test_timer block should run if any of the files have changed.  Now that test_timer is made up of two files, we should list both files.  Internally, `make` simply looks at the time stamp of test_timer and test_timer.c and file2.c.  If test_timer.c or file2.c has a timestamp that is greater than test_timer (or test_timer doesn't exist), then the block will be run again.

### Splitting up your code into multiple files part 2

This section's code is found in <i>illustrations/2\_timing/6\_timer</i>

It is generally a good idea to make each file or group of files self-contained.  In the last section, we didn't notice errors when running `make`, but we did when trying to build test_reverse.  The errors didn't occur because before including file2.c we had already included stdio.h and sys/time.h.  In general, you want to include your custom code first, as this will present the error earlier.  We should make the following change to test_timer.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "file2.c"
```

changes to

```c
#include "file2.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
```

If you go back and change test_timer.c in <i>illustrations/2\_timing/5\_timer</i> and then run `make`, you will get some of the same errors that we had when building test_reverse.

In illustrations/2_timing/5_timer after making above change

```
$ make
gcc -O3 test_timer.c -o test_timer
In file included from test_timer.c:1:
./file2.c:1:31: error: unknown type name 'size_t'
void reverse_string( char *s, size_t len ) {
                              ^
./file2.c:13:18: error: variable has incomplete type 'struct timeval'
  struct timeval tv;
                 ^
./file2.c:13:10: note: forward declaration of 'struct timeval'
  struct timeval tv;
         ^
./file2.c:14:3: warning: implicit declaration of function 'gettimeofday' is invalid in C99 [-Wimplicit-function-declaration]
  gettimeofday(&tv, NULL);
  ^
./file2.c:14:21: error: use of undeclared identifier 'NULL'
  gettimeofday(&tv, NULL);
                    ^
One warning and three errors generated.
make: *** [test_timer] Error 1
```

If we make the change in <i>illustrations/2\_timing/6\_timer</i> and run `make`

```
$ make
gcc -O3 test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.6000ns
Reverse => esreveR
time_spent: 1.7670ns
overall time_spent: 9.3670ns
```

In this section, I also removed the following two lines from test_timer.c as test_timer.c doesn't directly call any time-related functions.

```c
#include <sys/time.h>
#include <time.h>
```

It all works new because we have corrected file2.c to have the right include statements.  Particularly if you are developing a new package, it is a good idea to include your packages before outside or system packages.

## Separating the implementation from the interface

This section's code is found in <i>illustrations/2\_timing/7\_timer</i>

The key to writing large software projects that work is to define objects and make them highly reusable in a clear fashion.  When defining objects, it is best to split the object into an interface and an implementation.  Ideally, the interface will only describe what the object does and not how it works.  

This is much like an automobile and a transmission.  An automobile might require a transmission with certain specifications.  That automobile doesn't care how the transmission is made.  An interface is like the specifications to a transmission.  The implementation would include how the transmission works.  C allows you to declare that a thing exists before it has been defined.  Typically, this is done in header files (files that end in a .h extension), but it can be done anywhere.  In this section, we will show how to declare that the functions in file2.c exist without including them.

In test_timer.c

```c
#include "file2.c"
```

changes to

```c
void reverse_string( char *s, size_t len );
long get_time();
```

Declaring the functions this way lets the compiler know that the functions do exist.  The syntax is the same as writing the function, except you replace the {} with a semicolon. These are called function prototypes.

If you run `make`...

```
$ make
gcc -O3 test_timer.c -o test_timer
Undefined symbols for architecture x86_64:
  "_get_time", referenced from:
      _main in test_timer-276d5b.o
  "_reverse_string", referenced from:
      _main in test_timer-276d5b.o
ld: symbol(s) not found for architecture x86_64
```

This happens because we have defined the functions, but they ultimately don't make it into the binary.  To fix this, change the following line in the Makefile.

```Makefile
gcc -O3 test_timer.c -o test_timer
```

Changes to (add file2.c to the gcc line)

```Makefile
gcc -O3 file2.c test_timer.c -o test_timer
```

Save and run `make` again

```
$ make
gcc -O3 file2.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 8.1540ns
Reverse => esreveR
time_spent: 2.4490ns
overall time_spent: 10.6030ns
```

Everything works as expected.

### Separating the implementation from the interface (part 2)

This section's code is found in <i>illustrations/2\_timing/8\_timer</i>

Ideally, file2.c should have a corresponding file2.h that describes what exists in file2.c.  We can put the definition of `get_time` and `reverse_string` into file2.h as follows:

file2.h:

```c
#ifndef _file2_H
#define _file2_H

#include <stdlib.h>

void reverse_string( char *s, size_t len );
long get_time();

#endif
```

The #ifndef, #define, #endif block prevents the compiler from including the same code over and over again.  These statements check to see if _file2_H has already been defined and if it hasn't, it defines it.  If it has been defined previously, the compiler skips the block of code.

size_t is used in `reverse_string`, which is defined in stdlib.h, so we include stdlib.h.  One last change is to update file2.c to include file2.h so that we can be sure that file2.c and file2.h don't get out of sync.

file2.c

```c
#include "file2.h"

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
...
```

Now we can include file2.h in test_timer.c instead of defining the functions in test_timer.c.

test_timer.c changes from

```c
...

void reverse_string( char *s, size_t len );
long get_time();

...
```

to

```c
#include "file2.h"
```

Again, since this is something that is our own, we should include it before the system or third party includes.  The Makefile changed slightly.

Makefile changed from

```Makefile
test_timer: test_timer.c file2.c
```

to

```Makefile
test_timer: test_timer.c file2.h file2.c
```

So that test_timer will be built if file2.h is updated.

## Defining an object

This section's code is found in <i>illustrations/2\_timing/9\_xxx</i>

To build the code run...

```
$ make
gcc -O3 xxx.c test_xxx.c -o test_xxx
./test_xxx
Hello: (1, 2)
```

The objects that will be created throughout the rest of this book will look something like this (where xxx is the object name):

xxx.h

```c
#ifndef _xxx_H
#define _xxx_H

struct xxx_s;
typedef struct xxx_s xxx_t;

xxx_t * xxx_init(int param1, int param2);
void xxx_destroy( xxx_t *h );

void xxx_do_something( xxx_t *h, const char *prefix );

#endif
```

The header file or interface simply provides what exists and doesn't offer details into how xxx might work.  The `int param1`, `int param2`, and `const char *prefix` above are just examples.  The init function often doesn't have any parameters.  The init function is used to create the structure so that the other functions can use it within the xxx object.  The destroy function destroys the xxx_t structure.  The xxx_t structure doesn't have members.  It is just declared (like the functions after it) so that the compiler knows that the structure is defined somewhere.  By defining the structure like this, applications using the xxx object cannot access members of the structure.  The implementation is free to put whatever members in the structure to make the function work.  The only thing which applications that use this object are required to do is create (or init) the object and then pass it around to the various functions that use that type.  If there is a destroy method, then the application is expected to destroy the object to clean it up.

To use the above interface, you will include it and call its functions.

```c
#include "xxx.h"

int main() {
  xxx_t *handle = xxx_init(1, 2);
  xxx_do_something(handle, "Hello");
  xxx_destroy(handle);
  return 0;
}
```

xxx.h is just an interface.  The implementation of those functions would be in a C file.

xxx.c:

```c
#include "xxx.h"

#include <stdio.h>

struct xxx_s {
  int x;
  int y;
};

xxx_t * xxx_init(int param1, int param2) {
  xxx_t *h = (xxx_t *)malloc(sizeof(xxx_t));
  h->x = param1;
  h->y = param2;
  return h;
}

void xxx_destroy( xxx_t *h ) {
  free(h);
}

void xxx_do_something( xxx_t *h, const char *prefix ) {
  printf( "%s: (%d, %d)\n", prefix, h->x, h->y );
}
```

One thing that hasn't been talked about much is the struct keyword.

In C, you can group multiple data types as variables to define a larger structure.  A classic example is to create a point structure.  Typically, they would be defined as follows:

```c
#include <stdio.h>

struct point {
  int x;
  int y;
};

int main( int argc, char *argv[]) {
  struct point p;
  p.x = 1;
  p.y = 100;
  printf( "(%d, %d)\n", p.x, p.y );
  return 0;
}
```

You can use typedef to reduce the need for typing the keyword struct over and over...

```c
#include <stdio.h>

typedef struct point_s {
  int x;
  int y;
} point;

int main( int argc, char *argv[] ) {
  point p; // or struct point_s p;
  p.x = 1;
  p.y = 100;
  printf( "(%d, %d)\n", p.x, p.y );
  return 0;
}
```

You can also define that a struct will exist and then define it later.

```c
#include <stdio.h>

struct point_s;
typedef struct point_s point;

struct point_s {
  int x;
  int y;
};

int main( int argc, char *argv[] ) {
  point p; // or struct point_s p;
  p.x = 1;
  p.y = 100;
  printf( "(%d, %d)\n", p.x, p.y );
  return 0;
}
```

To access members of a struct value, you use the dot notation.  If you have a pointer to the structure, then you access members using the -> syntax.

```c
#include <stdio.h>

struct point_s;
typedef struct point_s point;

struct point_s {
  int x;
  int y;
};

int main( int argc, char *argv[] ) {
  point p; // or struct point_s p;
  point *ptr = &p; // &p gets the address of p (or returns a pointer to p).
  ptr->x = 1;
  ptr->y = 100;
  printf( "(%d, %d)\n", ptr->x, ptr->y );
  return 0;
}
```

In the code above, the xxx_s is declared in xxx.h and then typedef'd to xxx_t.

xxx.h:

```c
struct xxx_s;
typedef struct xxx_s xxx_t;
```

The xxx_s struct is actually defined in xxx.c:

```c
struct xxx_s {
  int x;
  int y;
};
```

The xxx_init function creates the xxx_t structure, sets the members, and then returns the newly allocated structure.

```c
xxx_t * xxx_init(int param1, int param2) {
  xxx_t *h = (xxx_t *)malloc(sizeof(xxx_t));
  h->x = param1;
  h->y = param2;
  return h;
}
```

The xxx_destroy function frees the memory that was allocated in xxx_init.

```c
void xxx_destroy( xxx_t *h ) {
  free(h);
}
```

The xxx_do_something function prints x and y after printing the prefix which is passed into it.

```c
void xxx_do_something( xxx_t *h, const char *prefix ) {
  printf( "%s: (%d, %d)\n", prefix, h->x, h->y );
}
```

## The timer interface

The following code is found in <i>illustrations/2\_timing/10\_timer</i>

The Makefile in this section has one minor difference.  This project will have a separate timer object.  The gcc command will run if test_timer.c, timer.c, or timer.h are changed.

```Makefile
test_timer: test_timer.c timer.c timer.h
	gcc -O3 timer.c test_timer.c -o test_timer
```

Build the project...

```
$ make
gcc -O3 timer.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 9.7730ns
Reverse => esreveR
time_spent: 2.5150ns
overall time_spent: 12.2880ns
```

The timing is the same as 4_timer.

In test_timer.c the following lines of code exists.

```c
#include "timer.h"
```

To make our project reusable, we need to break it up into objects (or interfaces and implementations).  In the above example, we are including timer.h (which we will make next).  

timer.h (comments removed)

```c
#ifndef _timer_H
#define _timer_H

struct timer_s;
typedef struct timer_s timer_t;

timer_t *timer_init(int repeat);
timer_t *timer_timer_init(timer_t *t);

void timer_destroy(timer_t *t);

void timer_subtract(timer_t *t, timer_t *sub);
void timer_add(timer_t *t, timer_t *add);

void timer_start(timer_t *t);
void timer_stop(timer_t *t);

double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);

#endif
```

Earlier, you learned that the struct keyword allows you to group zero or more types to form a new type.  In general, I like to name struct types with a _s suffix and then typedef them to have a _t suffix.  In the timer.h above, the timer_s struct was declared, but never actually defined.  The details of what is in the structure are part of the implementation and aren't meant to be known externally.  C allows you to define types in this way and use them as long as you only reference them as pointers.  All pointers have the same size (the number of bits that the CPU supports or the sizeof(size_t)).  For now, just recognize that there is a new type named timer_t and that timer_s will be defined in timer.c.

```c
struct timer_s;
typedef struct timer_s timer_t;
```

I follow a pattern where every function is prefixed by the object name (in this case, timer).  The primary type (if there is one) is usually the object name followed by _t.  Objects will typically have an init and a destroy method.  The job of the header file is to create an interface for applications to use.  It should hide the implementation details as much as possible.  I usually will define an interface before defining an implementation.

Initialize the timer.  `repeat` is necessary to indicate how many times the test will be repeated within the application so that the final result represents that.  If a thing is only being timed with a single repetition, then use a value of 1.  This function will allocate the timer_t structure and fill its members appropriately.  To free up the resources associated with this call, you must call timer_destroy with the return value of this call.

```c
timer_t *timer_init(int repeat);
```

Initialize a timer from another timer.  This will subtract the time spent and set the repeat from the timer which is passed as a parameter to `timer_timer_init`.

```c
timer_t *timer_timer_init(timer_t *t);
```

Destroy the timer created from `timer_init` or `timer_timer_init`.

```c
void timer_destroy(timer_t *t);
```

Subtract the amount of time spent in `sub` from the current timer (t)

```c
void timer_subtract(timer_t *t, timer_t *sub);
```

Add the amount of time spent in `add` to the current timer (t)

```c
void timer_add(timer_t *t, timer_t *add);
```

Start the timer (t)

```c
void timer_start(timer_t *t);
```

Stop the timer (t)

```c
void timer_stop(timer_t *t);
```

Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms), and seconds (sec).

```c
double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);
```

Notice that timer.h doesn't mention how it is going to be implemented in any way.  It simply names functions in a way that is useable and describes what they do.

I usually copy the .h (header file) to a similarly named .c (implementation file) and then fill in the details.

timer.c (incomplete)

```c
#ifndef _timer_H
#define _timer_H

struct timer_s;
typedef struct timer_s timer_t;

timer_t *timer_init(int repeat);
timer_t *timer_timer_init(timer_t *t);

void timer_destroy(timer_t *t);

void timer_subtract(timer_t *t, timer_t *sub);
void timer_add(timer_t *t, timer_t *add);

void timer_start(timer_t *t);
void timer_stop(timer_t *t);

double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);

#endif
```

The first thing to do is to remove the `#ifndef/#define/#endif` and replace it with an `#include "timer.h"`.

timer.c (incomplete)

```c
#include "timer.h"

struct timer_s;
typedef struct timer_s timer_t;

timer_t *timer_init(int repeat);
void timer_destroy(timer_t *t);

int timer_get_repeat(timer_t *t);
void timer_set_repeat(timer_t *t, int repeat);

void timer_subtract(timer_t *t, timer_t *sub);
void timer_add(timer_t *t, timer_t *add);

void timer_start(timer_t *t);
void timer_stop(timer_t *t);

double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);
```

The next thing to do is to fill in the timer_s details and remove the typedef that follows.

```c
struct timer_s;
typedef struct timer_s timer_t;
```

becomes

```c
struct timer_s {
  long base;
  int repeat;
  long time_spent;
  long start_time;
};
```

`repeat` is the number of times that the test will be repeated.  `time_spent` is the total time that has been spent within this object so far.  `start_time` is recorded every time `timer_start` is called.  `base` is used to track how much time should be added or subtracted from the final time spent based upon other timers.

```c
timer_t *timer_init(int repeat);
```

becomes

```c
#include <stdlib.h>

timer_t *timer_init(int repeat) {
  timer_t *t = (timer_t *)malloc(sizeof(timer_t));
  t->repeat = repeat;
  t->base = t->time_spent = t->start_time = 0;
  return t;
}
```

`#include <stdlib.h>` is added at the top of the file since it is needed for malloc.  All of the members are initialized to zero except repeat which is set to the value passed into `timer_init`.

```c
void timer_destroy(timer_t *t);
```

becomes

```c
void timer_destroy(timer_t *t) {
  free(t);
}
```

`timer_destroy` simply needs to free the memory that was allocated by either of the init methods.

```c
int timer_get_repeat(timer_t *t);
void timer_set_repeat(timer_t *t, int repeat);
```

becomes

```c
int timer_get_repeat(timer_t *t) {
  return t->repeat;
}

void timer_set_repeat(timer_t *t, int repeat) {
  t->repeat = repeat;
}
```
Because the details of the timer_t structure are only known to timer.c, functions must be used to access members of the structure.  This concept is known as encapsulation and keeps separation between the interface and the implementation.

```c
void timer_subtract(timer_t *t, timer_t *sub);
void timer_add(timer_t *t, timer_t *add);
```

becomes

```c
void timer_subtract(timer_t *t, timer_t *sub) {
  t->base -= (sub->time_spent+sub->base);
}

void timer_add(timer_t *t, timer_t *add) {
  t->base += (add->time_spent+add->base);
}
```

These methods may also have been initialized from another timer, so the `base` is added to the `time_spent`.  

```c
void timer_start(timer_t *t);
void timer_stop(timer_t *t);
```

becomes

```c
void timer_start(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void timer_stop(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}
```

`timer_start` is basically the same as `get_time` which was defined earlier.  The difference is that it sets the `start_time` member of the struct timer_t t.

Finally, the following functions are changed too.

```c
double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);
```

becomes

```c
double timer_ns(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return (ts*1000.0) / r;
}

double timer_us(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / r;
}

double timer_ms(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000.0);
}

double timer_sec(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000000.0);
}
```

The member `time_spent` and `base` are in microseconds.  Each function above does the appropriate conversions.

timer.c (the finished product)

```c
#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

struct timer_s {
  long base;
  int repeat;
  long time_spent;
  long start_time;
};

timer_t *timer_init(int repeat) {
  timer_t *t = (timer_t *)malloc(sizeof(timer_t));
  t->repeat = repeat;
  t->base = t->time_spent = t->start_time = 0;
  return t;
}

void timer_destroy(timer_t *t) {
  free(t);
}

/* get the number of times a task is meant to repeat */
int timer_get_repeat(timer_t *t) {
  return t->repeat;
}

/* set the number of times a task is meant to repeat */
void timer_set_repeat(timer_t *t, int repeat) {
  t->repeat = repeat;
}


void timer_subtract(timer_t *t, timer_t *sub) {
  t->base -= (sub->time_spent+sub->base);
}

void timer_add(timer_t *t, timer_t *add) {
  t->base += (add->time_spent+add->base);
}

void timer_start(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t->start_time = (tv.tv_sec * 1000000) + tv.tv_usec;
}

void timer_stop(timer_t *t) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long v = (tv.tv_sec * 1000000) + tv.tv_usec;
  v -= t->start_time;
  t->time_spent += v;
}

double timer_ns(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return (ts*1000.0) / r;
}

double timer_us(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / r;
}

double timer_ms(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000.0);
}

double timer_sec(timer_t *t) {
  double r = t->repeat * 1.0;
  double ts = t->time_spent + t->base;
  return ts / (r*1000000.0);
}
```

Now that the timer interface has been defined, we can use it in our test_timer.c code.

test_timer.c

```c
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_string( char *s, size_t len ) {
  char *e = s+len-1;
  while(s < e) {
    char tmp = *s;
    *s = *e;
    *e = tmp;
    s++;
    e--;
  }
}

int main( int argc, char *argv[]) {
  int repeat_test = 1000000;
  timer_t *overall_timer = timer_init(repeat_test);
  for( int i=1; i<argc; i++ ) {
    size_t len = strlen(argv[i]);
    char *s = (char *)malloc(len+1);

    timer_t *copy_timer = timer_init(timer_get_repeat(overall_timer));
    timer_start(copy_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
    }
    timer_stop(copy_timer);

    timer_t *test_timer = timer_init(timer_get_repeat(overall_timer));
    timer_start(test_timer);
    for( int j=0; j<repeat_test; j++ ) {
      strcpy(s, argv[i]);
      reverse_string(s, len);
    }
    timer_stop(test_timer);
    timer_subtract(test_timer, copy_timer);
    timer_add(overall_timer, test_timer);

    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", timer_ns(test_timer) );

    timer_destroy(test_timer);
    timer_destroy(copy_timer);
    free(s);
  }
  printf( "overall time_spent: %0.4fns\n", timer_ns(overall_timer) );
  timer_destroy(overall_timer);
  return 0;
}
```

The beginning should look familiar

```c
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_string( char *s, size_t len ) {
  char *e = s+len-1;
  while(s < e) {
    char tmp = *s;
    *s = *e;
    *e = tmp;
    s++;
    e--;
  }
}

int main( int argc, char *argv[]) {
...
```

We include the interface (or header file) we just created in the first line.  The rest of the code above has already been discussed.  

Considering what the code looked like before using the timer object offers insight into understanding the code.

Consider the following code from <i>illustrations/2\_timing/8\_timer</i>

```c
long overall_time = 0;
for( int i=1; i<argc; i++ ) {
  size_t len = strlen(argv[i]);
  char *s = (char *)malloc(len+1);

  long copy_t1 = get_time();
  for( int j=0; j<repeat_test; j++ ) {
    strcpy(s, argv[i]);
  }
  long copy_t2 = get_time();
```

vs

```c
timer_t *overall_timer = timer_init(repeat_test);
for( int i=1; i<argc; i++ ) {
  size_t len = strlen(argv[i]);
  char *s = (char *)malloc(len+1);

  timer_t *copy_timer = timer_init(timer_get_repeat(overall_timer));
  timer_start(copy_timer);
  for( int j=0; j<repeat_test; j++ ) {
    strcpy(s, argv[i]);
  }
  timer_stop(copy_timer);
```

First, it is important to recognize that the new code is longer.  The number of lines of code isn't always a good measure of deciding whether to create an object.  In the case of the timer object, the `get_time` function is hidden in the timer object.  

Instead of just having a counter for the overall time, I've replaced it with a timer object named overall_timer.

```c
long overall_time = 0;
```

becomes

```c
timer_t *overall_timer = timer_init(repeat_test);
```

In order to measure the time to copy, we will need a copy timer and will need start and stop it.

```c
long copy_t1 = get_time();
for( int j=0; j<repeat_test; j++ ) {
  strcpy(s, argv[i]);
}
long copy_t2 = get_time();
```

becomes

```c
timer_t *copy_timer = timer_init(timer_get_repeat(overall_timer));
timer_start(copy_timer);
for( int j=0; j<repeat_test; j++ ) {
  strcpy(s, argv[i]);
}
timer_stop(copy_timer);
```

The `timer_init` call gets the `repeat_test` variable from the `overall_timer` since they should all be the same.  The `timer_init` call could have just been

```c
timer_t *copy_timer = timer_init(repeat_test);
```

but that would require that the `repeat_test` variable be separate.  This becomes important in more complex timings, but for now, I'm primarily showing how `timer_get_repeat` can be useful.  In C, parameters to functions can be the result of other functions.  The `timer_get_repeat` call would be resolved before calling `timer_init`.

To find the time spent in the first example, you need to subtract `copy_t1` from `copy_t2` and then divide that by the repeat factor.  This will return the number of microseconds that elapsed.  The `timer_us(copy_timer)` would yield the same result.  With the timer object, it is easy to get the time spent in microseconds, but also nanoseconds, milliseconds, and seconds.  Also, the timer object automatically converts the time spent to a double (a decimal).


The next section of code times the test (the reverse call) in the same way that the copy timer worked.

```c
timer_t *test_timer = timer_init(timer_get_repeat(overall_timer));
timer_start(test_timer);
for( int j=0; j<repeat_test; j++ ) {
  strcpy(s, argv[i]);
  reverse_string(s, len);
}
timer_stop(test_timer);
```

The next two lines begin to show additional usefulness of the timer object.  The `copy_timer` is subtracted from the `test_timer` and then the `test_timer` is added to the `overall_timer`.  

```c
timer_subtract(test_timer, copy_timer);
timer_add(overall_timer, test_timer);
```

I think this is easier to read than

```c
long time_spent = (test_t2-test_t1) - (copy_t2-copy_t1);
overall_time += time_spent;
```

The timer objects also reduce the number of variables that are maintained in the code (each timer object replaces a t2 and t1 variable).

Finally, the following line

```c
printf( "time_spent: %0.4fns\n", (time_spent*1000.0)/(repeat_test*1.0));
```

is replaced with

```c
printf( "time_spent: %0.4fns\n", timer_ns(test_timer) );
```

which again reduces complexity.  It is also easy to switch from nanoseconds to another measure if desired.

## Making the timer object reusable

The following code is found in <i>illustrations/2\_timing/11\_timer</i>

This timer object is done and is ready to be reused.  In C, all of your functions share the same namespace.  If another project has a function named `timer_init`, there will be a conflict.  Projects typically adopt a package prefix in addition to the object prefix to prevent conflicts.  For this project, we will use ac (another c library).

My rules for adding the prefix are:

```
1.  Comments should refer to the object name only.
2.  All code references should be prefixed.
3.  Functions should only use a prefix once even if multiple objects are
    referenced in the name
```

The first thing to do is to copy timer.h to ac_timer.h and then apply the rules mentioned above.  The ac_timer header file exists in the current directory. What follows is a partial diff.

```
$ diff timer.h ac_timer.h
1,2c1,2
< #ifndef _timer_H
< #define _timer_H
---
> #ifndef _ac_timer_H
> #define _ac_timer_H
4,5c4,5
< struct timer_s;
< typedef struct timer_s timer_t;
---
> struct ac_timer_s;
> typedef struct ac_timer_s ac_timer_t;
12c12
< timer_t *timer_init(int repeat);
---
> ac_timer_t *ac_timer_init(int repeat);
...
```

The same thing is done for timer.c (copy to ac_timer.c and apply rules).

```
$ diff timer.c ac_timer.c
1c1
< #include "timer.h"
---
> #include "ac_timer.h"
7c7
< struct timer_s {
---
> struct ac_timer_s {
14,15c14,15
< timer_t *timer_init(int repeat) {
<   timer_t *t = (timer_t *)malloc(sizeof(timer_t));
---
> ac_timer_t *ac_timer_init(int repeat) {
>   ac_timer_t *t = (ac_timer_t *)malloc(sizeof(ac_timer_t));
21c21
...
```

Finally, test_timer.c needs to change to use ac_timer instead of timer.

```c
#include "ac_timer.h"
```

becomes

```c
#include "timer.h"
```

```c
timer_t *overall_timer = timer_init(repeat_test);
```

becomes

```c
ac_timer_t *overall_timer = ac_timer_init(repeat_test);
```

and so on.

## Splitting up a project into multiple directories
 - Moving ac_timer to src (and variables in Makefile)

The following code is found in <i>illustrations/2\_timing/12\_timer</i>

Once the object has the ac prefix, we can move it to the src directory.  This is done by executing the following command.

Please note that this command will not work because it has already been done for you.

```
mv ac_timer.h ac_timer.c $ac/src
```

You can see that the files exist by running

```
cd $ac/src
ls -l ac_timer.*
```

which will output

```
-rw-r--r--  1 ac  staff  1777 Sep 11 12:12 ac_timer.c
-rw-r--r--  1 ac  staff  1314 Sep 11 12:12 ac_timer.h
```

You can change back to the previous directory (illustrations/2_timing/12_timer) by running

```
cd -
```

or

```
cd $ac/illustrations/2_timing/12_timer
```

cd - allows you to change to the directory that you were in previously and is useful.

Ensuring the program builds properly requires a few changes to Makefile. The diff looks like the following:

```
$ diff Makefile ../11_timer/Makefile
1,5d0
< ROOT=../../..
< OBJECTS=$(ROOT)/src/ac_timer.c
< HEADER_FILES=$(ROOT)/src/ac_timer.h
< FLAGS=-O3 -I$(ROOT)/src
<
8,9c3,4
< test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
< 	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer
---
> test_timer: test_timer.c ac_timer.c ac_timer.h
> 	gcc -O3 ac_timer.c test_timer.c -o test_timer
```

A new section of variables are added at the top and the `test_timer` target uses those variables.

The Makefile for this project.

```Makefile
ROOT=../../..
OBJECTS=$(ROOT)/src/ac_timer.c
HEADER_FILES=$(ROOT)/src/ac_timer.h
FLAGS=-O3 -I$(ROOT)/src

all: test_timer examples

test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
```

`make` allows you to create variables using the <name>=<value> syntax outside of sections.  You can then reference the value of those variables by enclosing the name in $(<name>).  One variable can reference another variable (see OBJECTS and ROOT above).  For gcc to find the ac_timer.h file, the src path needs to be added to gcc's include path.  That is done by using the -I<directory> option.  If you have multiple include paths, you can specify -I<directory> multiple times.

## Splitting up the Makefile

The following code is found in <i>illustrations/2\_timing/13\_timer</i>

Ideally, the objects in src could be defined by a Makefile in src, and that Makefile should be included.  In src, there is a file named Makefile.include, which defines the variables that were in the Makefile in the last section (as well as for other objects in src).

Makefile.include in src

```Makefile
OBJECTS=$(ROOT)/src/ac_timer.c $(ROOT)/src/ac_buffer.c $(ROOT)/src/ac_pool.c
HEADER_FILES=$(ROOT)/src/ac_timer.h $(ROOT)/src/ac_buffer.h $(ROOT)/src/ac_pool.h
FLAGS=-O3 -I$(ROOT)/src
```

Makefile in illustrations/2_timing/13_timer

```Makefile
ROOT=../../..
include $(ROOT)/src/Makefile.include

all: test_timer examples

test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
```

The difference is that instead of the OBJECTS, HEADER_FILES, and FLAGS variables being specified in the Makefile, they are included by including $(ROOT)/src/Makefile.include.

[Table of Contents (only if viewing on Github)](../../../README.md)
