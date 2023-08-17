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
  ac_task_runner(task, token_aggregator);
  return true;
}

bool setup_dump_tokens(ac_task_t *task) {
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

  ac_schedule_run(scheduler, NULL);

  if (custom.inputs)
    ac_free(custom.inputs);
  if (custom.extensions)
    ac_free(custom.extensions);

  ac_schedule_destroy(scheduler);
  return 0;
}
