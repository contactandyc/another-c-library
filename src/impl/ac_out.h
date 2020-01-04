/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

struct ac_out_s;
typedef struct ac_out_s ac_out_t;

typedef struct {
  size_t buffer_size;
  bool append_mode;
  bool safe_mode;
  bool write_ack_file;
  bool abort_on_error;
  ac_io_format_t format;

  int level;
  ac_lz4_block_size_t size;
  bool block_checksum;
  bool content_checksum;

  bool gz;
  bool lz4;
  int fd;
  bool fd_owner;
} ac_out_options_t;

typedef struct {
  ac_io_partition_f partition;
  void *partition_tag;
  size_t num_partitions;

  ac_io_compare_f compare;
  void *compare_tag;

  size_t num_per_group;
  ac_io_compare_f int_compare;
  void *int_compare_tag;

  ac_io_reducer_f reducer;
  void *reducer_tag;

  ac_io_fixed_reducer_f fixed_reducer;
  void *fixed_reducer_tag;

  ac_io_fixed_compare_f fixed_compare;
  void *fixed_compare_tag;

  ac_io_fixed_sort_f fixed_sort;
  void *fixed_sort_tag;
} ac_out_ext_options_t;
