#include "ac_allocator.h"
#include "ac_in.h"
#include "ac_lz4.h"
#include "ac_out.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int compare_strings(ac_io_record_t *a, ac_io_record_t *b, void *tag) {
  return strcmp(a->record, b->record);
}

int main(int argc, char *argv[]) {
  if (argc < 2 || (!strcmp(argv[1], "-d") && argc < 3)) {
    printf("%s <output>\n", argv[0]);
    printf("%s -d <input> [output]\n", argv[0]);
  }
  if (argc == 2) {
    ac_out_options_t options;
    ac_out_options_init(&options);
    ac_out_options_buffer_size(&options, 16 * 1024 * 1024);
    ac_out_options_format(&options, ac_io_delimiter('\n'));
    ac_out_options_write_ack_file(&options);
    ac_out_options_gz(&options, 9);

    ac_out_ext_options_t ext_options;
    ac_out_ext_options_init(&ext_options);
    ac_out_ext_options_partition(&ext_options, ac_io_hash_partition, NULL);
    // ac_out_ext_options_num_partitions(&ext_options, 32);
    ac_out_ext_options_compare(&ext_options, compare_strings, NULL);
    // ac_out_ext_options_intermediate_group_size(&ext_options, 10);
    ac_out_ext_options_use_extra_thread(&ext_options);
    // ac_out_ext_options_sort_before_partitioning(&ext_options);
    ac_out_ext_options_num_sort_threads(&ext_options, 6);

    ac_out_t *out = ac_out_ext_init(argv[1], &options, &ext_options);

    uint32_t block_size = 80;
    char *block = (char *)ac_malloc(block_size);
    uint32_t len;
    for (int j = 0; j < 100000; j++) {
      for (int i = 'A'; i < 'Z'; i++) {
        memset(block, i, block_size);
        ac_out_write_record(out, block, block_size);
      }
    }
    ac_out_destroy(out);
    ac_free(block);
  } else {
    ac_in_options_t options;
    ac_in_options_init(&options);
    ac_in_options_buffer_size(&options, 10 * 1024 * 1024);
    ac_in_options_format(&options, ac_io_delimiter('\n'));

    ac_in_t *in = ac_in_init(argv[2], &options);
    ac_io_record_t *r;
    while ((r = ac_in_advance(in)) != NULL) {
      printf("%s\n", r->record);
    }
    ac_in_destroy(in);
  }
  return 0;
}
