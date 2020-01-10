#include "aux_methods.h"

ac_in_t *open_fixed_input(const char *in_file, size_t fixed_size,
                          size_t buffer_size) {
  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_buffer_size(&in_opts, buffer_size);
  // printf("opening %d size\n", ac_io_fixed(fixed_size));
  ac_in_options_format(&in_opts, ac_io_prefix()); // ac_io_fixed(fixed_size));
  return ac_in_init(in_file, &in_opts);
}

ac_out_t *open_output(const char *out_file, size_t buffer_size,
                      ac_io_compare_f compare, ac_io_partition_f partition,
                      size_t num_out, ac_io_reducer_f reducer) {
  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_buffer_size(&out_opts, buffer_size);
  ac_out_options_format(&out_opts,
                        ac_io_prefix()); // ac_io_fixed(sizeof(entry_t)));

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_use_extra_thread(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare, NULL);
  // if (compare)
  //   ac_out_ext_options_sort_while_partitioning(&out_ext_opts);
  if (partition) {
    ac_out_ext_options_partition(&out_ext_opts, partition, NULL);
    ac_out_ext_options_num_partitions(&out_ext_opts, num_out);
  }
  if (reducer)
    ac_out_ext_options_reducer(&out_ext_opts, reducer, NULL);

  return ac_out_ext_init(out_file, &out_opts, &out_ext_opts);
}
