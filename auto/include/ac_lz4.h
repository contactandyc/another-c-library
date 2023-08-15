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

#ifndef _ac_lz4_H
#define _ac_lz4_H

#include "ac_common.h"
#include "ac_buffer.h"

#include <inttypes.h>

uint64_t ac_lz4_hash64(const void *s, size_t len);

int ac_lz4_compress_bound(int inputSize);

size_t ac_lz4_compress_appending_to_buffer(ac_buffer_t *dest, void *src, int src_size);
bool ac_lz4_decompress_into_fixed_buffer(void *dest, int dest_size, void *src, int src_size);


enum ac_lz4_block_size_s { s64kb = 0, s256kb = 1, s1mb = 2, s4mb = 3 };
typedef enum ac_lz4_block_size_s ac_lz4_block_size_t;

struct ac_lz4_s;
typedef struct ac_lz4_s ac_lz4_t;

const char *ac_lz4_get_header(ac_lz4_t *r, uint32_t *length);
uint32_t ac_lz4_block_size(ac_lz4_t *r);
uint32_t ac_lz4_block_header_size(ac_lz4_t *r);
uint32_t ac_lz4_compressed_size(ac_lz4_t *r);

typedef struct {
  uint32_t block_size;
  uint32_t compressed_size;
  ac_lz4_block_size_t size;
  bool block_checksum;
  bool content_checksum;
  char *header;
} ac_lz4_header_t;

bool ac_lz4_check_header(ac_lz4_header_t *r, void *header,
                         uint32_t header_size);

/*
  ac_lz4_t *ac_lz4_init(int level, ac_lz4_block_size_t size,
                        bool block_checksum, bool content_checksum);

  ac_lz4_t *ac_lz4_init_decompress(void *header, uint32_t header_size);
*/

#ifdef _AC_DEBUG_MEMORY_
#define ac_lz4_init(level, size, block_checksum, content_checksum)             \
  _ac_lz4_init(level, size, block_checksum, content_checksum,                  \
               AC_FILE_LINE_MACRO("ac_lz4"))
ac_lz4_t *_ac_lz4_init(int level, ac_lz4_block_size_t size, bool block_checksum,
                       bool content_checksum, const char *caller);
#else
#define ac_lz4_init(level, size, block_checksum, content_checksum)             \
  _ac_lz4_init(level, size, block_checksum, content_checksum)
ac_lz4_t *_ac_lz4_init(int level, ac_lz4_block_size_t size, bool block_checksum,
                       bool content_checksum);
#endif

#ifdef _AC_DEBUG_MEMORY_
#define ac_lz4_init_decompress(header, header_size)                            \
  _ac_lz4_init_decompress(header, header_size,                                 \
                          AC_FILE_LINE_MACRO("ac_lz4_decompress"))
ac_lz4_t *_ac_lz4_init_decompress(void *header, uint32_t header_size,
                                  const char *caller);
#else
#define ac_lz4_init_decompress(header, header_size)                            \
  _ac_lz4_init_decompress(header, header_size)
ac_lz4_t *_ac_lz4_init_decompress(void *header, uint32_t header_size);
#endif

/* caller is expected to read block size in advance and compute src_len.  If
   block_size has high bit, block is uncompressed and high bit should be unset.
   src should not point to the byte just after block size in input stream.
*/
int ac_lz4_decompress(ac_lz4_t *l, const void *src, uint32_t src_len,
                      void *dest, uint32_t dest_len, bool compressed);

uint32_t ac_lz4_compress(ac_lz4_t *l, const void *src, uint32_t src_len,
                         void *dest, uint32_t dest_len);

uint32_t ac_lz4_compress_block(ac_lz4_t *l, const void *src, uint32_t src_len,
                               void *dest, uint32_t dest_len);

/* this will return a negative number if crc doesn't match.  dest should point
   to location for size if compressing and just after block_size if
   decompressing.  If result is non-negative, then it succeeded and read or
   wrote result byte(s).
*/
int ac_lz4_finish(ac_lz4_t *l, void *dest);

void ac_lz4_destroy(ac_lz4_t *r);

#endif
