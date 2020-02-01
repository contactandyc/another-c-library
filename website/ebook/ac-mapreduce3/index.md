---
path: "/ac-mapreduce3"
posttype: "docs"
title: "AC's Map Reduce Part 3"
---

In the last post, the ac\_schedule package was introduced, along with many of the features for managing tasks and creating a data pipeline using ac\_task\_input\_files and ac\_task\_output.  In this post, we will setup the example from part 1 using ac\_schedule and explore different ways of doing it.

## Back to Part 1

The example in part one took every line of code and text and count the tokens and then reorder the tokens by frequency descending.  The tasks could be defined as follows.

1. Convert text input into tokens, lowercasing and tokenizing, and writing to sorted and reduced output.  We could call this text\_to\_tokens
2. Further reduce the counts from text\_to\_tokens and then write to an output which is sorted by frequency descending and then token ascending.  This could be called token\_aggregator
3. Dump the top N token/frequency pairs to the terminal in frequency descending, token ascending order.

Do do this, we can alter the last code from the last post /examples/mapreduce2/input\_data\_4.c.

```c
ac_schedule_task(scheduler, "split", true, setup_split);
ac_schedule_task(scheduler, "partition", true, setup_partition);
ac_schedule_task(scheduler, "first", true, setup_first);
ac_schedule_task(scheduler, "all", true, setup_all);
ac_schedule_task(scheduler, "multi", true, setup_multi);
```

gets replaced with
```c
ac_schedule_task(scheduler, "text_to_tokens", true, setup_text_to_tokens);
ac_schedule_task(scheduler, "token_aggregator", true, setup_token_aggregator);
ac_schedule_task(scheduler, "dump_tokens", false, setup_dump_tokens);
```

and the setup functions have to change as well..
```c
bool setup_split(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_dump(task, ac_task_dump_text, NULL);
  ac_task_output(task, "split.lz4", "first|multi", 0.9, 0.1, AC_OUTPUT_SPLIT);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_first(ac_task_t *task) {
  ac_task_output(task, "first.lz4", "partition|multi", 0.9, 0.1,
                 AC_OUTPUT_FIRST);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_partition(ac_task_t *task) {
  ac_task_output(task, "partition.lz4", "all|multi", 0.9, 0.1,
                 AC_OUTPUT_PARTITION);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_all(ac_task_t *task) {
  ac_task_output(task, "all.lz4", "multi", 0.9, 0.1, AC_OUTPUT_NORMAL);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_multi(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}
```

gets replaced with
```c
bool setup_text_to_tokens(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_dump(task, ac_task_dump_text, NULL);
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_dump_tokens(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}
```

In setup\_text\_to\_tokens:
```c
ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
               AC_OUTPUT_SPLIT);
```

The output is marked as split because every partition of setup\_text\_to\_tokens may have similar tokens.  In order for the overall job to work properly, tokens should be split so that in the next task (token\_aggregator), tokens can be properly accumulated or reduced.

```c
ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
              AC_OUTPUT_NORMAL);
```

The output is marked as normal because all of the output from this task/partition needs to go to the dump\_tokens task.  The dump\_tokens task is not partitioned because for our task, it doesn't make sense.

The code is found in /examples/mapreduce3/part\_1\_1.c
```
$ rm -rf tasks
$ make part_1_1
$ ./part_1_1
Finished text_to_tokens[0] on thread 0 in 0.000ms
Finished text_to_tokens[1] on thread 1 in 0.000ms
Finished token_aggregator[0] on thread 1 in 0.000ms
Finished token_aggregator[1] on thread 2 in 0.000ms
Finished dump_tokens[0] on thread 2 in 0.000ms
$ ./part_1_1 -s
task: text_to_tokens [0/2]
  reverse dependencies:  token_aggregator[2]
  custom runner
      input[0]: split_input (0)
      output[0]: tokens.lz4 split
        destinations: token_aggregator[2]
        tasks/text_to_tokens_0/tokens_0.lz4
task: text_to_tokens [1/2]
  reverse dependencies:  token_aggregator[2]
  custom runner
      input[0]: split_input (1)
        sample/sample.tbontb (20)
      output[0]: tokens.lz4 split
        destinations: token_aggregator[2]
        tasks/text_to_tokens_1/tokens_1.lz4
task: token_aggregator [0/2]
  dependencies:  text_to_tokens[2]
  reverse dependencies:  dump_tokens[1]
  custom runner
      input[0]: tokens.lz4 (2)
        tasks/text_to_tokens_0/tokens_0_0.lz4 (0)
        tasks/text_to_tokens_1/tokens_1_0.lz4 (0)
      output[0]: tokens_by_frequency.lz4 normal
        destinations: dump_tokens[1]
        tasks/token_aggregator_0/tokens_by_frequency_0.lz4
task: token_aggregator [1/2]
  dependencies:  text_to_tokens[2]
  reverse dependencies:  dump_tokens[1]
  custom runner
      input[0]: tokens.lz4 (2)
        tasks/text_to_tokens_0/tokens_0_1.lz4 (0)
        tasks/text_to_tokens_1/tokens_1_1.lz4 (0)
      output[0]: tokens_by_frequency.lz4 normal
        destinations: dump_tokens[1]
        tasks/token_aggregator_1/tokens_by_frequency_1.lz4
task: dump_tokens [0/1]
  dependencies:  token_aggregator[2]
  custom runner
      input[0]: tokens_by_frequency.lz4 (2)
        tasks/token_aggregator_0/tokens_by_frequency_0.lz4 (0)
        tasks/token_aggregator_1/tokens_by_frequency_1.lz4 (0)
```

This shows that the files are properly wired up.  dump\_tokens will receive a file from both of the token\_aggregator partitions.  The token\_aggregator receives split files from both partitions of text\_to\_tokens.  In the next section, we will begin to actually write some output.

/examples/mapreduce3/part\_1\_1.c
```c
#include "ac_schedule.h"

typedef struct {
  size_t num_inputs;
  ac_io_file_info_t *inputs;
  char *dir;
  char *ext;
  char **extensions;
} custom_arg_t;

bool file_ok(const char *filename, void *arg) {
  custom_arg_t *ca = (custom_arg_t *)arg;
  char **p = ca->extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

bool finish_custom_args(int argc, char **argv, void *arg) {
  custom_arg_t *ca = (custom_arg_t *)arg;
  if (!ca->dir)
    ca->dir = (char *)"sample";
  if (!ca->ext)
    ca->ext = (char *)"tbontb";

  ca->extensions = ac_split2(NULL, ',', ca->ext);
  ca->inputs = ac_io_list(ca->dir, &ca->num_inputs, file_ok, ca);
  if (ca->inputs)
    return true;
  return false;
}

int parse_custom_args(int argc, char **argv, void *arg) {
  if (argc < 2)
    return 0;

  custom_arg_t *ca = (custom_arg_t *)arg;

  if (!strcmp(argv[0], "--dir")) {
    ca->dir = argv[1];
    return 2;
  } else if (!strcmp(argv[0], "--ext")) {
    ca->ext = argv[1];
    return 2;
  }
  return 0;
}

ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  custom_arg_t *ca = (custom_arg_t *)ac_task_custom_arg(w->task);
  return ac_io_select_file_info(w->worker_pool, num_files, ca->inputs,
                                ca->num_inputs, w->partition,
                                w->num_partitions);
}

bool do_nothing(ac_worker_t *w) { return true; }

bool setup_text_to_tokens(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_dump(task, ac_task_dump_text, NULL);
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ac_task_runner(task, do_nothing);
  return true;
}

bool setup_dump_tokens(ac_task_t *task) {
  ac_task_runner(task, do_nothing);
  return true;
}

void custom_usage() {
  printf("Find all tokens ending in .h, .c, and .md and sort by\n");
  printf("frequency descending.\n\n");
  printf("--dir <dir> - directory to scan\n");
  printf("--ext <extensions> - comma delimited list of file extensions to "
         "consider");
}

int main(int argc, char *argv[]) {
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);
  custom_arg_t custom;
  memset(&custom, 0, sizeof(custom));

  ac_schedule_custom_args(scheduler, custom_usage, parse_custom_args,
                          finish_custom_args, &custom);

  ac_schedule_task(scheduler, "text_to_tokens", true, setup_text_to_tokens);
  ac_schedule_task(scheduler, "token_aggregator", true, setup_token_aggregator);
  ac_schedule_task(scheduler, "dump_tokens", false, setup_dump_tokens);

  ac_schedule_run(scheduler, ac_worker_complete);

  if (custom.inputs)
    ac_free(custom.inputs);
  if (custom.extensions)
    ac_free(custom.extensions);

  ac_schedule_destroy(scheduler);
  return 0;
}
```

## Transforming Input into Output

At the heart of the map/reduce pattern is that each task will transform input into output.  In Part 1 of this series, files were broken into lines and then tokens which were transformed into a sorted set of tokens each with a frequency of one.  In the last section, three tasks were defined (text\_to\_tokens, token\_aggregator, and dump\_tokens).  In this section, text\_to\_tokens will be defined.  From the first post's final source code (/examples/mapreduce/sort\_tokens\_reduce\_sort\_by\_freq\_and\_display.c), we can take the following code.

```c
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
```

The function lowercase\_tokenize\_and\_write\_tokens can be converted to use the same signature as the do\_nothing function as follows.

```c
bool do_nothing(ac_worker_t *w) { return true; }

bool text_to_tokens(ac_worker_t *w) {
  ac_in_t *in = ac_worker_in(w, 0);
  ac_out_t *out = ac_worker_out(w, 0);
  ac_pool_t *pool = w->pool;
  ac_buffer_t *bh = w->bh;

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

  ac_in_destroy(in);
  ac_out_destroy(out);
  return true;
}
```

In the latter function, the pool and buffer are just used as they are members of ac\_worker\_t.  The in and out are constructed using ac\_worker\_in and ac\_worker\_out respectively.  The zero as the second argument corresponds to the numbered input or output.  The first input is 0, second is 1, etc.  In the previous section, running ./part\_1\_1 -s displayed the following.

```
input[0]: split_input (1)
  sample/sample.tbontb (20)
output[0]: tokens.lz4 split
  destinations: token_aggregator[2]
  tasks/text_to_tokens_1/tokens_1.lz4
```

ac\_worker\_in(w, 0) takes sample/sample.tbontb and converts it into an ac\_in\_t object.  ac\_worker\_out(w, 0) creates an output that is split with a base name of tasks/text\_to\_tokens\_1/tokens\_1.lz4 and expects it to be written to 2 partitions.

Other than that, the text\_to\_tokens function is the same as lowercase\_tokenize\_and\_write\_tokens.

Now that text\_to\_tokens is implemented, it can be referenced in setup\_text\_to\_tokens.

```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_runner(task, do_nothing);
  return true;
}
```

changes to
```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_runner(task, text_to_tokens);
  return true;
}
```

In order for the file to be written properly (and for the command line dump feature to work), the output needs to be configured a bit more.

```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ...
}
```

changes to
```c
void dump_term_frequency(ac_worker_t *w, ac_io_record_t *r, ac_buffer_t *bh,
                         void *arg) {
  ac_buffer_appendf(bh, "%u\t%s", *(uint32_t *)(r->record),
                    r->record + sizeof(uint32_t));
}

bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ...
}
```

## Partitioning output

The dump\_term\_frequency was needed to describe the binary contents of the record.  ac\_io\_prefix is a good choice for a format when the record is variable length and binary.

Running this will yield the following.
```
$ make part_1_2
$ rm -rf tasks
$ ./part_1_2
tasks/text_to_tokens_0/tokens_0.lz4 from text_to_tokens is configured
  to be split, but does not specify a partition method!  Exiting early!
Abort trap: 6
```

In order to have split output, a partition method must be specified.  The function that needs to be called is ac\_task\_output\_partition.

```c
void ac_task_output_partition(ac_task_t *task, ac_io_partition_f part,
                              void *arg);
```

The signature of the partition function is...
```c
size_t ac_io_partition(const ac_io_record_t *r, size_t num_part, void *tag);
```

In our case, we will want to split the data based upon the token and not the whole record.  Our partition method will be..

```c
#include "ac_conv.h"

size_t partition_by_token(const ac_io_record_t *r, size_t num_part, void *tag) {
  char *token = r->record+sizeof(uint32_t);
  uint64_t hash = ac_hash64(token, r->length-sizeof(uint32_t));
  return hash % num_part;
}
```

ac\_conv includes a number of conversions which are useful including hashing a set of bytes and converting it into a 64 bit integer (this uses the xxhash found in lz4 library).

The partition\_by\_token can now be configured in setup\_text\_to\_tokens.
```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_partition(task, partition_by_token, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ...
}
```

Running this now...
```
$ make part_1_3
$ ./part_1_3
Finished text_to_tokens[0] on thread 1 in 0.427ms
Finished text_to_tokens[1] on thread 3 in 0.498ms
Finished token_aggregator[0] on thread 3 in 0.000ms
Finished token_aggregator[1] on thread 0 in 0.000ms
Finished dump_tokens[0] on thread 0 in 0.000ms
```

Notice that the text\_to\_tokens seemed to take a little time.  
```
$ ./part_1_3 -s -t token_aggregator:0
task: token_aggregator [0/2]
  dependencies:  text_to_tokens[2]
  reverse dependencies:  dump_tokens[1]
  custom runner
      input[0]: tokens.lz4 (2)
        tasks/text_to_tokens_0/tokens_0_0.lz4 (11)
        tasks/text_to_tokens_1/tokens_1_0.lz4 (49)
      output[0]: tokens_by_frequency.lz4 normal
        destinations: dump_tokens[1]
        tasks/token_aggregator_0/tokens_by_frequency_0.lz4
```

Inspecting the first partition of token\_aggregator, we can see that the tokens\_0\_0.lz4 is 11 bytes and the tokens\_1\_0.lz4 is 49 bytes.  The lz4 format typically has an 11 byte overhead, so the 11 byte files are actually empty files - which is okay.  The tokens\_1\_0.lz4 can be dumped using the -d option.

```
$ ./part_1_3 -d tasks/text_to_tokens_1/tokens_0_0.lz4
$ ./part_1_3 -d tasks/text_to_tokens_1/tokens_0_1.lz4
$ ./part_1_3 -d tasks/text_to_tokens_1/tokens_1_0.lz4
1	to
1	be
1	not
1	to
1	be
$ ./part_1_3 -d tasks/text_to_tokens_1/tokens_1_1.lz4
1	or
```

Wildcards also work.
```
$ ./part_1_3 -d tasks/text_to_tokens_1/tokens_*.lz4
1	to
1	be
1	not
1	to
1	be
1	or
```

The -p option will show line numbers in the first column.
```
$ ./part_1_3 -p tasks/text_to_tokens_1/tokens_*.lz4
1	1	to
2	1	be
3	1	not
4	1	to
5	1	be
1	1	or
```

## Writing Sorted Output

Our goal for this task was that it would write sorted and reduced output.  To write sorted output, a comparison function must be specified.  The function to specify a comparison function is...

```c
void ac_task_output_compare(ac_task_t *task, ac_io_compare_f compare,
                            void *compare_tag);
```

The compare function has the prototype
```c
int compare(ac_io_record_t *p1, ac_io_record_t *p2, void *tag);
```

and can be borrowed from Part 1
```c
int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}
```

The compare\_tokens can now be configured in setup\_text\_to\_tokens.
```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens, NULL);
  ac_task_output_partition(task, partition_by_token, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ...
}
```

Now running this..
```
$ make part_1_4
$ ./part_1_4 -f
Finished text_to_tokens[0] on thread 3 in 1.197ms
Finished text_to_tokens[1] on thread 2 in 1.860ms
Finished token_aggregator[0] on thread 2 in 0.000ms
Finished token_aggregator[1] on thread 1 in 0.000ms
Finished dump_tokens[0] on thread 1 in 0.000ms
$ ./part_1_3 -d tasks/text_to_tokens_1/tokens_*.lz4
1	be
1	be
1	not
1	to
1	to
1	or
```

Instead of removing the tasks folder, the -f option was to used to force rerunning of all of the tasks.  The data dumped shows that it is sorted.  "or" is out of order because it comes from a separate file.

## Reducing Sorted Output

Again, our goal for this task was that it would write sorted and reduced output.  To reduce the sorted output from the last section, a reducer function must be specified.  The function to specify a reducer function is...

```c
void ac_task_output_reducer(ac_task_t *task, ac_io_reducer_f reducer,
                            void *reducer_tag);
```

The reducer function has the prototype
```c
bool reduce(ac_io_record_t *res, const ac_io_record_t *r,
            size_t num_r, ac_buffer_t *bh, void *arg);
```

Borrow the reduce function from Part 1 sort\_tokens\_reduce\_sort\_by\_freq\_and\_display.c
```c
bool reduce_frequency(ac_io_record_t *res, const ac_io_record_t *r,
                      size_t num_r, ac_buffer_t *bh, void *arg) {
  *res = r[0];
  uint32_t total = 0;
  for (size_t i = 0; i < num_r; i++)
    total += (*(uint32_t *)r[i].record);
  (*(uint32_t *)res->record) = total;
  return true;
}
```

The reduce\_frequency can now be configured in setup\_text\_to\_tokens.
```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens, NULL);
  ac_task_output_reducer(task, reduce_frequency, NULL);
  ac_task_output_partition(task, partition_by_token, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ...
}
```

Now running this..
```
$ make part_1_5
$ ./part_1_5 -f
Finished text_to_tokens[0] on thread 3 in 1.197ms
Finished text_to_tokens[1] on thread 2 in 1.860ms
Finished token_aggregator[0] on thread 2 in 0.000ms
Finished token_aggregator[1] on thread 1 in 0.000ms
Finished dump_tokens[0] on thread 1 in 0.000ms
$ ./part_1_5 -d tasks/text_to_tokens_*/token*
2	be
1	not
2	to
1	or
```

Instead of removing the tasks folder, the -f option was to used to force rerunning of all of the tasks.  The data dumped shows that it is sorted and reduced (be and to have a frequency of 2).  "or" is out of order because it is from a different file.

## The Second Task

text\_to\_tokens has been setup.  Now it's time to setup the token\_aggregator.  The token aggregator will take input and then sort it by frequency descending and token ascending.  The compare\_tokens\_by\_frequency function can be borrowed from Part 1.

```c
int compare_tokens_by_frequency(const ac_io_record_t *r1,
                                const ac_io_record_t *r2, void *arg) {
  uint32_t *a = (uint32_t *)r1->record;
  uint32_t *b = (uint32_t *)r2->record;
  if (*a != *b)
    return (*a < *b) ? 1 : -1; // descending
  return strcmp((char *)(a + 1), (char *)(b + 1));
}
```

For this task, the output will not be partitioned or reduced, so setting those functions will be unnecessary. The output should still be able to be dumped and will have a prefix format.  The setup\_token\_aggregator will change from...
```c
bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);

  ac_task_runner(task, do_nothing);
  return true;
}
```

to
```c
bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens_by_frequency, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ac_task_runner(task, do_nothing);
  return true;
}
```

The last thing to do is to replace the do\_nothing with a function which will convert the input to output.  The function will look like the following.
```c
bool token_aggregator(ac_worker_t *w) {
  ac_in_t *in = ac_worker_in(w, 0);
  ac_out_t *out = ac_worker_out(w, 0);

  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    ac_out_write_record(out, r->record, r->length);

  ac_in_destroy(in);
  ac_out_destroy(out);
  return true;
}
```

Finally, token\_aggregator must be wired up to the setup\_token\_aggregator.

```c
bool setup_token_aggregator(ac_task_t *task) {
  ...
  ac_task_runner(task, token_aggregator);
  ...
}
```

You may be wondering how the input is computed for token\_aggregator.  The input is defined by the ac\_task\_output call in setup\_text\_to\_tokens when token\_aggregator is listed as a destination.  Any additional parameters further defining the input for token\_aggregator should be defined along with the ac\_task\_output call.

```c
bool setup_text_to_tokens(ac_task_t *task) {
  ...
  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
```

Running this...
```
$ make part_1_6
$ ./part_1_6 -f
$ ./part_1_6 -d tasks/token_aggregator_0/tokens_by_frequency_0.lz4
2	be
2	to
1	not
$ ./part_1_6 -d tasks/token_aggregator_1/tokens_by_frequency_1.lz4
1	or
```

The individual files are sorted by frequency descending and token ascending.


## The Final Task

dump\_tokens should take the inputs from token\_aggregator and print them to the terminal.  This function will not output anything.  To finish it, the do\_nothing function will need replaced.
```c
bool dump_tokens(ac_worker_t *w) {
  ac_in_t *in = ac_worker_in(w, 0);

  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    printf("%u\t%s", *(uint32_t *)(r->record), r->record + sizeof(uint32_t));
  ac_in_destroy(in);
  return true;
}
```

and dump\_tokens will have to be used in setup\_dump\_tokens.
```c
bool setup_dump_tokens(ac_task_t *task) {
  ac_task_runner(task, dump_tokens);
  return true;
}
```

do\_nothing is no longer needed, so the function can be removed.  

Running the task...
```
$ make part_1_7
$ ./part_1_7 -f
Finished text_to_tokens[0] on thread 1 in 1.200ms
Finished text_to_tokens[1] on thread 0 in 1.422ms
Finished token_aggregator[0] on thread 0 in 0.363ms
Finished token_aggregator[1] on thread 3 in 0.404ms
2	be
2	to
1	not
1	or
Finished dump_tokens[0] on thread 3 in 0.244ms
```

If the lines which state Finished are not desired, the following line can be changed in the main function.
```c
ac_schedule_run(scheduler, ac_worker_complete);
```

to
```c
ac_schedule_run(scheduler, NULL);
```

Now, if run again.
```
$ make part_1_8
$ ./part_1_8 -f
2	be
2	to
1	not
1	or
$ ./part_1_8
$
```

Perhaps, we would want the dump\_tokens task to run everytime.  Add the following to setup\_dump\_tokens.

```c
bool setup_dump_tokens(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_runner(task, dump_tokens);
  return true;
}
```

```
$ make part_1_9
$ ./part_1_9
2	be
2	to
1	not
1	or
$ ./part_1_9
2	be
2	to
1	not
1	or
```

As you can see, the task is run everytime.  If we put back the Finished.. line, it'll be a little clearer.

```
$ make part_1_10
$ ./part_1_10
2	be
2	to
1	not
1	or
Finished dump_tokens[0] on thread 0 in 0.209ms
```

Only dump\_tokens runs everytime.


## Limiting Input

Sometimes it can be handy to limit total input.  All of the tasks can be reset with the -f and we can change the extensions and directory.

```
$ make part_1_10
$ ./part_1_10 --dir .. --ext c -f
[ERROR] Command line arguments don't match (../.. != ..) - (use --new-args?)
...
$ ./part_1_10 --dir .. --ext c -f --new-args
Finished text_to_tokens[0] on thread 1 in 18.612ms
Finished text_to_tokens[1] on thread 0 in 20.159ms
Finished token_aggregator[0] on thread 0 in 0.425ms
Finished token_aggregator[1] on thread 3 in 0.851ms
509	char
498	0
497	return
439	task
371	ca
362	s
321	w
302	bh
286	int
268	argv
...
1	unistd
1	unknownfirstname
1	unknownlastname
1	unknownregion
1	unknownstate
1	up
1	uppercased
1	used
1	via
1	zero
Finished dump_tokens[0] on thread 0 in 0.744ms
```

ac\_task\_input\_limit can be used to limit how many records will pass through the input of the next function (dump\_tokens).

```c
bool setup_token_aggregator(ac_task_t *task) {
  ...
  ac_task_input_limit(task, 10);
  ac_task_runner(task, token_aggregator);
  return true;
}
```

Now the top 10 records will be shown.
```
$ make part_1_11
$ ./part_1_11
509	char
498	0
497	return
439	task
371	ca
362	s
321	w
302	bh
286	int
268	argv
Finished dump_tokens[0] on thread 3 in 0.216ms
```
