# Getting Started

To get started, you will need to clone the project.   

```bash
git clone https://github.com/contactandyc/standard-template-library-alternative-in-c.git
```

In the project, there is an illustrations folder which contains most of the code.  There is also a src directory where final code is placed.  Normally, one would start with a hello world project.  That actually exists in the second chapter, so you may find it easier to jump ahead and then come back.  The code for this chapter is located in <i>illustrations/0_getting_started</i>

At various points in this project, we will be timing code in an attempt to optimize it.  Our first object is going to be simple, but will illustrate how I plan to maintain separation between interfaces and their respective implementation.

The following code is found in <i>illustrations/0_getting_started/1_timer</i>

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

Every good c program that is capable of being executed has a main function.  The main function is typically implemented with the following two parameters to allow command line arguments to be passed to the program.  argc represents the number of arguments (the name of the program is the 1st argument).  argv represents the arguments.  argv[0] references the name of the program as called from the command line.  The main function returns an integer.  If the program executes successfully, it should return 0 which lets the command shell know that the program finished normally.  

```c
int main( int argc, char *argv[] ) {
  ...
  return 0;
}
```

C has a few built-in data types.

```
char   - a single byte
short  - two bytes
int    - four bytes
long   - typically eight bytes
```

A byte is represented by 8 bits.  The range of 1 bit would be 0-1, two bits 0-3, and so on.  For 8 bits, the range is 0-255.  The short has a range of 0-((256*256)-1) or 0-65535.  C counts from 0 (all bits off) instead of 1.  In addition to this these types can be signed (the default) or unsigned.  If the data type is unsigned, the number range will start with zero.  Otherwise, the number range will begin -(2^(number of bits-1)) to (2^(number of bits-1))-1.  A signed char will range from -128 to 127.  A signed char and a char are the same thing.

Variables are typically declared as
```c
datatype variable_name = value;
```

or
```c
datatype variable_name;
```

In the main function declaration, argv is declared in a unique way.

```c
char *argv[];
```

The asterisk means that the variable named argv is a pointer to the type char which is a single byte and is signed (signed is the default datatype prefix).  The [] after the argv indicates that argv is referencing an array of pointers.

In the following array
```
0123456789012345678901234567
./test_timer This is a test
```

argv would be an array of pointers<br/>
argv[0] => . (at byte 0)<br/>
argv[1] => T (at byte 13)<br/>
argv[2] => i (at byte 18)<br/>
argv[3] => a (at byte 21)<br/>
argv[4] => t (at byte 23)<br/>
argv[5] => NULL (pointing to NULL means that the pointer is not pointing at anything)<br/>

Notice that the pointers refer to individual bytes.  In C (and any language), a pointer refers to a location in memory (typically RAM or random access memory).  Strings or sequences of characters are defined by looking for a terminating character (a non-printable 0).  The above example is not technically correct in that what would really happen is prior to the arguments getting to the main function, they would be split into 5 strings.

argv would be an array of pointers<br/>
argv[0] => "./test_timer"<br/>
argv[1] => "This"<br/>
argv[2] => "is"<br/>
argv[3] => "a"<br/>
argv[4] => "test"<br/>
argv[5] => NULL (pointing to NULL means that the pointer is not pointing at anything)<br/>

The string "This" above is 4 bytes long but requires 5 bytes to be represented.  To determine the length of a string in C, you would advance a pointer until the zero is referenced.  A simple implementation to find the length of a string might look like...

```c
int strlen(char *s) {
  char *p = s;
  while(*p != 0)
    p++;
  return p-s;
}
```

This function returns an integer (so it will only support finding the length of strings that are 2,147,483,647 or less bytes).  The function's name is strlen.  It takes as an input a single pointer to a byte in RAM.  The pointer is of type char.  Imagine that s pointed to the first character in the string "This" (or the letter T).  This functions seeks to find a pointer to the byte that is just past the end of the string (or just after the letter s) and then subtracts the two pointers.

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

Pointers always reference specific bytes in RAM.  In most languages the pointer is defined to reference a type.  The purpose of the type so that when the pointer is incremented, subtracted, added, etc, that the appropriate number of bytes would be advanced.  A <i>char *</i> is a pointer which references a sequence of bytes which are of type char.  The char is a single byte, so advancing a pointer to a char by one is the same as incrementing the pointer by 1 byte.

Make a copy of the pointer s and call it p.
```c
char *p = s;
```


```c
while(*p != 0)
  p++;
```

C allows for logic to be repeated until a condition is met.  A while loop is technically defined as

```c
while(<condition>)
  do_something;
```

or if there are multiple lines of code which need executed within the loop
```c
while(<condition>) {
  do_something1;
  do_something2;
  ...
  do_somethingN;
}
```

It is important to recognize the difference between comparisons and conditions.  A condition can be true or false.  Comparisons requires two objects and can evaluate to true or false.  In most (every?) computer language, loops and if logic will use conditions as opposed to comparisons.  Comparisons are a subset of conditions.  In C, true is non-zero and false is zero.

This would never do_something.
```c
while(0) {
  do_something;
}
```

This would never stop executing do_something.
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

Pointers are declared using an asterisk.  The value of what the pointer is pointing at is obtained through a process called dereferencing which is done by placing an asterisk before the pointer.  The first time through this loop, assuming that s pointed to "Test", p would be pointing at a value T.  *p would result in the single character 'T'.  In C, single characters are defined in single quotes.  Longer strings are defined in double quotes.  

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

At this point, p points to just passed the letter s and has been advanced 4 times so the length is 4.

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

The coding example is going to reverse strings a number of times and time the process.  The arguments that are passed into your program should not be modified by the program.  If you wish to modify an argument, you should first allocate memory for your program to use and then copy the argument into the newly allocated memory.  The malloc function will allocate the number of bytes requested for use.  You can read about it by typing "man malloc" from the command line.  Programs which need extra memory to work with and must request that memory from the operating system.  malloc is one of the core ways that that is done.  Memory requested should later be freed using the <i>free</i> call.  The malloc function can return NULL meaning that the memory was not available.  Trying to writing to a pointer which is pointing at NULL will cause your program to crash.  You can check for the error or just allow the program to not so gracefully crash.  In my examples, I'm going to simply allow the program to crash if NULL is returned.  The only other reasonable option would be to have the program fail early which effectively is the same thing.  In the example below, there are a few functions called.  The early writers of C decided to shorten the names of the funtions.

```
malloc - memory allocate
strlen - string length
strcpy - string copy
```

Once s is pointing at newly allocated memory which is the string length of argv[i] (the current iteration of the loop), a string copy of the argument is performed so that s points to a copy of the given argument.

```c
for( int i=1; i<argc; i++ ) {
  char *s = (char *)malloc(strlen(argv[i])+1);
  ...
  strcpy(s, argv[i]);
  ...
  free(s);
```

At the beginning of the program, you may have noticed a few #include statements.  #include effectively copies the contents of the filename into the current program.  In order to use functions like malloc, strcpy, and free, the proper file must be included.  Files with a suffix ".h" are called header files and typically define how to call a function.  

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

A quick note on using parenthesis.  I strongly believe in making code easier to read.  Technically, I could have skipped the parenthesis without harm due to order of operations, but the code becomes more readable by adding the parenthesis.

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
overall_time += (test_t2-test_t1);
```

After timing the reverse_string call, printf is used to print the string before it was reversed and the new form of the string (s).  Printf allows for format specifiers to match arguments after the first parameter (also known as the format string).  %s indicates that there must be a string for the given argument.  %0.4f expects a floating point number and prints 4 decimal places.  test_t2 and test_t1 are both measured in microseconds.  Multiplying the difference by 1000 will change the unit type to nanoseconds.  Since the test was repeated 1 million times, the overall time needs divided by 1 million.  By multiplying or dividing a number by a decimal, it converts the type to a decimal.

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

The lines of code after test_data_structure will run if the file called test_data_structure is older than the files after the colon.  If you edit test_timer.c, the program will be built using the following line of code.  The output is test_timer.
```Makefile
gcc test_timer.c -o test_timer
```

If you didn't want to use a Makefile to build test_data_structure, you could do so from the command line using the following command.

```bash
gcc test_timer.c -o test_timer
```

The examples block will run everytime because it doesn't have any dependencies and examples isn't a file that exists.  If you were to create a file called examples, then the examples block would cease to run.  By running <b>make</b>, you will effectively build test_timer if it needs built and run the examples block.  Running <b>make clean</b> will clean up the binary.  You can run any block by specifying it.  <b>make all</b> is equivalent to running <b>make</b>.  If you just want to run the examples block, you can by running <b>make examples</b>.  



# Defining the timer interface

The following code is found in <i>illustrations/0_getting_started/2_timer</i>

The Makefile has one minor difference.  This project will have a separate timer object.  The gcc command will run if test_timer.c, timer.c, or timer.h are changed.
```Makefile
test_timer: test_timer.c timer.c timer.h
	gcc timer.c test_timer.c -o test_timer
```

Run <b>make</b> to build this project and run examples.

timer.h
```c
#ifndef _timer_H
#define _timer_H

struct timer_s;
typedef struct timer_s timer_t;

/*
   Initialize the timer.  repeat is necessary to indicate how many times the
   test was repeated so that the final result represents that.  If a thing is
   only being timed with a single repetition, then use a value of 1.
*/
timer_t *timer_init(int repeat);

/*
   Initialize a timer from another timer.  This will subtract the time spent
   from the other timer and set the repeat to match the other timer.
*/
timer_t *timer_timer_init(timer_t *t);

/* destroy the timer */
void timer_destroy(timer_t *t);

/* Subtract the amount of time spent in sub from the current timer */
void timer_subtract(timer_t *t, timer_t *sub);
/* Add the amount of time spent in add to the current timer */
void timer_add(timer_t *t, timer_t *add);

/* start the timer */
void timer_start(timer_t *t);

/* end the timer */
void timer_end(timer_t *t);

/*
  Returns time spent in nanoseconds (ns), microseconds (us), milliseconds (ms),
  and seconds (sec).
*/
double timer_ns(timer_t *t);
double timer_us(timer_t *t);
double timer_ms(timer_t *t);
double timer_sec(timer_t *t);

#endif
```
