---
path: "/ac-mapreduce"
posttype: "docs"
title: "AC's Map Reduce Part 1"
---

The AC library has a number of useful objects.  As part of it, there are multiple ways to transform data.  To really understand it well and the rest of the ac library, I've created an example.  The example will take every line of code and text and count the tokens and then reorder the tokens by frequency descending. Before considering every line of code and text, we will start small - with "to be or not to be".

The first step in counting the tokens in "to be or not to be" is to break the string up into individual tokens and then assigning each token a frequency of 1.

```
to - 1
be - 1
or - 1
not - 1
to - 1
be - 1
```

We will call these token / frequency pairs token\_freq records.  The next step is to sort the token\_freq records such that the tokens which are identical are grouped together.  For now, we will assume that the best way to do this is sort based upon the text.

```
be - 1
be - 1
not - 1
or - 1
to - 1
to - 1
```

Once these token\_freq records are sorted, they can be grouped and reduced where reducing accumulates the frequency found on each token\_freq record which has an equal string.

```
be - 2
not - 1
or - 1
to - 2
```

Finally, the token\_freq records should be sorted by frequency descending.  We will also assume if the frequency is equal, that we will secondarily sort by token ascending.

```
be - 2
to - 2
not - 1
or - 1
```

Conceptually, this should be pretty straight forward.  We could add a bit more complexity if the original line was "To be or not to be!".  In this case, it may be desirable to normalize the case of To and to to a common case (such as lower-case).  The explanation point might also be ignored.  To create our original tokens, we might first lower-case the whole string and then break the string on spaces and explanation points.  Of course, our text and code will be more complex, so there will likely be a need to break on a whole list of characters.   

I came up with the following characters for this code base.  I'm sure I'm missing some, but the goal isn't to do perfect tokenization.  The goal is to demonstrate how the AC's schedule can be used like a map/reduce.

```
(*\"\',+-/\\| \t{});[].=&%<>!#`:
```

The second thing to consider is that to find every line of code and text within the repo, there is a need to find all of the files containing those lines.  Running a few find commands to see how much code there is yields the following within the AC repo.

```
$ find . -name "*.[ch]" -exec cat {} \; | wc
   50964  177930 1705336
$ find . -name "*.md" -exec cat {} \; | wc
   14589   70183  481292
```

or the number of files...
```
$ find . -name "*.[ch]" -exec ls {} \; | wc -l
     223
$ find . -name "*.md" -exec ls {} \; | wc -l
      35
```

There are approximately 51k lines of code in 223 files and 15k lines of documentation in 35 files.  For our routine, we will use the AC library to locate those files based upon a directory, read the files a line at a time, lowercase each line, tokenize each line, sort the tokens alphabetically, reduce the frequencies, and finally sort by frequency descending followed by a secondary sort of tokens ascending.  A major goal of the AC library make the components separable.  It's important that users can include parts that matter to them and improve upon them.  Of course, my hope is that the whole library will be found useful.

## Listing the files

ac\_io is a collection of useful input/output related functions and structures.  ac\_io\_list will find all of the files within a directory that matches your criteria.  Each file's name, size, and last modified time stamp will be returned to you.  The tag is meant for other purposes which will be explored later.  ac\_io\_list will return an array of ac\_io\_file\_info\_t structures which are defined as follows.

```c
struct ac_io_file_info_t {
  char *filename;
  size_t size;
  time_t last_modified;
  int32_t tag;
};
```

and here's the prototype for ac\_io\_list...
```c
ac_io_file_info_t *ac_io_list(const char *path, size_t *num_files,
                              bool (*file_valid)(const char *filename,
                                                 void *arg),
                              void *arg);
```

ac\_io\_list will return an array of ac\_io\_file\_info\_t which has num\_files elements (set by ac\_io\_list) from the given path.  The file\_valid is a user callback that if provided can return true or false to indicate whether the file should be returned in the list.  The arg is passed through from ac\_io\_list call to the callback file\_valid.

To follow along with code in the terminal, you will need to be in the examples/mapreduce directory.  

examples/mapreduce/list\_files.c
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_io.h"

#include <stdio.h>
#include <locale.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;  
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  char date_time[20];

  size_t total = 0;
  for (size_t i = 0; i < num_files; i++) {
    total += files[i].size;
    printf("%s %'20lu\t%s\n", ac_date_time(date_time, files[i].last_modified),
           files[i].size, files[i].filename);
  }
  printf("%'lu byte(s) in %'lu file(s)\n", total, num_files);
  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```

```
$ cd examples/mapreduce
$ make list_files
$ ./list_files ../.. c,h,md
...
2020-01-22 22:10:42                2,501	../../src/ac_timer.c
2020-01-23 18:47:59                2,542	../../src/ac_cgi.h
2020-01-24 02:19:38               11,710	../../src/ac_in.h
2,193,648 byte(s) in 259 file(s)
```

A few quick C things to call out about the example above.  The first line of the main function calls setlocale.

```c
setlocale(LC_NUMERIC, "");
```

This causes printf and %' to print comma delimited numbers.  setlocale is found in locale.h.  

Because all of the files have a time\_t time stamp, I've included ac\_conv to use the ac\_date\_time method to convert the time stamp into a readable format.  ac\_date\_time requires a 20 byte array to passed to it and returns a reference to it so it can be used inline (as it is in the printf call above).

ac\_split is found in ac\_allocator.h and splits a string into an array which can later be freed using ac\_free.

In file_ok...
```c
bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}
```

ac\_io\_extension is found in ac\_io.h and returns true if the given string matches the given extension.  If the string is an empty string "", then it will match a file if no extension exists (there is no way to match a file that ends in a period).

```c
size_t num_files = 0;
ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);
```
Finds all of the files which have the expected extensions.

ac\_io\_list and ac\_split both allocate memory using ac\_allocator, so ac\_free is used to free the memory.

## Reading a file a line at a time

The ac\_in object allows reading a file at a time, merging a group of sorted files as if the file is a single file, creating a stream from a buffer, a set of records, and more.  It also allows reading a gzip or lz4 file as if it was uncompressed.

For this, we will change the following lines in list\_files.c to dump the files to the terminal
```c
size_t total = 0;
for (size_t i = 0; i < num_files; i++) {
  total += files[i].size;
  printf("%s %'20lu\t%s\n", ac_date_time(date_time, files[i].last_modified),
         files[i].size, files[i].filename);
}
printf("%'lu byte(s) in %'lu file(s)\n", total, num_files);
```

gets changed to in examples/mapreduce/dump\_files\_1.c
```c
ac_in_options_t opts;
ac_in_options_init(&opts);
ac_in_options_format(&opts, ac_io_delimiter('\n'));

for (size_t i = 0; i < num_files; i++) {
  ac_io_record_t *r;
  ac_in_t *in = ac_in_init(files[i].filename, &opts);
  while ((r = ac_in_advance(in)) != NULL)
    printf("%s\n", r->record);
  ac_in_destroy(in);
}
```

```
$ make dump_files_1
$ ./dump_files_1 . txt
This is line 1
This is line 2
This is line 3
$ ./dump_files_1 . lz4
This is line 1
This is line 2
This is line 3
$ ./dump_files_1 . gz
This is line 1
This is line 2
This is line 3
```

The example above dumps the contents of sample.txt, sample.txt.gz, and sample.txt.lz4.  ac\_in automatically understands files that have a gz or lz4 extension to be compressed.

The ac\_in object is configured so to understand what a record looks like.  In this case, a record is a line of text and each line is delimited by a newline character ('\n').  The ac\_in\_options\_t structure is first initialized and then the record type is indicated.

```c
ac_in_options_t opts;
ac_in_options_init(&opts);
ac_in_options_format(&opts, ac_io_delimiter('\n'));
```

The for loop after it will open each file as an ac\_in object and then print each record.

```c
for (size_t i = 0; i < num_files; i++) {
  ac_io_record_t *r;
  ac_in_t *in = ac_in_init(files[i].filename, &opts);
  while ((r = ac_in_advance(in)) != NULL)
    printf("%s\n", r->record);
  ac_in_destroy(in);
}
```

This code can be further simplified using ac\_in\_init\_from\_list.

```c
ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
ac_io_record_t *r;
while ((r = ac_in_advance(in)) != NULL)
  printf("%s\n", r->record);
ac_in_destroy(in);
```

I realize this doesn't reduce that many lines of code, but it does begin to demonstrate how ac\_in can be incredibly useful when working with data.  The logic to open all of the files is wrapped up in the first line.  The logic to read all of the files is in the next 3, and the logic to destroy everything is in the last line.

examples/mapreduce/dump\_code.c
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    printf("%s\n", r->record);
  ac_in_destroy(in);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```

## Lowercasing and tokenizing

The ac\_pool object contains a naive tokenizer (ac\_pool\_tokenize) that takes in a string, a list of characters to break on, and returns the list of tokens found.

Continuing from the dump\_files.c in the previous section, the following modifications will be made to make lowercase\_and\_tokenize.c  

ac\_pool must be included.
```c
#include "ac_pool.h"
```

Change
```c
ac_io_record_t *r;
while ((r = ac_in_advance(in)) != NULL)
  printf("%s\n", r->record);
```

to
```c
lowercase_and_tokenize(in);
```

and add the lowercase\_and\_tokenize method.
```c
void lowercase(char *s) {
  while(*s) {
    if(*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void lowercase_and_tokenize(ac_in_t *in) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    for( size_t i=0; i<num_tokens; i++ )
      printf("[%lu] %s\n", i, tokens[i]);
  }
  ac_pool_destroy(pool);
}
```

```
$ make lowercase_and_tokenize
$ ./lowercase_and_tokenize . txt
[0] this
[1] is
[2] line
[3] 1
[0] this
[1] is
[2] line
[3] 2
[0] this
[1] is
[2] line
[3] 3
```

lowercase\_and\_tokenize takes the ac\_in\_t object as an argument and iterates through all of the records, lowercasing, tokenizing, and printing them.

The ac\_pool object is introduced in this example.  The ac\_pool\_tokenize method requires it to do the tokenization.  Memory is collected when ac\_pool\_clear or ac\_pool\_destroy is called.  The pool is initially setup to keep a reserve of 4096 bytes.  This should be plenty for the tokens in normal lines of text.  ac\_pool\_clear is an extremely cheap call to make, so I frequently will place it at the beginning of a loop.  This will ensure that it gets cleared if there happen to be continue statements throughout the loop.

examples/mapreduce/lowercase\_and\_tokenize.c
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_pool.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void lowercase_and_tokenize(ac_in_t *in) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    for (size_t i = 0; i < num_tokens; i++)
      printf("[%lu] %s\n", i, tokens[i]);
  }
  ac_pool_destroy(pool);
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  lowercase_and_tokenize(in);
  ac_in_destroy(in);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```

## Writing token frequency pairs to a sorted output

The token frequency pairs will be written in the following format.

```
4 byte frequency
a regular c string for a token (without the zero terminator)
```

ac\_io defines a method for comparing records with the following signature.
```c
int compare(const ac_io_record_t *r1, const ac_io_record_t *r2, void *arg);
```

In order to write to a sorted stream, this must be implemented.  Our goal is to sort the tokens by the token string.  The compare method would look like..
```c
int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}
```

In order to use ac\_out\_t (for writing output), ac\_out.h must be included.
```c
#include "ac_out.h"
```

Changing the function named lowercase\_and\_tokenize to
lowercase\_tokenize\_and\_write\_tokens and alter the signature to include ac\_out\_t \*out.

```c
void lowercase_and_tokenize(ac_in_t *in) {
```

becomes

```c
void lowercase_tokenize_and_write_tokens(ac_in_t *in, ac_out_t *out) {
```

A buffer is created at the beginning of the function and destroyed at the end.
```c
ac_buffer_t *bh = ac_buffer_init(1000);
...
ac_destroy(bh);
```

Instead of printing the tokens, they are written to out (the output stream).

```c
for (size_t i = 0; i < num_tokens; i++)
  printf("[%lu] %s\n", i, tokens[i]);
```

Changes to
```c
uint32_t one = 1;
for (size_t i = 0; i < num_tokens; i++) {
  ac_buffer_set(bh, &one, sizeof(one));
  ac_buffer_appends(bh, tokens[i]);
  ac_out_write_record(out, ac_buffer_data(bh), ac_buffer_length(bh));
}
```

The ac\_buffer is introduced to form variable length records (a common use case).  In this case, the records start with a 4 byte frequency (which is equal to 1) and then are followed by the individual token.  ac\_buffer has a clear method (ac\_buffer\_clear), but ac\_buffer\_set is equivalent in that it first clears the buffer and sets new data.  ac\_buffer\_data returns a pointer to the data and ac\_buffer\_length returns the length of the given data.

ac\_out\_write\_record is the only reference to the out object passed into this function.  This is important because this function doesn't care if the output is sorted, partitioned, compressed, or just a regular file.  It also doesn't know if the record format is fixed length, variable length, or a delimited format.  That's all configured - which I'll show next.

The following code initializes the output stream
```c
ac_out_options_t out_opts;
ac_out_options_init(&out_opts);
ac_out_options_format(&out_opts, ac_io_prefix());

ac_out_ext_options_t out_ext_opts;
ac_out_ext_options_init(&out_ext_opts);
ac_out_ext_options_compare(&out_ext_opts, compare_tokens, NULL);

ac_out_t *out = ac_out_ext_init("sorted_tokens", &out_opts, &out_ext_opts);
```

At first, it may look like a lot of code, but that's mostly because I use very long function names.  The first 3 lines setup the regular output options to be prefix format (4 byte prefixed length followed by data).  The next 3 lines setup an extended output options to use the compare\_tokens method defined above.  Because the extended options are used, ac\_out\_ext\_init is used to finally initialize the output.


```
$ make sort_tokens
$ ./sort_tokens ../.. c,h,md
$ less sorted_tokens
^E^@^@^@^A^@^@^@$^E^@^@^@^A^@^@^@$^E
^@^@^@^A^@^@^@$^E^@^@^@^A^@^@^@$^E^@
^@^@^A^@^@^@$^E^@^@^@^A^@^@^@$^E^@^@
^@^A^@^@^@$^E^@^@^@^A^@^@^@$^E^@^@^@
^A^@^@^@$^E^@^@^@^A^@^@^@$^E^@^@^@^A
^@^@^@$^E^@^@^@^A^@^@^@$^E^@^@^@^A^@
^@^@$^E^@^@^@^A^@^@^@$^E^@^@^@^A^@^@
```

The data is binary and hard to read.  The binary seems to a 4 byte 5, a 4 byte 1, and a single byte '$' over and over again.  In the next section, the binary data will be displayed.  

examples/mapreduce/sort\_tokens.c:
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"
#include "ac_pool.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void sort_tokens_by_token(ac_in_t *in, ac_out_t *out) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_init(1000);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    uint32_t one = 1;
    for (size_t i = 0; i < num_tokens; i++) {
      ac_buffer_set(bh, &one, sizeof(one));
      ac_buffer_appends(bh, tokens[i]);
      ac_out_write_record(out, ac_buffer_data(bh), ac_buffer_length(bh));
    }
  }
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
}

int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_prefix());

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare_tokens, NULL);

  ac_out_t *out = ac_out_ext_init("sorted_tokens", &out_opts, &out_ext_opts);
  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  sort_tokens_by_token(in, out);
  ac_in_destroy(in);
  ac_out_destroy(out);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```

## Displaying the sorted output

In the last section, the data produced was binary.  In this section, we will print the sorted output just before the final sorted file is written (and avoid writing the final sorted file).

The first change from sort\_tokens.c to sort\_tokens\_and\_display.c is to replace the ac\_out\_destroy with an ac\_out\_in call.  ac\_out\_in will take an output stream and convert it to an input stream.  After calling ac\_out\_in, there is no longer a need to destroy the output stream.  

```c
ac_out_destroy(out);
```

changes to
```c
in = ac_out_in(out);
ac_io_record_t *r;
while ((r = ac_in_advance(in)) != NULL) {
  uint32_t frequency = (*(uint32_t *)(r->record));
  char *token = r->record + sizeof(uint32_t);
  printf("%u\t%s\n", frequency, token);
}
ac_in_destroy(in);
```

As before, we will move the print loop to a function called display\_token\_frequencies.

```c
in = ac_out_in(out);
display_token_frequencies(in);
ac_in_destroy(in);
```

and add the function above the main function.
```c
void display_token_frequencies(ac_in_t *in) {
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    uint32_t frequency = (*(uint32_t *)(r->record));
    char *token = r->record + sizeof(uint32_t);
    printf("%u\t%s\n", frequency, token);
  }
}
```

```
$ make sort_tokens_and_display
$ ./sort_tokens_and_display ../.. c,h,md > sorted_tokens.txt
$ head sorted_tokens.txt
1	$
1	$
1	$
1	$
1	$
1	$
1	$
1	$
1	$
1	$
```

As expected, the first tokens consist of a bunch of '$' tokens each with a frequency of 1.  If you were to continue looking through sorted\_tokens.txt, you would find that the tokens are sorted.

examples/mapreduce/sort\_tokens\_and\_display.c:
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"
#include "ac_pool.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void lowercase_tokenize_and_write_tokens(ac_in_t *in, ac_out_t *out) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_init(1000);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    uint32_t one = 1;
    for (size_t i = 0; i < num_tokens; i++) {
      ac_buffer_set(bh, &one, sizeof(one));
      ac_buffer_appends(bh, tokens[i]);
      ac_out_write_record(out, ac_buffer_data(bh), ac_buffer_length(bh));
    }
  }
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
}

int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

void display_token_frequencies(ac_in_t *in) {
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    uint32_t frequency = (*(uint32_t *)(r->record));
    char *token = r->record + sizeof(uint32_t);
    printf("%u\t%s\n", frequency, token);
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_prefix());

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare_tokens, NULL);

  ac_out_t *out = ac_out_ext_init("sorted_tokens", &out_opts, &out_ext_opts);
  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  lowercase_tokenize_and_write_tokens(in, out);
  ac_in_destroy(in);
  in = ac_out_in(out);
  display_token_frequencies(in);
  ac_in_destroy(in);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```


## Reducing the frequencies

In the last example, all of the tokens were shown to be sorted and have a frequency of 1.  This will show how to reduce the token/one pairs such that each token exists with a total frequency (and isn't repeated).  ac\_io.h defines a prototype for reducers as follows.

```c
bool reducer(ac_io_record_t *res, const ac_io_record_t *r, size_t num_r,
             ac_buffer_t *bh, void *arg);
```

The reducer function is expected to set \*res and return true if the array of records reduced properly.  If false is returned, all of the records will be skipped.  It is very common that an ac\_buffer\_t object would be handy in constructing the final record, so it is passed into the reducer (but can be ignored).  The arg is user specified, but often is just NULL.  The AC map/reduce approach to reducing records is to call a reducer with an array of num\_r records which are all equal according to the compare method.  It is often the case that the first record is all that is needed and because of that, ac\_io defines a reducer named ac\_io\_keep\_first.

Our reducer will sum the individual frequencies.
```c
bool reduce_frequency(ac_io_record_t *res, const ac_io_record_t *r, size_t num_r,
                      ac_buffer_t *bh, void *arg) {
  *res = r[0];
  uint32_t total = 0;
  for(size_t i=0; i<num_r; i++ )
    total += (*(uint32_t *)r[i].record);
  (*(uint32_t *)res->record) = total;
  return true;
}
```

The first 4 bytes of each record are the frequencies.  The for loop sums up all of the frequencies.

The only changes from sort\_tokens\_and\_display.c to make sort\_tokens\_reduce\_and\_display.c are to add the reduce\_frequency method above and then hook it up to the out\_ext\_options as follows (just after the compare method is set in the main function).

```c
ac_out_ext_options_reducer(&out_ext_opts, reduce_frequency, NULL);
```

```
$ make sort_tokens_reduce_and_display
$ ./sort_tokens_reduce_and_display ../.. c,h,md > sorted_tokens.txt
$ head sorted_tokens.txt
220	$
1	$1
1	$2
3	$^e
39	$^e^
43	$ac
5201	0
3	00
1	000
2	000ns
```

Notice that the tokens are alphabetical, unique, and have frequencies!  Also lowercase\_tokenize\_and\_write\_tokens didn't actually change at all with this change.  Again, this shows how it is useful to separate opening output streams from the transformations.


examples/mapreduce/sort\_tokens\_reduce\_and\_display.c:
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"
#include "ac_pool.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void lowercase_tokenize_and_write_tokens(ac_in_t *in, ac_out_t *out) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_init(1000);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    uint32_t one = 1;
    for (size_t i = 0; i < num_tokens; i++) {
      ac_buffer_set(bh, &one, sizeof(one));
      ac_buffer_appends(bh, tokens[i]);
      ac_out_write_record(out, ac_buffer_data(bh), ac_buffer_length(bh));
    }
  }
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
}

int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

bool reduce_frequency(ac_io_record_t *res, const ac_io_record_t *r,
                      size_t num_r, ac_buffer_t *bh, void *arg) {
  *res = r[0];
  uint32_t total = 0;
  for (size_t i = 0; i < num_r; i++)
    total += (*(uint32_t *)r[i].record);
  (*(uint32_t *)res->record) = total;
  return true;
}

void display_token_frequencies(ac_in_t *in) {
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    uint32_t frequency = (*(uint32_t *)(r->record));
    char *token = r->record + sizeof(uint32_t);
    printf("%u\t%s\n", frequency, token);
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_prefix());

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare_tokens, NULL);
  ac_out_ext_options_reducer(&out_ext_opts, reduce_frequency, NULL);

  ac_out_t *out = ac_out_ext_init("sorted_tokens", &out_opts, &out_ext_opts);
  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  lowercase_tokenize_and_write_tokens(in, out);
  ac_in_destroy(in);
  in = ac_out_in(out);
  display_token_frequencies(in);
  ac_in_destroy(in);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```

## Chaining sorts - sort by frequency

In the last two sections, the result of the sort is printed to the screen and not saved anywhere.  The last section had each token being unique with a frequency.  Next, we will make changes to sort the token frequency pairs by frequency first descending and token next ascending.  In order to do this, first define a compare method.

```c
int compare_tokens_by_frequency(const ac_io_record_t *r1, const ac_io_record_t *r2,
                                void *arg) {
  uint32_t *a = (uint32_t *)r1->record;
  uint32_t *b = (uint32_t *)r2->record;
  if(*a != *b)
    return (*a < *b) ? 1 : -1; // descending
  return strcmp((char *)(a+1), (char *)(b+1));
}
```

Next, we will setup an output stream using the extended options.  We can reuse the previous out\_ext\_opts from before and then create the output stream.  This will be done right after the ac\_out\_in call.  The following lines will change from...
```c
in = ac_out_in(out);
display_token_frequencies(in);
ac_in_destroy(in);
```

to
```c
in = ac_out_in(out);
ac_out_ext_options_compare(&out_ext_opts, compare_tokens_by_frequency, NULL);
ac_out_ext_options_reducer(&out_ext_opts, NULL, NULL);
out = ac_out_ext_init("sorted_tokens_final", &out_opts, &out_ext_opts);
ac_io_record_t *r;
while((r=ac_in_advance(in)) != NULL)
  ac_out_write_record(out, r->record, r->length);
ac_in_destroy(in);

in = ac_out_in(out);
display_token_frequencies(in);
ac_in_destroy(in);
```

Even though the final sorted output isn't written to disk.  Temp files may be written.  Because of this, it is important to make sure that the names are not the same.  Notice that I use "sorted\_tokens\_final" in the second call to ac\_out\_ext\_init.

The following three lines are so common that there is a helper function in ac\_in.h named ac\_in\_out.

```c
ac_io_record_t *r;
while((r=ac_in_advance(in)) != NULL)
  ac_out_write_record(out, r->record, r->length);
```

can be replaced with
```c
ac_in_out(in, out);
```

This makes our change
```c
in = ac_out_in(out);
ac_out_ext_options_compare(&out_ext_opts, compare_tokens_by_frequency, NULL);
ac_out_ext_options_reducer(&out_ext_opts, NULL, NULL);
out = ac_out_ext_init("sorted_tokens_final", &out_opts, &out_ext_opts);
ac_in_out(in, out);
ac_in_destroy(in);

in = ac_out_in(out);
display_token_frequencies(in);
ac_in_destroy(in);
```

This is all there is to chaining sorts in the in/out library.  Notice that the last 3 lines of the change are the same as the original lines.  Again, display\_token\_frequencies doesn't care about the order of the input, so it all just works!

```
$ make sort_tokens_reduce_sort_by_freq_and_display
$ ./sort_tokens_reduce_sort_by_freq_and_display ../.. c,h,md > sorted_tokens.txt
$ head sorted_tokens.txt
6993	the
5223	0
4476	if
4308	1
4243	h
3762	p
3688	n
3608	a
3514	char
3473	const
```

If everything worked as expected, running this latest program against the "To be or not to be!" file found in sample should yield

```
2 be
2 to
1 not
1 or
```

And it does!
```
$ ./sort_tokens_reduce_sort_by_freq_and_display sample tbontb
2	be
2	to
1	not
1	or
```

examples/mapreduce/sort\_tokens\_reduce\_sort\_by\_freq\_and\_display.c
```c
#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"
#include "ac_pool.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void lowercase_tokenize_and_write_tokens(ac_in_t *in, ac_out_t *out) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_init(1000);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    uint32_t one = 1;
    for (size_t i = 0; i < num_tokens; i++) {
      ac_buffer_set(bh, &one, sizeof(one));
      ac_buffer_appends(bh, tokens[i]);
      ac_out_write_record(out, ac_buffer_data(bh), ac_buffer_length(bh));
    }
  }
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
}

int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

int compare_tokens_by_frequency(const ac_io_record_t *r1,
                                const ac_io_record_t *r2, void *arg) {
  uint32_t *a = (uint32_t *)r1->record;
  uint32_t *b = (uint32_t *)r2->record;
  if (*a != *b)
    return (*a < *b) ? 1 : -1; // descending
  return strcmp((char *)(a + 1), (char *)(b + 1));
}

bool reduce_frequency(ac_io_record_t *res, const ac_io_record_t *r,
                      size_t num_r, ac_buffer_t *bh, void *arg) {
  *res = r[0];
  uint32_t total = 0;
  for (size_t i = 0; i < num_r; i++)
    total += (*(uint32_t *)r[i].record);
  (*(uint32_t *)res->record) = total;
  return true;
}

void display_token_frequencies(ac_in_t *in) {
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    uint32_t frequency = (*(uint32_t *)(r->record));
    char *token = r->record + sizeof(uint32_t);
    printf("%u\t%s\n", frequency, token);
  }
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_prefix());

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare_tokens, NULL);
  ac_out_ext_options_reducer(&out_ext_opts, reduce_frequency, NULL);

  ac_out_t *out = ac_out_ext_init("sorted_tokens", &out_opts, &out_ext_opts);
  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  lowercase_tokenize_and_write_tokens(in, out);
  ac_in_destroy(in);
  in = ac_out_in(out);
  ac_out_ext_options_compare(&out_ext_opts, compare_tokens_by_frequency, NULL);
  ac_out_ext_options_reducer(&out_ext_opts, NULL, NULL);
  out = ac_out_ext_init("sorted_tokens_final", &out_opts, &out_ext_opts);
  ac_in_out(in, out);
  ac_in_destroy(in);

  in = ac_out_in(out);
  display_token_frequencies(in);
  ac_in_destroy(in);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
```

## Final thoughts

Hopefully, you've been able to see how the ac\_in and ac\_out objects can be helpful.  There was much that wasn't covered that can be found in the ac\_in and ac\_out documentation and the header files.

Understanding how ac\_in and ac\_out work is helpful for using ac\_schedule (the next part of this documentation).  While this code is efficient, my macbook has 16 threaded cores and a lot of RAM.  The ac\_schedule is built to take advantage of multiple cores and lots (or a limited supply) of RAM.
