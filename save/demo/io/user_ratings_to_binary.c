#include "ac_allocator.h"
#include "ac_in.h"
#include "ac_out.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int32_t user_id;
  int32_t item_id;
  int32_t rating;
  int32_t date;
} entry_t;

void entry_text_to_binary(const char *in_path, int num_in,
                          const char *out_file) {
  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_format(&in_opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_fixed(sizeof(entry_t)));
  ac_out_t *out = ac_out_init(out_file, &out_opts);

  char *filename = (char *)ac_malloc(strlen(in_path) + 30);
  for (int i = 1; i <= num_in; i++) {
    sprintf(filename, "%s_%07d.txt", in_path, i);
    ac_in_t *in = ac_in_init(filename, &in_opts);
    ac_io_record_t *r = ac_in_advance(in);
    if (r) {
      int user_id, rating, year, month, day;
      while ((r = ac_in_advance(in))) {
        if (sscanf(r->record, "%d,%d,%d-%d-%d", &user_id, &rating, &year,
                   &month, &day) != 5)
          abort();
        entry_t entry;
        entry.date = (year * 10000) + (month * 100) + day;
        entry.rating = rating;
        entry.user_id = user_id;
        entry.item_id = i;
        ac_out_write_record(out, &entry, sizeof(entry));
      }
    }
    ac_in_destroy(in);
  }
  ac_free(filename);
  ac_out_destroy(out);
}

size_t split_by_user(const ac_io_record_t *r, size_t num_part, void *tag) {
  entry_t *entry = (entry_t *)r->record;
  return entry->user_id % num_part;
}

void split_entry(const char *out_file, int num_out) {
  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_format(&in_opts, ac_io_fixed(sizeof(entry_t)));
  ac_in_t *in = ac_in_init(out_file, &in_opts);

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_fixed(sizeof(entry_t)));

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_partition(&out_ext_opts, split_by_user, NULL);
  ac_out_ext_options_num_partitions(&out_ext_opts, num_out);
  ac_out_t *out = ac_out_ext_init(out_file, &out_opts, &out_ext_opts);

  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    ac_out_write_record(out, r->record, r->length);
  ac_out_destroy(out);
  ac_in_destroy(in);
  remove(out_file);
}

int usage(const char *prog) {
  printf("%s <input_prefix> <num_inputs> <output_file> [output_split]\n", prog);
  printf(
      "Each input is expected to be numbered with the number corresponding\n");
  printf("to the item id.  The format of the input files should be\n");
  printf("user_id,rating,year-month-day\n");
  printf("\n");
  printf("The input_prefix should look like <path>/<prefix> and will have\n");
  printf("_<item_id>.txt appended to it.\n");
  return -1;
}

int main(int argc, char *argv[]) {
  if (argc < 4)
    return usage(argv[0]);

  char *prefix = argv[1];
  char *output = argv[3];

  int num_in = 0;
  if (sscanf(argv[2], "%d", &num_in) != 1)
    return usage(argv[0]);

  int num_out = 1;
  if (argc > 4 && sscanf(argv[4], "%d", &num_out) != 1)
    return usage(argv[0]);

  entry_text_to_binary(prefix, num_in, output);

  if (num_out > 1)
    split_entry(output, num_out);

  return 0;
}
