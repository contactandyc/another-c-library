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

#ifndef _ac_io_H
#define _ac_io_H

#include "ac_common.h"

size_t ac_io_file_size(const char *filename);

/* char *ac_io_read_file(size_t *len, const char *filename); */

#ifdef _AC_DEBUG_MEMORY_
#define ac_io_read_file(len, filename)                                         \
  _ac_io_read_file(len, filename, AC_FILE_LINE_MACRO("ac_io_read_file"))
char *_ac_io_read_file(size_t *len, const char *filename, const char *caller);
#else
#define ac_io_read_file(len, filename) _ac_io_read_file(len, filename)
char *_ac_io_read_file(size_t *len, const char *filename);
#endif

#endif
