# Getting Started

To get started, you will need to clone the project.   

```bash
git clone https://github.com/contactandyc/standard-template-library-alternative-in-c.git
```

In the project, there is an illustrations folder which contains most of the code.  There is also a src directory where final code is placed.  Normally, one would start with a hello world project.  That actually exists in the second chapter, so you may find it easier to jump ahead and then come back.  The code for this chapter is located in <i>illustrations/0_getting_started</i>

At various points in this project, we will be timing code in an attempt to optimize it.  Our first object is going to be simple, but will illustrate how I plan to maintain separation between interfaces and their respective implementation.

The following code is found in <i>illustrations/0_getting_started/1_timer</i>
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
size_t - on a 64 bit system, 8 bytes (or 64 bits), on a 32 bit system,
         4 bytes (or 32 bits).
ssize_t - signed size_t
bool   - can be different sizes, but only has two states (true and false)
float  - four byte decimal (I try to avoid this type as it quickly loses
         precision)
double - eight byte decimal
void   - this doesn't have a size and is a special type
```

A byte is represented by 8 bits.  The range of 1 bit would be 0-1, two bits 0-3, and so on.  For 8 bits, the range is 0-255.  The short has a range of 0-((256*256)-1) or 0-65535.  C counts from 0 (all bits off) instead of 1.  In addition to this these types can be signed (the default) or unsigned.  If the data type is unsigned, the number range will start with zero.  Otherwise, the number range will begin -(2^(number of bits-1)) to (2^(number of bits-1))-1.  A signed char will range from -128 to 127.  A signed char and a char are the same thing.

size_t is a type that is defined in C to represent the number of bits that the cpu is and is unsigned (meaning it can't be negative).  ssize_t is a signed alternative.  On a 64 bit system, a long and ssize_t are equivalent.

You can overflow a type.

```c
unsigned char a = 255;
a = a + 1;  // a will become zero
a = a - 1;  // a will become 255 again
char b = 127;
b = b + 1; // b will become -128
b = b - 1; // b will become 127 again
```

bool is another type that is defined in C (you must include stdbool.h to get it).  bool is defined as having the value true or false.  This type can make code more readable in that it makes it clear to the reader that there is only two possible states.

double (and float) are used for decimals.  Because float only uses 32 bits, it loses precision quite easily.  I don't tend to use it for anything.  Instead I opt to use double.

In C you can define your own type using typedef using the following syntax.

```
typedef <existing type> <new type name>;
```

for example, the following would define a new type called number_t which has an underlying type of unsigned int.
```c
typedef unsigned int number_t;

number_t a = 100;
```

A variable can be cast from one type to another either implicitly or explicitly.  Imagine you want to convert an int to a double or vice versa.

```c
int x = 100;
double y = x;
y = y + 0.05;
x = y;
printf( "%d\n", x ); // would print 100
```

When the casting happens, precision is lost if the new type can't accommodate for the value.  The above example shows casting happening implicitly.  Below is an example of casting happening explicitly.

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

It is common for developers to use int as a return type, however it is generally more efficient and less bug prone to use size_t.  In order for a 64 bit CPU to work with an int, it must split a register since the CPU is meant to work with 64 bit integers.  This split isn't cheap.  In addition, if you use size_t or (ssize_t for signed numbers), the program will be more portable to 64 bit systems where a string might be longer than 2 billion bytes.  A better implementation of strlen might look like the following.  The only difference is that the strlen returns a type size_t.  Strings can't be negative in length, so returning an unsigned number also helps people using the function to understand that.
```c
size_t strlen(char *s) {
  char *p = s;
  while(*p != 0)
    p++;
  return p-s;
}
```

C allows for variables to be declared as constant meaning that they can't change.  This is particularly useful in functions as the function can indicate that the input will not change.  In the above example, the string is not modified in determining the length.  Adding const will indicate to the user (and compiler) that what s points to will not be changed.
```c
size_t strlen(const char *s) {
  const char *p = s; // p must also be declared with const
  while(*p != 0)
    p++;
  return p-s;
}
```

Another minor optimization to the function above is to look at the while loop.  while expects a condition.

```c
while(*p != 0)
```

can be changed to
```c
while(*p)
```

As the check for 0 is redundant.  It doesn't hurt anything to add the != 0 and sometimes it makes code easier to read.

## The void type

A function with a return type of void means that the function doesn't expect to return anything. An example might be...
```c
void print_hello(const char *name) {
  printf("Hello %s\n", name);
}
```

This function prints something to the screen and doesn't return anything.

A void pointer (void *) is a special type of pointer that must be cast to another type before it can be used.  

Before describing the void pointer, I want to show an example of casting pointers...

The following would produce a warning.
```c
size_t strlen(const char *s) {
  char *p = s;
  ...
```

```bash
warning: initializing 'char *' with an expression of type 'const char *' discards qualifiers
```

To avoid such a warning, you can either change the type of p to const char * or you can cast s to char *.
```c
char *p = (char *)s;
```

Imagine you have a function which will print the value of variables of different types such as the following.

Code found in <i>illustrations/0_getting_started/1_timer</i>

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

In the program above, the void pointer can't actually be used directly.  It must be converted to a different pointer type before the value that the pointer is pointing at can be referenced.  

The program above introduced <i>if</i>, <i>else if</i>, and the & operator.  If statements have a similar syntax as the while statement.  

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

else if works in connection with if.  else if only happens if the if (or else if statements abpve it) have not been evaluated to equal a true condition.  Finally, else doesn't expect a condition and essentially becomes the default block to run if all other conditions were evaluated to be false.  It's also important to realize the difference between = and ==.  A single equal statement is used for assignment.  A double equal statement indicates an equality test.

In code below, p was of type void *.  It must be converted before it can be dereferenced (to get the value of what p is pointing at).
```c
unsigned char *vp = (unsigned char *)p;
printf( "unsigned char type: %u\n", *vp );
```

The code above can be shortened to...
```c
printf( "unsigned char type: %u\n", *(unsigned char *)p );
```

You will find the above shortening often.  I used the two line version to make what was happening clear.

Placing an asterisk before a pointer results in getting the value of what the pointer is pointing at.  Placing an ampersand before a value results in a pointer to the value.  For example...
```c
double a = 1.5;
double *p = &a;  // p points to the value 1.5
```

## What happens during compilation

In C, your code is converted to binary (which is generally very hard for humans to read).  C doesn't use line separation to separate code (except for compiler directives (lines that start with #)).  In C, there are two types of comments.  /* */ can be multiline comments.  // comments run to the end of the line.  The first thing a compiler does is remove comments.

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

The lines of code after test_data_structure will run if the file called test_data_structure is older than the files after the colon.  If you edit test_timer.c, the program will be built using the following line of code.  The output is test_timer.
```Makefile
gcc test_timer.c -o test_timer
```

If you didn't want to use a Makefile to build test_data_structure, you could do so from the command line using the following command.

```bash
gcc test_timer.c -o test_timer
```

The examples block will run everytime because it doesn't have any dependencies and examples isn't a file that exists.  If you were to create a file called examples, then the examples block would cease to run.  By running <b>make</b>, you will effectively build test_timer if it needs built and run the examples block.  Running <b>make clean</b> will clean up the binary.  You can run any block by specifying it.  <b>make all</b> is equivalent to running <b>make</b>.  If you just want to run the examples block, you can by running <b>make examples</b>.  

# Doing a better job of timing

In the last section, we explored how to time the reverse_string function.  In this section, we will explore how to better time the function.  One thing you may have noticed is that there is a million calls to both reverse_string and strcpy.  There is also the overhead of the loop.  To do the timing properly, we should have timed the strcpy and the loop and subtracted that from the loop which has the reverse_string function called.

The timing for the work in <i>illustrations/0_getting_started/2_timer</i> was...
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

This section's code is found in <i>illustrations/0_getting_started/2_timer</i>

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

The timing for the work in <i>illustrations/0_getting_started/2_timer</i> was...
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

This section's code is found in <i>illustrations/0_getting_started/3_timer</i>
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

The timing for the work in <i>illustrations/0_getting_started/3_timer</i> was...
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

This section's code is found in <i>illustrations/0_getting_started/3_timer</i>
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

# Defining the timer interface

The following code is found in <i>illustrations/0_getting_started/5_timer</i>

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
