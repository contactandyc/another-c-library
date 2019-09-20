# Timing Code
## The first project (an introduction to C)

In the project, there is an illustrations folder that contains most of the code.  There is also a src directory where the final code exists.  Normally, one would start with a hello world project. That exists later in a section called Hello Buffer.  If you are lost, hopefully, it will make more sense once you get to the Hello Buffer section. I would recommend reading and working through the examples in this chapter and then coming back after working through Hello Buffer.  The code for this chapter is located in <i>illustrations/2_timing</i>

At various points in this project, we will be timing code in an attempt to optimize it.  Our first object is going to be simple but will illustrate how I plan to maintain separation between interfaces and their respective implementation.

The following code is found in <i>illustrations/2_timing/1_timer</i>
```bash
cd $stla/illustrations/2_timing/1_timer
```

```bash
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

`double` (and `float`) are used for decimals.  Because `float` only uses 32 bits, it loses precision quickly.  I do not tend to use it for anything.  Instead, I opt to use `double`

`typedef` can be utilized to define your own type using the following syntax:

```
typedef <existing type> <new type name>;
```

For example, the following would define a new type called number_t, which has an underlying type of unsigned int.
```c
typedef unsigned int number_t;

number_t a = 100;
```

Every data type in C has a size.  The size of a pointer is always the same (it is the same as the size_t type). The `sizeof()` operator determines the size of a type or variable. `sizeof(number_t)` finds the size of the `number_t` type (and return 4 since unsigned int is 4 bytes). `sizeof(void)` is not allowed as that does not make sense.

A variable can be cast from one type to another either implicitly or explicitly.  Imagine you want to convert an int to a double or vice versa.

```c
int x = 100;
double y = x;
y = y + 0.05;
x = y;
printf( "%d\n", x ); // would print 100
```

When the casting happens, precision is lost if the new type cannot accommodate for the value.  The above example shows casting happening implicitly.  Below is an example of casting happening explicitly.

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

The asterisk means that the variable named `argv` is a pointer to the type `char` which is a single byte and is signed (signed is the default datatype prefix).  The `[]` after the `argv` indicates that `argv` is referencing an array of pointers.

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

Notice that the pointers refer to individual bytes.  In C (and any language), a pointer refers to a location in memory (typically RAM or random access memory).  Strings or sequences of characters are defined by looking for a terminating character (a non-printable 0).  The above example is not technically correct in that what would happen is before the arguments getting to the main function; they would split into 5 strings.

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

C allows for logic to be repeated until a condition is met.  For example, a while loop is technically defined as

```c
while(<condition>)
  do_something;
```

Alternatively, if there are multiple lines of code which need to be executed within the loop

```c
while(<condition>) {
  do_something1;
  do_something2;
  ...
  do_somethingN;
}
```

It is essential to recognize the difference between comparisons and conditions.  A condition can be true or false.  Comparisons require two objects and can evaluate to true or false.  In most (every?) computer language, loops and if logic will use conditions as opposed to comparisons.  Comparisons are a subset of conditions.  In C, true is non-zero, and false is zero.

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

Pointers are declared using an asterisk. Pointers point at values obtained through a process called dereferencing. Dereferencing occurs when an asterisk precedes the pointer. The first time through this loop, assuming that s pointed to "Test", p would be pointing at a value T.  *p would result in the single character 'T'.  In C, single characters are defined in single quotes.  Longer strings are defined in double-quotes.  

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

At this point, p points to just passed the letter s and advanced 4 times, so the length is 4.

It is common for developers to use `int` as a return type. However, it is generally more efficient and less bug-prone to use `size_t`.  In order for a 64 bit CPU to work with an `int`, it must split a register since the CPU is meant to work with 64-bit integers.  This split is not cheap.  Additionally, if you use `size_t` or (`ssize_t` for signed numbers), the program will be more portable to 64-bit systems where a string might be longer than 2 billion bytes.  A better implementation of `strlen` might look like the following.  The only difference is that the `strlen` returns a type `size_t`.  Strings cannot be negative in length, so returning an unsigned number also helps people using the function to understand that.

```c
size_t strlen(char *s) {
  char *p = s;
  while(*p != 0)
    p++;
  return p-s;
}
```

C allows for variables to be declared as constant meaning that they cannot change.  Constants are particularly useful in functions because the function can indicate that the input will not change.  Above, the string remains unmodified while determining the length.  Adding `const` will indicate to the user (and compiler) that what s points to will not be changed.

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

## The void type

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

```bash
warning: initializing 'char *' with an expression of type 'const char *' discards qualifiers
```

To avoid such a warning, either change the type of p to `const char *` or cast s to `char *`
```c
char *p = (char *)s;
```

Imagine you have a function that will print the value of variables of different types such as the following.

Code found in <i>illustrations/2_timing/1_timer</i>

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
```bash
gcc void_pointers.c -o void_pointers -Wall
```

and run it
```bash
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
double type (with 4 decimal places): 1.5000
```

In the program above, the void pointer cannot actually be used directly.  It must be converted to a different pointer type before the value that the pointer is pointing at can be referenced.  

The program above introduced `if`, `else if`, and the `&` operator.  If statements have a similar syntax as the while statement.  

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

The curly braces are not needed if there is exactly one statement.


`else if` works in connection with `if`.  `else if` only happens if the if (or else if statements above it) have not been evaluated to equal a true condition.  Finally, else does not expect a condition and essentially becomes the default block to run if all other conditions were evaluated to be false.  It is also essential to realize the difference between = and ==.  A single equal statement is used for assignment.  A double equal statement indicates an equality test.

In code below, p was of type `void *`.  It must be converted before it can be dereferenced (to get the value of what p is pointing at).

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

Semicolons form natural breaks.  If the loop (while) only has one statement, it doesn't need curly braces.  Let's slowly introduce space back into the code above.

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

The syntax is important for the C compiler to understand where one statement ends and the next one starts.  All of the above examples compile and will work the same.  The last example is easier to read.  C doesn't force you to make the code easy to read.  It is just a very good idea to do so!

There are different types of loops in C.  The for loop is defined as follows.

```c
for( [initialization_code]; [condition]; [post_loop code] ) {

}
```

There are 3 components of the for loop.  The initialization_code, condition, and post_loop code.  Notice that they are enclosed in [] instead of <>.  This is a standard way of indicating that the bit of code is optional.

If there is no initialization_code, condition, or post_loop code, the for loop will continue forever.  The following example would print the string "Hello World!" followed by a new line character continuously forever (or until the program was stopped).
```c
for(;;) {
  printf( "Hello World!\n" );
}
```

This code will loop through the arguments skipping the name of the program (as it is the first argument in the argv array).
```c
for( int i=1; i<argc; i++ ) {
```

The example is going to reverse strings a number of times and time the process.  The arguments that are passed into your program should not be modified by the program.  If you wish to modify an argument, you should first allocate memory for your program to use and then copy the argument into the newly allocated memory.  The malloc function will allocate the number of bytes requested for use.  You can read about it by typing "man malloc" from the command line.  Programs which need extra memory to work with must request that memory from the operating system.  malloc is one of the core ways that that is done.  Memory requested should later be freed using the <i>free</i> call.  The malloc function can return NULL meaning that the memory was not available.  Trying to writing to a pointer which is pointing at NULL will cause your program to crash.  You can check for the error or just allow the program to not so gracefully crash.  In my examples, I'm going to simply allow the program to crash if NULL is returned.  The only other reasonable option would be to have the program fail early which effectively is the same thing.  In the example below, there are a few functions called.  The early writers of C decided to shorten the names of the funtions.

```
malloc - memory allocate
strlen - string length
strcpy - string copy
```

Once s is pointing at newly allocated memory which is the string length of argv[i] + 1 for the zero terminator (the current iteration of the loop), a string copy of the argument is performed so that s points to a copy of the given argument.

```c
for( int i=1; i<argc; i++ ) {
  char *s = (char *)malloc(strlen(argv[i])+1);
  ...
  strcpy(s, argv[i]);
  ...
  free(s);
```

At the beginning of the program there were a few #include statements.  #include effectively copies the contents of the filename into the current program.  In order to use functions like malloc, strcpy, and free, the proper file must be included.  Files with a suffix ".h" are called header files and typically define how to call a function.  

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
```

The include's were chosen because of the following functions that are called within the program.
```
stdio.h -> printf, NULL, and many other io related functions
stdlib.h -> malloc, free, and many other standard library related functions
string.h -> strlen, strcpy, and many other string related functions
sys/time.h -> gettimeofday
time.h -> sometimes an alternate location of gettimeofday
```

This program is going to time how long the reverse string method takes to run.  To get the amount of time that something takes, one might get a start time and an end time and then subtract the start time from the end time.  The following function will get the time in microseconds (millionths of seconds).  Passing NULL to gettimeofday will cause gettimeofday to return the current time.  The timeval structure consists of two members (the number of seconds and the number of microseconds).  
```c
long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}
```

A quick note on using parenthesis.  I strongly believe in making code easier to read.  Technically, I could have skipped the parenthesis without harm due to order of operations, but the code becomes easier to read by adding the parenthesis.

Reversing a string takes an extremely small amount of time.  It's so small that in order to accurately measure it, you need to repeat the test a million times to make a good measurement.  In each loop, the strcpy resets s such that it has a copy of the ith argument so that it can be reversed.  The time that the process takes is test_t2 - test_t1.  
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

After timing the reverse_string call, printf is used to print the string on the terminal before it was reversed and the new form of the string (s).  Printf allows for format specifiers to match arguments after the first parameter (also known as the format string).  %s indicates that there must be a string for the given argument.  %0.4f expects a floating point number and prints 4 decimal places.  test_t2 and test_t1 are both measured in microseconds.  Multiplying the difference by 1000 will change the unit type to nanoseconds.  Since the test was repeated 1 million times, the overall time needs divided by 1 million.  By multiplying or dividing a number by a decimal, it converts the type to a decimal.

```c
    printf("%s => %s\n", argv[i], s);
    printf( "time_spent: %0.4fns\n", ((test_t2-test_t1)*1000.0)/(repeat_test*1.0));
    ...

  printf( "overall time_spent: %0.4fns\n", overall_time*1000.0/(repeat_test*1.0));
```

The last function to examine is the reverse_string call.
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
1.  set a pointer e to the last character in the string.  This is found by
    determining the length of the string s and pointing to the length-1
    char beyond s.
2.  while s is less than e
      swap the value that is pointed to by s and e.
      advance s by one and decrement e by one.
```

Imagine the string "Reverse".  The strlen or length of "Reverse" is 7 because
if you were to subtract the pointer just after the string from the pointer that
points to the beginning of the string, you will get 7.
```
01234567
Reverse
^      ^
s      e
```

To reverse the string, we need the end pointer to point to the last character 'e'.
```
01234567
Reverse
^     ^
s     e
```

The first step is to swap R and e.  In order to swap R and e, a temporary
variable is needed.
```c
char tmp = *s;  // *s == R
*s = *e;        // *e == e, so now string is equal to eeverse
*e = tmp;       // tmp = R, so now string is equal to eeversR
s++;            // s points to the second e
e--;            // e points to s
```

The while loop will continue because s is less than e.
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

The while loop will continue because s is less than e.
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

The while loop will NOT continue because s is not less than e.
```
01234567
esvereR
   ^
   s
   e
```

The string is now esreveR which is the reverse of Reverse.

# The basic Makefile

If you change to that directory, you will find the following Makefile.  

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
```bash
make
```

The first block with a colon will be run.  In this Makefile, it is the following line.
```Makefile
all: test_timer examples
```

The all group simply refers to other groups to be built.  In this case it is test_timer and examples.
```Makefile
test_timer: test_timer.c
	gcc test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
```

The lines of code after test_timer will run if the file called test_timer is older than the files after the colon.  If you edit test_timer.c, the program will be built using the following line of code.  The output is test_timer.
```Makefile
gcc test_timer.c -o test_timer
```

If you didn't want to use a Makefile to build test_timer, you could do so from the command line using the following command.

```bash
gcc test_timer.c -o test_timer
```

The examples block will run everytime because it doesn't have any dependencies and examples isn't a file that exists.  If you were to create a file called examples, then the examples block would cease to run.  By running <b>make</b>, you will effectively build test_timer if it needs built and run the examples block.  Running <b>make clean</b> will clean up the binary.  You can run any block by specifying it.  <b>make all</b> is equivalent to running <b>make</b>.  If you just want to run the examples block, you can by running <b>make examples</b>.  

# Doing a better job of timing

In the last section, we explored how to time the reverse_string function.  In this section, we will explore how to better time the function.  One thing you may have noticed is that there is a million calls to both reverse_string and strcpy.  There is also the overhead of the loop.  To do the timing properly, we should have timed the strcpy and the loop and subtracted that from the loop which has the reverse_string function called.

The timing for the work in <i>illustrations/2_timing/2_timer</i> was...
```bash
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 46.4650ns
Reverse => esreveR
time_spent: 20.5820ns
overall time_spent: 67.0470ns
```

This section's code is found in <i>illustrations/2_timing/2_timer</i>

```bash
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 35.8950ns
Reverse => esreveR
time_spent: 13.7250ns
overall time_spent: 49.6200ns
```

Running make yields a 17.5 nanosecond improvement.  The difference between 1_timer and 2_timer can be found by running...

```bash
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

The less than symbols mean that the change is in the file in the first parameter (test_timer.c).  The greater than symbol indicates that the change is in the file in the second parameter (../1_timer/test_timer.c).  You should also notice three dashes (---) between the two lines indicating time_spent.  The 30,35d29 means that the lines were added after line 29 as lines 30-35.  The 42c36 means that line 42 in test_timer.c was compared with line 36 in ../1_timer/test_timer.c.

As you can see from above, there are two changes.  The first times everything but the reverse_string.
```c
  long copy_t1 = get_time();
  for( int j=0; j<repeat_test; j++ ) {
     strcpy(s, argv[i]);
  }
  long copy_t2 = get_time();
```

The second change subtracts the time spent doing everything but the reverse string from the reverse string loop.
```c
long time_spent = (test_t2-test_t1) - (copy_t2-copy_t1);
```

# Doing a better job of timing continued

In the last section, we eliminated the cost of the strcpy and loop from the timing.  Another thing to do is to reconsider our reverse_string function.  The reverse_string calls strlen to get the length of the string s.  We could try and just pass the length of s into the call.  We can get the length of the argument outside of the repeated and timed test.  For completeness, we will compare the timing of 2_timer with 3_timer.

The timing for the work in <i>illustrations/2_timing/2_timer</i> was...
```bash
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 37.4880ns
Reverse => esreveR
time_spent: 12.5070ns
overall time_spent: 49.9950ns
```

This section's code is found in <i>illustrations/2_timing/3_timer</i>
```bash
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 28.8140ns
Reverse => esreveR
time_spent: 6.5320ns
overall time_spent: 35.3460ns
```

Running make yields a 14.5 nanosecond improvement.  The difference between 2_timer and 3_timer can be found by running...
```bash
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

Instead of getting the string length of s in every call, reverse_string now expects the length of the string to be passed into it.

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

The length is passed into the reverse_string call so that reverse_string doesn't have to calculate it.  This optimization yielded another 14.5 nanoseconds.

# Compiler optimizations

It's important to not forget that the compiler can optimize the code further.  You can pass a flag called -O3 to gcc and sometimes see an improvement.

The timing for the work in <i>illustrations/2_timing/3_timer</i> was...
```bash
$ make
gcc test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 28.8140ns
Reverse => esreveR
time_spent: 6.5320ns
overall time_spent: 35.3460ns
```

This section's code is found in <i>illustrations/2_timing/4_timer</i>
```bash
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
```bash
$ diff . ../3_timer/
diff ./Makefile ../3_timer/Makefile
4c4
< 	gcc -O3 test_timer.c -o test_timer
---
> 	gcc test_timer.c -o test_timer
```

The -O3 optimization (full optimization) was turned on in gcc.  It is good to actually compare time with optimizations turned on because sometimes the fastest code is slower once optimizations are turned on.

# Splitting up your code into multiple files

This section's code is found in <i>illustrations/2_timing/5_timer</i>

If you run diff -q . ../4_timer/, a brief summary of what changed will be displayed.
```bash
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

The compiler basically inserts the contents of file2.c into test_timer.c.  At this point, if there was another source file that wanted to use these functions, all they would have to do is #include "file2.c".  For example...

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

```bash
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

As you can see there are lots of errors.  It's generally best to work through errors starting from the first one and working down as the first error may cause other errors to occur.

The first error is
```
In file included from test_reverse.c:1:
./file2.c:1:31: error: unknown type name 'size_t'
void reverse_string( char *s, size_t len ) {
```

The error indicates that the type size_t is not known.  Use your favorite search engine and query "error: unknown type name 'size_t'".  You should be able to find that it is defined in stdio.h, so the solution is to include stdio.h.  Notice that we actually do include stdio.h in test_reverse.c, but it is after we include file2.c.  Ideally, file2.c would be complete, so let's modify file2.c to include stdio.h and rerun gcc.  You will have to add the following line to the beginning of file2.c.

```c
#include <stdio.h>
```

and then run gcc again.
```bash
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

NULL was also defined in stdio.h, so by fixing the first error, we actually fixed another error.  This is a big reason that I fix errors from the beginning to the end.  Often the fixes, fix other problems.

The next error is
```c
In file included from test_reverse.c:1:
./file2.c:15:18: error: variable has incomplete type 'struct timeval'
  struct timeval tv;
```

Again, use your favorite search engine and query "error: variable has incomplete type 'struct timeval'".  I found the answer was to #include <sys/time.h> on stack overflow.  Again, let's add this to file2.c right after the #include <stdio.h> call.

Add this right after #include <stdio.h> to file2.c
```c
#include <sys/time.h>
```

Again, run gcc to check for more errors
```bash
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

Again, by fixing the one error, we actually fixed more.  Let's continue with the next warning.
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

We don't have to recompile everytime.  We can go ahead and look at the next error and try and fix more than one.
```
test_reverse.c:9:21: error: too few arguments to function call, expected 2, have 1
    reverse_string(s);
    ~~~~~~~~~~~~~~  ^
./file2.c:4:1: note: 'reverse_string' declared here
void reverse_string( char *s, size_t len ) {
^
```

Again, gcc is helping us by pointing out that reverse_string requires two arguments and what they should be.  I forgot to pass the length of the string as the second argument.  We can do that with strlen.

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

Now if you run gcc...
```bash
$ gcc test_reverse.c -o test_reverse
$
```

All of the errors and warnings are gone.  

We can run test_reverse like...
```bash
$ ./test_reverse This is a test
This => sihT
is => si
a => a
test => tset
```

If you were following along and changing file2.c, you can run make again to see if the changes work in the original test_timer.

```bash
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

This change is helpful because it says that the test_timer block should run if any of the files have changed.  Now that test_timer is made up of two files, we should list both files.  Internally, make simply looks at the time stamp of test_timer and of test_timer.c and file2.c.  If test_timer.c or file2.c has a time stamp that is greater than test_timer (or test_timer doesn't exist), then the block will be run again.

# Splitting up your code into multiple files part 2

This section's code is found in <i>illustrations/2_timing/6_timer</i>

It is generally a good idea to make each file or group of files self-contained.  In the last section, we didn't notice errors when running make, but we did when trying to build test_reverse.  The errors didn't occur because prior to including file2.c we had already included stdio.h and sys/time.h.  In general, you want to include your custom code first as this will present the error earlier.  We should make the following change to test_timer.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "file2.c"
```

change to
```c
#include "file2.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
```

If you go back and change test_timer.c in <i>illustrations/2_timing/5_timer</i> and then run make, you will get some of the same errors that we had when building test_reverse.

in illustrations/2_timing/5_timer after making above change
```bash
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
1 warning and 3 errors generated.
make: *** [test_timer] Error 1
```

If we make the change in <i>illustrations/2_timing/6_timer</i> and run make
```bash
$ make
gcc -O3 test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 7.6000ns
Reverse => esreveR
time_spent: 1.7670ns
overall time_spent: 9.3670ns
```

In this section, I also removed the following two lines from test_timer.c as test_timer.c doesn't directly call any time related functions.

```c
#include <sys/time.h>
#include <time.h>
```

It all works because we have fixed file2.c to have the right include statements.  Particularly if you are developing a new package, it is a good idea to include your own packages before outside or system packages.

# Separating the implementation from the interface

This section's code is found in <i>illustrations/2_timing/7_timer</i>

The key to writing large software projects that work is to clearly define objects and make them highly reusable.  When defining objects, it is best to split the object into an interface and an implementation.  Ideally, the interface will only describe what the object does and not how it works.  This is much like an automobile and a transmission.  An automobile might require a transmission with certain specifications.  That automobile doesn't care how the transmission is made.  An interface is like the specifications to a transmission.  The implementation would include how the transmission actually works.  C allows you to declare that a thing exists prior to it being defined.  Typically, this is done in header files (files that end in a .h extension), but it can be done anywhere.  In this section, we will show how to declare that the functions in file2.c without including them.

In test_timer.c
```c
#include "file2.c"
```

changes to
```c
void reverse_string( char *s, size_t len );
long get_time();
```

Declaring the functions this way, lets the compiler know that the functions do exist.  The syntax is the same as writing the function, except you replace the {} with a semicolon.

If you run make...
```bash
$ make
gcc -O3 test_timer.c -o test_timer
Undefined symbols for architecture x86_64:
  "_get_time", referenced from:
      _main in test_timer-276d5b.o
  "_reverse_string", referenced from:
      _main in test_timer-276d5b.o
ld: symbol(s) not found for architecture x86_64
```

This happens because we have defined the functions, but they ultimately don't make it into the binary.  To fix this, change the following line in Makefile.

```Makefile
gcc -O3 test_timer.c -o test_timer
```

to (add file2.c to the gcc line)
```Makefile
gcc -O3 file2.c test_timer.c -o test_timer
```

Save and run make again
```bash
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

# Separating the implementation from the interface (part 2)

This section's code is found in <i>illustrations/2_timing/8_timer</i>

Ideally, file2.c should have a corresponding file2.h that describes what exists in file2.c.  We can put the definition of get_time and reverse_string into file2.h as follows...

file2.h:
```c
#ifndef _file2_H
#define _file2_H

#include <stdlib.h>

void reverse_string( char *s, size_t len );
long get_time();

#endif
```

The #ifndef, #define, #endif block basically prevents the compiler from including the same code over and over again.  It checks to see if _file2_H has been defined and if it hasn't, it defines it.  If it has been defined previously, the block is skipped by the compiler.

size_t is used in reverse_string which is defined in stdlib.h, so we include stdlib.h.  One last change is to update file2.c to include file2.h so that we can be sure that file2.c and file2.h don't get out of sync.

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

Again, since this is something that is our own, we should include it before system or third party includes.  The Makefile changed slightly

Makefile changed from
```Makefile
test_timer: test_timer.c file2.c
```

to
```Makefile
test_timer: test_timer.c file2.h file2.c
```

So that test_timer will be built if file2.h is updated.

# Defining an object

This section's code is found in <i>illustrations/2_timing/9_xxx</i>

To build the code run...
```bash
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

The header file or interface simply provides what exists and doesn't offer details into how xxx might work.  The int param1, int param2 and const char *prefix above are just examples.  The init function often doesn't have any parameters.  The init function is used to create the structure so that it can be used by the other functions within the xxx object.  The destroy function destroys the xxx_t structure.  The xxx_t structure doesn't have members.  It is just declared (like the functions after it), so that the compiler knows that the structure is defined somewhere.  By defining the structure like this, applications using the xxx object cannot access members of the structure.  The implementation is free to put whatever members in the structure to make the function work.  The only thing applications which use this object are required to do is create (or init) the object and then pass it around to the various functions that use that type.  If there is a destroy method, then the application is expected to destroy the object to clean it up.

To use the above interface, you will include it and call it's methods.
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

In C, you can group multiple data types as variables together to define a larger structure.  A classic example is to create a point structure.  Typically, they would be defined as follows...
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



# Defining the timer interface

The following code is found in <i>illustrations/2_timing/10_timer</i>

The Makefile has one minor difference.  This project will have a separate timer object.  The gcc command will run if test_timer.c, timer.c, or timer.h are changed.
```Makefile
test_timer: test_timer.c timer.c timer.h
	gcc -O3 timer.c test_timer.c -o test_timer
```

Build the project...
```bash
$ make
gcc -O3 timer.c test_timer.c -o test_timer
./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse
ABCDEFGHIJKLMNOPQRSTUVWXYZ => ZYXWVUTSRQPONMLKJIHGFEDCBA
time_spent: 9.7730ns
Reverse => esreveR
time_spent: 2.5150ns
overall time_spent: 12.2880ns
```

The timing is the same as 4_timer.  This section will be about

In test_timer.c the following lines of code exists.
```c
#include "timer.h"
```

In order to make our project reusable, we need to break it up into objects (or interfaces and implementations).  In the above example, we are including timer.h (which we will make next).  

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

Earlier you learned that the struct keyword allows you to group zero or more types together to form a new type.  In general, I like to name struct types with a _s suffix and then typedef them to have a _t suffix.  In the timer.h above, the timer_s struct was declared, but never actually defined.  The details of what is in the structure is part of the implementation and isn't meant to be known externally.  C allows you to define types in this way and use them as long as you only reference them as pointers.  All pointers have the same size (the number of bits that the cpu supports or the sizeof(size_t)).  For now, just recognize that there is a new type named timer_t and that timer_s will be defined in timer.c.

```c
struct timer_s;
typedef struct timer_s timer_t;
```

I follow a pattern where every function is prefixed by the object name (in this case timer).  The primary type (if there is one) is usually the object name followed by _t.  Objects will typically have an init and a destroy method.  The job of the header file is to create an interface for applications to use.  It should hide implementation details as much as possible.  I usually will define an interface before defining an implementation.

Initialize the timer.  repeat is necessary to indicate how many times the test will be repeated within the application so that the final result represents that.  If a thing is only being timed with a single repetition, then use a value of 1.  This function will allocate the timer_t structure and fill its members appropriately.  To free up the resources associated with this call, you must call timer_destroy with the return value of this call.
```c
timer_t *timer_init(int repeat);
```

Initialize a timer from another timer.  This will subtract the time spent and set the repeat from the timer which is passed as a parameter to timer_timer_init.
```c
timer_t *timer_timer_init(timer_t *t);
```

Destroy the timer created from timer_init or timer_timer_init.
```c
void timer_destroy(timer_t *t);
```

Subtract the amount of time spent in sub from the current timer (t)
```c
void timer_subtract(timer_t *t, timer_t *sub);
```

Add the amount of time spent in add to the current timer (t)
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

Notice that timer.h doesn't mention how it is going to be implemented in any way.  It simply names functions in a way that are useable and describes what they do.

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

The first thing to do is to remove the #ifndef/#define/#endif and replace it with an #include "timer.h".

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

The next thing to do is to fill in the the timer_s details and remove the typedef that follows.

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

repeat is the number of times that the test will be repeated.  time_spent is the total time that has been spent within this object so far.  start_time is recorded everytime timer_start is called.  base is used to track how much time should be added or subtracted from the final time spent based upon other timers.

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

#include <stdlib.h> is added at the top of the file since it is needed for malloc.  All of the members are initialized to zero except repeat (which is set to the value passed into timer_init).


```c
void timer_destroy(timer_t *t);
```

becomes
```c
void timer_destroy(timer_t *t) {
  free(t);
}
```

timer_destroy simply needs to free the memory that was allocated by either of the init methods.

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
Because the details of the timer_t structure are only known to timer.c, functions must be used to access members of the structure.  This encapsulation keeps a good separation from the interface and the implementation.

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

These methods may also have been initialized from another timer, so the base is added to the time_spent.  

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

timer_start is basically the same as get_time() defined earlier.  The difference is that it sets the start_time member of the struct timer_t t.

Finally, the following functions are changed to.
```c
double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);
```

to
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

The member time_spent and base are in microseconds.  Each function above does the appropriate conversions.


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
```s
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
```

We include the interface (or header file) we just created in the first line.  The rest of the code above has already been discussed.  

To understand the rest of the code, it is helpful to consider what it looked like prior to using the timer object.

Consider the following code from <i>illustrations/2_timing/8_timer</i>
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

First, it is important to recognize that the new code is actually longer.  The number of lines of code isn't always a good measure to decide to create an object.  In the case of the timer object, the get_time() function is hidden in the timer object.  

Instead of just having a counter for the overall time, I've replaced it with an timer object named overall_timer.
```c
long overall_time = 0;
```

becomes
```c
timer_t *overall_timer = timer_init(repeat_test);
```

In order to measure the time to copy, we will need a copy timer and start and stop it.
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

The timer_init call gets the repeat_test variable from the overall_timer since they should all be the same.  The timer_init call could have just been
```c
timer_t *copy_timer = timer_init(repeat_test);
```

but that would require that the repeat_test variable be separately.  This becomes important in more complex timings, but for now I'm primarily showing how timer_get_repeat can be useful.  In C, parameters to functions can be the result of other functions.  The timer_get_repeat call would be resolved prior to calling timer_init.

To find the time spent in the first example, you need to subtract copy_t1 from copy_t2 and then divide that by the repeat factor.  This will return the number of microseconds that elapsed.  The timer_us(copy_timer) would yield the same result.  With the timer object, it is easy to get the time spent in microseconds, but also nanoseconds, milliseconds, and seconds.  In addition, the timer object automatically converts the time spent to a double (a decimal).


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

The next two lines begin to show additional usefulness of the timer object.  The copy_timer is subtracted from the test_timer and then the test_timer is added to the overall_timer.  
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

# Making the timer object reusable

The following code is found in <i>illustrations/2_timing/11_timer</i>

This timer object is done and is ready to be reused.  In C, all of your functions share the same name space.  If another project has a function named timer_init, there will be a conflict.  To prevent this, projects typically adopt a package prefix in addition to the object prefix.  For this project, we will use stla (standard template library alternative).

My rules for adding the prefix are...
```
1.  Comments should refer to the object name only.
2.  All code references should be prefixed.
3.  Functions should only use a prefix once even if multiple objects are
    referenced in the name
```

The first thing to do is to copy timer.h to stla_timer.h and then apply the rules mentioned above.  The stla_timer header file exists in the current directory what follows is a partial diff.

```
$ diff timer.h stla_timer.h
1,2c1,2
< #ifndef _timer_H
< #define _timer_H
---
> #ifndef _stla_timer_H
> #define _stla_timer_H
4,5c4,5
< struct timer_s;
< typedef struct timer_s timer_t;
---
> struct stla_timer_s;
> typedef struct stla_timer_s stla_timer_t;
12c12
< timer_t *timer_init(int repeat);
---
> stla_timer_t *stla_timer_init(int repeat);
...
```

The same thing is done for timer.c (copy to stla_timer.c and apply rules).
```bash
$ diff timer.c stla_timer.c
1c1
< #include "timer.h"
---
> #include "stla_timer.h"
7c7
< struct timer_s {
---
> struct stla_timer_s {
14,15c14,15
< timer_t *timer_init(int repeat) {
<   timer_t *t = (timer_t *)malloc(sizeof(timer_t));
---
> stla_timer_t *stla_timer_init(int repeat) {
>   stla_timer_t *t = (stla_timer_t *)malloc(sizeof(stla_timer_t));
21c21
...
```

Finally, the test_timer.c needs to change to use stla_timer instead of timer.

```c
#include "stla_timer.h"
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
stla_timer_t *overall_timer = stla_timer_init(repeat_test);
```

and so on.

# Moving stla_timer to src (and variables in Makefile)
## splitting up a project into multiple directories

The following code is found in <i>illustrations/2_timing/12_timer</i>

Once the object has the stla prefix, we can move it to the src directory.  This is done by executing the following command - this actually won't work as it already has been moved.

This command will not work because it was done for you.
```bash
mv stla_timer.h stla_timer.c $stla/src
```

You can see that the files are there by running
```bash
cd $stla/src
ls -l stla_timer.*
```

which will output
```bash
-rw-r--r--  1 ac  staff  1777 Sep 11 12:12 stla_timer.c
-rw-r--r--  1 ac  staff  1314 Sep 11 12:12 stla_timer.h
```

You can change back to the previous directory (illustrations/2_timing/12_timer) by running
```bash
cd -
```

or
```bash
cd $stla/illustrations/2_timing/12_timer
```

cd - allows you to change to the directory that you were in previously and is useful.

To make the program build properly, there are a few changes to Makefile that are needed.  The diff looks like the following.

```bash
$ diff Makefile ../11_timer/Makefile
1,5d0
< ROOT=../../..
< OBJECTS=$(ROOT)/src/stla_timer.c
< HEADER_FILES=$(ROOT)/src/stla_timer.h
< FLAGS=-O3 -I$(ROOT)/src
<
8,9c3,4
< test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
< 	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer
---
> test_timer: test_timer.c stla_timer.c stla_timer.h
> 	gcc -O3 stla_timer.c test_timer.c -o test_timer
```
A new section of variables are added at the top and the test_timer target use those variables.

The Makefile for this project.
```Makefile
ROOT=../../..
OBJECTS=$(ROOT)/src/stla_timer.c
HEADER_FILES=$(ROOT)/src/stla_timer.h
FLAGS=-O3 -I$(ROOT)/src

all: test_timer examples

test_timer: test_timer.c $(OBJECTS) $(HEADER_FILES)
	gcc $(FLAGS) $(OBJECTS) test_timer.c -o test_timer

examples:
	./test_timer ABCDEFGHIJKLMNOPQRSTUVWXYZ Reverse

clean:
	rm -f test_timer *~
```

make allows you to create variables using the <name>=<value> syntax outside of sections.  You can then reference the value of those variables by enclosing the name in $(<name>).  One variable can reference another variable (see OBJECTS and ROOT above).  In order for gcc to find the stla_timer.h file, the src path needs to be added to gcc's include path.  That is done by using the -I<directory> option.  If you have multiple include paths, you can specify -I<directory> multiple times.  

# Splitting up the Makefile

The following code is found in <i>illustrations/2_timing/13_timer</i>

Ideally, the objects in src could be defined by a Makefile in src and that Makefile could be included.  In src, there is a file named Makefile.include which defines the variables that were in the Makefile in the last section (as well as for other objects in src).

Makefile.include in src
```Makefile
OBJECTS=$(ROOT)/src/stla_timer.c $(ROOT)/src/stla_buffer.c $(ROOT)/src/stla_pool.c
HEADER_FILES=$(ROOT)/src/stla_timer.h $(ROOT)/src/stla_buffer.h $(ROOT)/src/stla_pool.h
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

# Continue to create our second object [Buffer](3_buffer.md)!
