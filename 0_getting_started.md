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



```c
int main( int argc, char *argv[] ) {
  int repeat_test = 1000000;
  long overall_time = 0;
  for( int i=1; i<argc; i++ ) {
  }
  ...
  return 0;
}
```



#include "buffer.h"

int main( int argc, char *argv[] ) {
  buffer_t *bh = buffer_init(10);
  buffer_sets(bh, "Hello");
  buffer_appendc(bh, ' ');
  buffer_appendf(bh, "%s!", "Buffer");
  /* print Hello Buffer! followed with a newline */
  printf( "%s\n", buffer_data(bh) );
  buffer_destroy(bh);
  return 0;
}
```

building...

```bash
git clone https://github.com/contactandyc/realtime.git
cd illustrations/buffer/1_buffer
gcc hello_buffer.c buffer.c -o hello_buffer
```

output
```bash
$ ./hello_buffer
Hello Buffer!
$
```

Instead of <i>gcc hello...</i>, you can run
```bash
make
```
