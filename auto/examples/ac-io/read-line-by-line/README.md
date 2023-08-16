# Explanation of code

```c
#include "ac_in.h"
#include "ac_out.h"
```
Include ac_in and ac_out for input and output

```c
int main(int argc, char *argv[]) {
```
This is the entry point of the program. `argc` and `argv` are used to handle command-line arguments.

```c
if (argc < 2) {
    printf("%s <input> [output]\n", argv[0]);
    return -1;
}
```
This block checks whether there are at least two arguments (including the program name). If not, it prints a usage message and returns -1, terminating the program.

```c
ac_in_options_t in_opts;
ac_in_options_init(&in_opts);
ac_in_options_format(&in_opts, ac_io_delimiter('\n'));
```
These lines initialize an input options structure and set the delimiter to a newline character.

```c
ac_out_options_t out_opts;
ac_out_options_init(&out_opts);
ac_out_options_format(&out_opts, ac_io_delimiter('\n'));
```
Similar to the input options, these lines initialize an output options structure and set the delimiter to a newline character.

```c
ac_in_t *in = ac_in_init(argv[1], &in_opts);
```
Initializes an input handler (`ac_in_t`) with the first command-line argument as the input source and previously defined input options.

```c
ac_out_t *out;
if (argc > 2)
    out = ac_out_init(argv[2], &out_opts);
else
    out = ac_out_init_with_fd(1, false, &out_opts);
```
Initializes an output handler (`ac_out_t`) either with the second command-line argument as the file path or file descriptor 1 (standard output), depending on the number of command-line arguments.

```c
ac_io_record_t *r;
while ((r = ac_in_advance(in)) != NULL)
    ac_out_write_record(out, r->record, r->length);
```
This loop reads records from the input handler and writes them to the output handler. It continues until no more records are available.

```c
ac_out_destroy(out);
ac_in_destroy(in);
return 0;
}
```
These lines clean up the input and output handlers, releasing any allocated resources, and signal successful termination of the program.


# Running the example program

Dump the sample.txt file to standard output.

```bash
% ./read_line_by_line sample.txt
Line 1: This is the first record.
Line 2: This is the second record.
Line 3: This is the third record.
Line 4: This is the fourth record.
Line 5: This is the fifth record.
```

Compress the sample.txt file using gzip
```bash
% ./read_line_by_line sample.txt sample.txt.gz
```

Prove that the gz file is correct (requires gzcat to be installed)
```bash
% gzcat sample.txt.gz
Line 1: This is the first record.
Line 2: This is the second record.
Line 3: This is the third record.
Line 4: This is the fourth record.
Line 5: This is the fifth record.
```

Take the newly created sample.txt.gz and compress it using lz4
```bash
% ./read_line_by_line sample.txt.gz sample.txt.lz4
```

Prove that the lz4 file is correct (requires lz4cat to be installed)
```bash
% lz4cat sample.txt.lz4
Line 1: This is the first record.
Line 2: This is the second record.
Line 3: This is the third record.
Line 4: This is the fourth record.
Line 5: This is the fifth record.
```

# Summary

The ac_in/ac_out libraries can seemlessly read and write files which are compressed using lz4 or zlib.  All that is needed is the files be referred to using the .gz or .lz4 file extension.
