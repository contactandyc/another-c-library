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

typedef struct {
  char *buffer;
  size_t buffer_length;
  size_t buffer_size;
  int fd;
  bool fd_owner;
  ac_io_format_t format;
  bool abort_on_error;
  int32_t tag;

  bool gz;
  bool lz4;

  size_t start_offset;
  size_t end_offset;
} ac_in_options_t;
