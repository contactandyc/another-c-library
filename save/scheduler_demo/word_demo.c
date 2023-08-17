#include "ac_schedule.h"

size_t num_inputs = 0;
ac_io_file_info_t *inputs = NULL;

//#define USE_CHAIN

/* By defining the filenames, it is easier to change them as needed.  Also,
   if .lz4 or .gz file extensions are used, the files will be compressed as
   such!  lz4 is often faster than uncompressed and takes less space, so
   it is highly recommended. */
#define SPLIT_WORDS "split_words.lz4"
#define COUNT_WORDS "count_words.lz4"
#define TMP_WORDS "tmp"
#define TOKENS_TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

/* In order to provide outside input, a callback function is provided to
   identify which files belong with each piece of work (or partition of a
   task).
*/
ac_io_file_info_t *get_input_files(ac_worker_t *w, size_t *num_files,
                                   ac_worker_input_t *inp) {
  size_t num = (num_inputs / w->num_partitions) + 1;
  ac_io_file_info_t *res = (ac_io_file_info_t *)ac_pool_alloc(
      w->pool, num * sizeof(ac_io_file_info_t));
  ac_io_file_info_t *rp = res;
  for (size_t i = w->partition; i < num_inputs; i += w->num_partitions) {
    *rp = inputs[i];
    rp++;
  }
  *num_files = rp - res;
  return res;
}

/* For each input record, write each word within the given line with a
   frequency of 1.  The reducer will accumulate the final word frequency
   later on.
*/
void write_words(ac_worker_t *w, ac_io_record_t *r, ac_out_t **outs) {
  ac_pool_clear(w->pool);
  size_t num_tokens = 0;
  char **tokens =
      ac_pool_tokenize(w->pool, &num_tokens, TOKENS_TO_SPLIT_ON, r->record);
  ac_buffer_clear(w->bh);
  uint32_t one = 1;
  for (size_t i = 0; i < num_tokens; i++) {
    ac_buffer_set(w->bh, &one, sizeof(one));
    ac_buffer_appends(w->bh, tokens[i]);
    ac_out_write_record(outs[0], ac_buffer_data(w->bh),
                        ac_buffer_length(w->bh));
  }
}

/* In order to find word frequency, words need to be first sorted together. */
int compare_words(const ac_io_record_t *p1, const ac_io_record_t *p2,
                  void *arg) {
  char *a = p1->record + sizeof(uint32_t);
  char *b = p2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

/* To accumulate totals for each word.  It should be noted that this may be
   called more than once for a given word (and the number associated with
   each record may be greater than one).
*/
bool reduce_word_count(ac_io_record_t *res, const ac_io_record_t *r,
                       size_t num_r, ac_buffer_t *bh, void *tag) {
  *res = *r;
  uint32_t total = 0;
  for (size_t i = 0; i < num_r; i++) {
    uint32_t *v = (uint32_t *)r[i].record;
    total += *v;
  }
  (*(uint32_t *)r[0].record) = total;
  return true;
}

/* To sort words by descending frequency. */
int compare_word_freq(const ac_io_record_t *p1, const ac_io_record_t *p2,
                      void *arg) {
  uint32_t f1 = (*(uint32_t *)p1->record);
  uint32_t f2 = (*(uint32_t *)p2->record);
  if (f1 != f2)
    return (f1 < f2) ? 1 : -1;
  char *a = p1->record + sizeof(uint32_t);
  char *b = p2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

/* The default ac_io_hash_partition allows an offset into the record to be
   passed to it to skip the first N bytes.  In this case, the words should
   be hashed, but not the first 4 byte frequency. */
size_t hash_offs = 4;

/* In order to debug, dump the contents of word count record to a line. */
void dump_word_count(ac_worker_t *w, ac_io_record_t *r, ac_buffer_t *bh,
                     void *arg) {
  ac_buffer_appendf(bh, "%u\t%s", (*(uint32_t *)r->record),
                    r->record + sizeof(uint32_t));
}

/* split_words takes the raw input and transforms it to word count records.  If
   USE_CHAIN is defined, the output of this task will be sorted and reduced.
   Otherwise, the output will be unsorted words each with an associated one
   frequency or count.  The output is split into N pieces where N is the
   number of partitions defined in the ac_schedule_init call in main.  The
   output is directed to the count_words task.  If count_words was not a
   partitioned task (only a single partition), then the output would not be
   split.  */
bool setup_split_words(ac_task_t *task) {
  /* The name of the input is used in the ac_task_transform call below.  The
     0.35 means that the input can use 35% of the ram associated with the given
     job.  get_input_files is a callback to get input files for each partition
     of split_words. */
  ac_task_input_files(task, "input", 0.35, get_input_files);
  /* The input is delimited with newlines. */
  ac_task_input_format(task, ac_io_delimiter('\n'));
  /* To dump the input, use the ac_task_dump_text method defined in
     ac_schedule.h */
  ac_task_input_dump(task, ac_task_dump_text, NULL);
  /* The input can be limited, making all of the subsequent tasks much faster.
     This can be useful for debugging. */
  // ac_task_input_limit(task, 100);

  /* Define output and direct it to count_words.  The 0.65 means that 65% of
     the ram can be used for the output writer.  The 0.10 means that 10% of
     the ram can be used for reading the input in count_words.  This output
     creates an input named SPLIT_WORDS for the task count_words. */
  ac_task_output(task, SPLIT_WORDS, "count_words", 0.65, 0.10,
                 AC_OUTPUT_KEEP | AC_OUTPUT_SPLIT);
  /* The file will be formatted with a 32bit length followed by length bytes. */
  ac_task_output_format(task, ac_io_prefix());
  /* Used to dump the input - for debugging purposes. */
  ac_task_output_dump(task, dump_word_count, NULL);
  /* Used to split the output into the number of partitions that count_words
     is configured to have. */
  ac_task_output_partition(task, ac_io_hash_partition, &hash_offs);
  /* When data is sorted, this can speed up the process. */
  // ac_task_output_use_extra_thread(task);
#ifndef USE_CHAIN
  /* If chaining is defined, sort the output using the compare_words compare
     method and the reduce_word_count reducer method. */
  ac_task_output_compare(task, compare_words, NULL);
  ac_task_output_reducer(task, reduce_word_count, NULL);
#endif
  /* Use the default runner */
  ac_task_default_runner(task);
  /* Transform "input" to SPLIT_WORDS using the write_words callback to
     process each input record. */
  ac_task_transform(task, "input", SPLIT_WORDS, write_words);
  /* The setup succeeded! */
  return true;
}

#ifdef USE_CHAIN
/* If chaining is being used, the input is not sorted.  This will sort and
   reduce the input and then chain the result of the sort into a new sort
   which will count the frequency of each word and direct the output to
   dump_word_count.  For chaining to work, the output must not be partitioned.
   The third parameter of the first ac_task_output should be NULL to indicate
   that the data doesn't have a destination.  If there is more than one output,
   only the first output is chained to the next transformation.  The other
   outputs may specify a destination.
*/
bool setup_count_words(ac_task_t *task) {
  /* Define all of the outputs and then hook them together at the end. */
  ac_task_output(task, TMP_WORDS, NULL, 0.4, 0.10, AC_OUTPUT_KEEP);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_dump(task, dump_word_count, NULL);
  ac_task_output_use_extra_thread(task);
  ac_task_output_compare(task, compare_words, NULL);
  ac_task_output_reducer(task, reduce_word_count, NULL);

  ac_task_output(task, COUNT_WORDS, "dump_word_count", 0.4, 0.10,
                 AC_OUTPUT_KEEP);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_dump(task, dump_word_count, NULL);
  // ac_task_output_use_extra_thread(task);
  ac_task_output_compare(task, compare_word_freq, NULL);
  ac_task_input_limit(task, 100);

  ac_task_default_runner(task);
  /* first transform, sorts by word, and reduces frequency */
  ac_task_transform(task, SPLIT_WORDS, TMP_WORDS, NULL);
  /* second transform takes input from first and sorts by frequency, word */
  ac_task_transform(task, NULL, COUNT_WORDS, NULL);
  return true;
}
#else
/* In this case, chaining is not used and the input is sorted and reduced
   prior to reaching count_words.  Note that the input is defined as a
   set of files and has a reducer that is set in split_words. */
bool setup_count_words(ac_task_t *task) {
  ac_task_output(task, COUNT_WORDS, "dump_word_count", 0.4, 0.10,
                 AC_OUTPUT_KEEP);
  ac_task_output_format(task, ac_io_prefix());
  ac_task_output_dump(task, dump_word_count, NULL);
  ac_task_output_compare(task, compare_word_freq, NULL);
  /* Specify a limit on the number of records to use from this output in the
     next task. */
  ac_task_input_limit(task, 100);

  ac_task_default_runner(task);
  ac_task_transform(task, SPLIT_WORDS, COUNT_WORDS, NULL);
  return true;
}
#endif

void dump_word_counts(ac_worker_t *w, ac_io_record_t *r, ac_out_t **outs) {
  uint32_t frequency = (*(uint32_t *)(r->record));
  char *word = r->record + sizeof(uint32_t);
  printf("%u\t%s\n", frequency, word);
}

/* dump_word_count doesn't actually output to a file. Because of this, it
   should run everytime so the output can always be available. */
bool setup_dump_word_count(ac_task_t *task) {
  ac_task_run_everytime(task);
  ac_task_default_runner(task);
  ac_task_transform(task, COUNT_WORDS, NULL, dump_word_counts);
  return true;
}

bool check_file_extensions(const char *filename) {
  if (ac_io_extension(filename, ".h") || ac_io_extension(filename, ".c") ||
      ac_io_extension(filename, ".md"))
    return true;
  return false;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "Find all words ending in .h, .c, and .md and sort by\n");
    fprintf(stderr, "frequency descending.\n\n");
    fprintf(stderr, "Run ./word_demo -h for help\n");
  }
  char *input_dir = "..";
  if (argc > 1 && argv[1][0] != '-') {
    input_dir = argv[1];
    argc--;
    argv++;
  }
  /* Create a list of all of the files which satisfy the check_file_extensions
     rule. */
  inputs = ac_io_list(input_dir, &num_inputs, check_file_extensions);

  /* The scheduler has its own usage handling.  I'm planning on allowing for
     a user customizable usage statement for parameters that are not in
     conflict with the scheduler. Specify how many partitions, default
     number of cpus, default MB of RAM. */
  ac_schedule_t *scheduler = ac_schedule_init(argc - 1, argv + 1, 4, 4, 1000);
  ac_schedule_task_dir(scheduler, "word_demo_tasks");

  /* Define each of the tasks with a callback to finish defining the tasks.
     The third parameter indicates whether the task is partitioned or not.
   */
  ac_schedule_task(scheduler, "split_words", true, setup_split_words);
  ac_schedule_task(scheduler, "count_words", true, setup_count_words);
  ac_schedule_task(scheduler, "dump_word_count", false, setup_dump_word_count);

  /* Run the tasks, show the plan, or dump the contents of files produced. */
  ac_schedule_run(scheduler, ac_worker_complete);

  /* Cleanup */
  ac_schedule_destroy(scheduler);
  ac_free(inputs);
  return 0;
}
