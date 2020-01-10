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

struct ac_in_s;
typedef struct ac_in_s ac_in_t;
struct ac_out_s;
typedef struct ac_out_s ac_out_t;

typedef struct {
  size_t buffer_size;
  size_t compressed_buffer_size;

  ac_io_format_t format;
  bool abort_on_error;
  bool abort_on_partial_record;
  bool abort_on_file_not_found;
  bool abort_on_file_empty;
  int32_t tag;

  bool gz;
  bool lz4;

  bool full_record_required;

  ac_io_compare_f compare;
  void *compare_tag;
  ac_io_reducer_f reducer;
  void *reducer_tag;
} ac_in_options_t;

void ac_in_empty(ac_in_t *h);
