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

/examples/mapreduce3/part\_1\_11.c
```c
#include "ac_conv.h"
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

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

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

bool dump_tokens(ac_worker_t *w) {
  ac_in_t *in = ac_worker_in(w, 0);

  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    printf("%u\t%s\n", *(uint32_t *)(r->record), r->record + sizeof(uint32_t));
  ac_in_destroy(in);
  return true;
}

void dump_term_frequency(ac_worker_t *w, ac_io_record_t *r, ac_buffer_t *bh,
                         void *arg) {
  ac_buffer_appendf(bh, "%u\t%s", *(uint32_t *)(r->record),
                    r->record + sizeof(uint32_t));
}

size_t partition_by_token(const ac_io_record_t *r, size_t num_part, void *tag) {
  char *token = r->record + sizeof(uint32_t);
  uint64_t hash = ac_hash64(token, r->length - sizeof(uint32_t));
  return hash % num_part;
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

bool setup_text_to_tokens(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_dump(task, ac_task_dump_text, NULL);

  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens, NULL);
  ac_task_output_reducer(task, reduce_frequency, NULL);
  ac_task_output_partition(task, partition_by_token, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ac_task_runner(task, text_to_tokens);
  return true;
}

bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens_by_frequency, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ac_task_input_limit(task, 10);
  ac_task_runner(task, token_aggregator);
  return true;
}

bool setup_dump_tokens(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_runner(task, dump_tokens);
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

## Using the default runner

Up to this point we have used ac\_task\_runner in our setup functions to do our work.  ac\_schedule defines another function named ac\_task\_default\_runner which is meant to replace common I/O related tasks.

The first task's runner is below.
```c
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

bool setup_text_to_tokens(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ...

  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ...

  ac_task_runner(task, text_to_tokens);
  return true;
}
```

This function reads a record at a time from input and transforms it into output.  The default task runner expects one or more transformations to be defined.  If there is only one input file, then a callback function can be defined for each record, and most of the code above can be eliminated.

```c
void write_tokens(ac_worker_t *w, ac_io_record_t *r, ac_out_t **outs) {
  ac_buffer_t *bh = w->bh;
  lowercase(r->record);
  size_t num_tokens = 0;
  char **tokens = ac_pool_tokenize(w->pool, &num_tokens, TO_SPLIT_ON, r->record);
  uint32_t one = 1;
  for (size_t i = 0; i < num_tokens; i++) {
    ac_buffer_set(bh, &one, sizeof(one));
    ac_buffer_appends(bh, tokens[i]);
    ac_out_write_record(outs[0], ac_buffer_data(bh), ac_buffer_length(bh));
  }
}

bool setup_text_to_tokens(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ...

  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ...

  ac_task_default_runner(task);
  ac_task_transform(task, "split_input", "tokens.lz4", write_tokens);
  return true;
}
```

For the second task, we can eliminate more code.

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

bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ...
  ac_task_runner(task, token_aggregator);
  return true;
}
```

becomes
```c
bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ...
  ac_task_default_runner(task);
  ac_task_transform(task, "tokens.lz4", "tokens_by_frequency.lz4", NULL);
  return true;
}
```

For this transform, NULL was used as the callback (and token\_aggregator can be entirely removed)!  If NULL is specified and there is one input, the input will be written to all of the outputs specified.  Multiple inputs and outputs can be specified by separating them by vertical bars.

For the last task, a little code can be removed.
```c
bool dump_tokens(ac_worker_t *w) {
  ac_in_t *in = ac_worker_in(w, 0);

  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    printf("%u\t%s\n", *(uint32_t *)(r->record), r->record + sizeof(uint32_t));
  ac_in_destroy(in);
  return true;
}

bool setup_dump_tokens(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_runner(task, dump_tokens);
  return true;
}
```

becomes
```c
void dump_token(ac_worker_t *w, ac_io_record_t *r, ac_out_t **outs) {
  printf("%u\t%s\n", *(uint32_t *)(r->record), r->record + sizeof(uint32_t));
}

bool setup_dump_tokens(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_default_runner(task);
  ac_task_transform(task, "tokens_by_frequency.lz4", NULL, dump_token);
  return true;
}
```

In this case, NULL was specified for outputs which means that no outputs are opened.  For each record in the input, dump_token is called.

The program should work in a similar manner
```c
$ rm -rf tasks
$ make part_1_12
$ ./part_1_12 --dir ../.. --ext c,h,md | head
Finished text_to_tokens[1] on thread 1 in 1208.007ms
Finished text_to_tokens[0] on thread 0 in 1282.818ms
Finished token_aggregator[0] on thread 2 in 5.741ms
Finished token_aggregator[1] on thread 3 in 5.890ms
91209	the
60316	a
56671	https
51436	to
47033	com
43033	0
36492	github
35460	of
34704	1
31457	for
```

## Increasing Partitions

We can increase the number of partitions in the main function and see how our program runs.  If all is good, the output should be the same.

```c
ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 2, 4, 10);
```

gets changed to
```c
ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 16, 16, 10);
```

Which changes from using 2 partitions and 4 cpus to 16 partitions and 16 cpus.

```
$ make part_1_13
$ ./part_1_13 --dir ../.. --ext c,h,md | head
Finished text_to_tokens[7] on thread 5 in 229.493ms
Finished text_to_tokens[8] on thread 3 in 232.452ms
Finished text_to_tokens[10] on thread 11 in 234.440ms
Finished text_to_tokens[15] on thread 14 in 241.641ms
Finished text_to_tokens[11] on thread 7 in 243.947ms
Finished text_to_tokens[14] on thread 15 in 243.971ms
Finished text_to_tokens[3] on thread 6 in 246.909ms
Finished text_to_tokens[6] on thread 10 in 249.808ms
Finished text_to_tokens[0] on thread 0 in 255.357ms
Finished text_to_tokens[13] on thread 13 in 264.330ms
Finished text_to_tokens[1] on thread 1 in 271.022ms
Finished text_to_tokens[2] on thread 8 in 271.306ms
Finished text_to_tokens[5] on thread 2 in 275.768ms
Finished text_to_tokens[9] on thread 9 in 279.335ms
Finished text_to_tokens[4] on thread 4 in 298.948ms
Finished text_to_tokens[12] on thread 12 in 384.969ms
Finished token_aggregator[3] on thread 11 in 16.168ms
Finished token_aggregator[6] on thread 15 in 17.610ms
Finished token_aggregator[1] on thread 5 in 18.026ms
Finished token_aggregator[2] on thread 3 in 18.426ms
Finished token_aggregator[5] on thread 7 in 18.588ms
Finished token_aggregator[7] on thread 10 in 18.561ms
Finished token_aggregator[4] on thread 14 in 19.046ms
Finished token_aggregator[14] on thread 9 in 19.623ms
Finished token_aggregator[9] on thread 0 in 20.400ms
Finished token_aggregator[10] on thread 13 in 20.864ms
Finished token_aggregator[8] on thread 6 in 21.356ms
Finished token_aggregator[12] on thread 8 in 21.423ms
Finished token_aggregator[11] on thread 1 in 22.145ms
Finished token_aggregator[13] on thread 2 in 22.307ms
Finished token_aggregator[15] on thread 4 in 26.595ms
Finished token_aggregator[0] on thread 12 in 31.597ms
91209	the
60316	a
56671	https
51436	to
47033	com
43033	0
36492	github
35460	of
34704	1
31457	for
```

is the same as the result from the last section
```
91209	the
60316	a
56671	https
51436	to
47033	com
43033	0
36492	github
35460	of
34704	1
31457	for
```

So increasing partitions, doesn't alter the end result.  Obviously, many other tests could be run to validate the results, but this is a good one!  The full code is below.

/examples/mapreduce3/part\_1\_13.c
```c
#include "ac_conv.h"
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

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void write_tokens(ac_worker_t *w, ac_io_record_t *r, ac_out_t **outs) {
  ac_buffer_t *bh = w->bh;
  lowercase(r->record);
  size_t num_tokens = 0;
  char **tokens =
      ac_pool_tokenize(w->pool, &num_tokens, TO_SPLIT_ON, r->record);
  uint32_t one = 1;
  for (size_t i = 0; i < num_tokens; i++) {
    ac_buffer_set(bh, &one, sizeof(one));
    ac_buffer_appends(bh, tokens[i]);
    ac_out_write_record(outs[0], ac_buffer_data(bh), ac_buffer_length(bh));
  }
}

void dump_token(ac_worker_t *w, ac_io_record_t *r, ac_out_t **outs) {
  printf("%u\t%s\n", *(uint32_t *)(r->record), r->record + sizeof(uint32_t));
}

void dump_term_frequency(ac_worker_t *w, ac_io_record_t *r, ac_buffer_t *bh,
                         void *arg) {
  ac_buffer_appendf(bh, "%u\t%s", *(uint32_t *)(r->record),
                    r->record + sizeof(uint32_t));
}

size_t partition_by_token(const ac_io_record_t *r, size_t num_part, void *tag) {
  char *token = r->record + sizeof(uint32_t);
  uint64_t hash = ac_hash64(token, r->length - sizeof(uint32_t));
  return hash % num_part;
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

bool setup_text_to_tokens(ac_task_t *task) {
  ac_task_input_files(task, "split_input", 0.1, get_input_files);
  ac_task_input_format(task, ac_io_delimiter('\n'));
  ac_task_input_dump(task, ac_task_dump_text, NULL);

  ac_task_output(task, "tokens.lz4", "token_aggregator", 0.9, 0.1,
                 AC_OUTPUT_SPLIT);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens, NULL);
  ac_task_output_reducer(task, reduce_frequency, NULL);
  ac_task_output_partition(task, partition_by_token, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ac_task_default_runner(task);
  ac_task_transform(task, "split_input", "tokens.lz4", write_tokens);
  return true;
}

bool setup_token_aggregator(ac_task_t *task) {
  ac_task_output(task, "tokens_by_frequency.lz4", "dump_tokens", 0.9, 0.1,
                 AC_OUTPUT_NORMAL);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_compare(task, compare_tokens_by_frequency, NULL);
  ac_task_output_dump(task, dump_term_frequency, NULL);
  ac_task_input_limit(task, 10);
  ac_task_default_runner(task);
  ac_task_transform(task, "tokens.lz4", "tokens_by_frequency.lz4", NULL);
  return true;
}

bool setup_dump_tokens(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_default_runner(task);
  ac_task_transform(task, "tokens_by_frequency.lz4", NULL, dump_token);
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
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 16, 16, 10);
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

## Debugging a Task

The AC's map/reduce framework allows for debugging individual partitions of tasks in an isolated environment.  

```
$ ./part_1_13 --debug token_aggregator:0 debug
$ ls debug/
tokens_by_frequency_0.lz4
```

This can be used to run any task/partition and have the output placed in the debug directory (or whatever directory you choose).  To dump the tokens_by_frequency_0.lz4, run the dump command as follows.

```
./part_1_13 --debug token_aggregator:0 debug -d debug/tokens* | head
56672	 https
13554	 define
10491	 not
8645	 add
6034	 svg
5024	 compare
4914	 gatsbyjs
4646	 include
2648	 types
2348	 gpointer
```

An additional parameter dump\_input can be optionally passed to the --debug option to dump the input files.

```
$ ./part_1_13 --debug token_aggregator:0 debug dump_input | head
3	 $and
5	 $comment
9	 $exact
1	 $foo~bar
7	 $merge
27	 $readonlyarray
3	 $replacement$
2	 $tmp
5	 $visual
2	 0000ff66
```

Using lldb, you can set the following breakpoints.
```
$ lldb ./part_1_13
(lldb) target create "./part_1_13"
Current executable set to './part_1_13' (x86_64).
(lldb) br set --name debug_task
Breakpoint 1: where = part_1_13`debug_task + 19 at ac_schedule.c:1532:21, address = 0x000000010009f3d3
(lldb) r --debug token_aggregator:0 debug dump_input
Process 83008 launched: '/Users/ac/code/website/ac/examples/mapreduce3/part_1_13' (x86_64)
Process 83008 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
    frame #0: 0x000000010009f3d3 part_1_13`debug_task(h=0x00000001001b5898) at ac_schedule.c:1532:21
   1529	}
   1530
   1531	void debug_task(ac_schedule_thread_t *h) {
-> 1532	  ac_pool_t *pool = ac_pool_init(65536);
   1533	  h->bh = ac_buffer_init(200);
   1534	  ac_pool_t *tmp_pool = ac_pool_init(65536);
   1535	  ac_buffer_t *bh = ac_buffer_init(1024);
Target 0: (part_1_13) stopped.
(lldb) br set --name run_worker
Breakpoint 2: where = part_1_13`run_worker + 12 at ac_schedule.c:1255:8, address = 0x000000010009f6dc
(lldb) cont
* thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 2.1
    frame #0: 0x000000010009f6dc part_1_13`run_worker(w=0x00000001001be080) at ac_schedule.c:1255:8
   1252
   1253	static void setup_worker(ac_worker_t *w) {}
   1254	static bool run_worker(ac_worker_t *w) {
-> 1255	  bool r = true;
   1256	  w->timer = ac_timer_init(1);
   1257	  ac_timer_start(w->timer);
   1258	  if (w->task->runner)
(lldb) n
(lldb)
Process 83008 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step over
    frame #0: 0x000000010009f719 part_1_13`run_worker(w=0x00000001001be080) at ac_schedule.c:1259:9
   1256	  w->timer = ac_timer_init(1);
   1257	  ac_timer_start(w->timer);
   1258	  if (w->task->runner)
-> 1259	    r = w->task->runner(w);
   1260	  ac_timer_stop(w->timer);
   1261	  return true;
   1262	}
(lldb) s
Process 83008 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = step in
    frame #0: 0x00000001000a0d0f part_1_13`in_out_runner(w=0x00000001001be080) at ac_schedule.c:1831:50
   1828	}
   1829
   1830	static bool in_out_runner(ac_worker_t *w) {
-> 1831	  ac_transform_t *transforms = (ac_transform_t *)w->data;
   1832	  ac_in_t *in = NULL;
   1833	  while (transforms) {
   1834	    size_t num_outs = transforms->num_outputs;
Target 0: (part_1_13) stopped.
```

The in_out_runner is the default runner.  From here, it should be pretty straight forward to step through your individual task.  You can also set breakpoints in your configured code.  

## Further Help

I plan on continuing to write more posts to further explain the AC map/reduce library.  There is a number of additional functions in ac\_schedule.h, ac\_in.h, ac\_out.h, and ac\_io.h which should be fairly well commented in github and will soon have examples on this website.
