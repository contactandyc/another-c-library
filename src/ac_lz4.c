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

#include "ac_lz4.h"

#include "lz4/lz4.h"
#include "lz4/lz4hc.h"
#include "lz4/xxhash.h"

#include "ac_allocator.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t _64kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x60, 0x40, 0x82};
static uint8_t c_64kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x64, 0x40, 0xa7};
static uint8_t b_64kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x70, 0x40, 0xad};
static uint8_t cb_64kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x74, 0x40, 0xbd};

static uint8_t _256kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x60, 0x50, 0xfb};
static uint8_t c_256kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x64, 0x50, 0x08};
static uint8_t b_256kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x70, 0x50, 0x84};
static uint8_t cb_256kb[7] = {0x04, 0x22, 0x4d, 0x18, 0x74, 0x50, 0xff};

static uint8_t _1mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x60, 0x60, 0x51};
static uint8_t c_1mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x64, 0x60, 0x85};
static uint8_t b_1mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x70, 0x60, 0x33};
static uint8_t cb_1mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x74, 0x60, 0xd9};

static uint8_t _4mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x60, 0x70, 0x73};
static uint8_t c_4mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x64, 0x70, 0xb9};
static uint8_t b_4mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x70, 0x70, 0x72};
static uint8_t cb_4mb[7] = {0x04, 0x22, 0x4d, 0x18, 0x74, 0x70, 0x8e};

struct ac_lz4_s {
  ac_lz4_block_size_t size;
  bool content_checksum;
  bool block_checksum;
  int level;

  uint8_t *header;
  uint32_t header_size;

  uint32_t block_size;
  uint32_t compressed_size;
  uint32_t block_header_size;
  XXH32_state_t xxh;
  void *ctx;
};

const char *ac_lz4_get_header(ac_lz4_t *r, uint32_t *length) {
  *length = r->header_size;
  return (const char *)r->header;
}

uint32_t ac_lz4_block_size(ac_lz4_t *r) { return r->block_size; }

uint32_t ac_lz4_block_header_size(ac_lz4_t *r) { return r->block_header_size; }

uint32_t ac_lz4_compressed_size(ac_lz4_t *r) {
  return r->compressed_size + r->block_header_size;
}

static void write_little_endian_32(char *dest, uint32_t v) {
  (*(uint32_t *)(dest)) = v;
}

static uint32_t read_little_endian_32(char *dest) {
  return (*(uint32_t *)(dest));
}

uint32_t ac_lz4_compress(ac_lz4_t *l, const void *src, uint32_t src_len,
                         void *dest, uint32_t dest_len) {
  int level = l->level;
  void *ctx = l->ctx;
  if (level < LZ4HC_CLEVEL_MIN) {
    /* this does a bit more than just attaching dictionary (needed?) */
    LZ4_attach_dictionary((LZ4_stream_t *)ctx, NULL);
    int const acceleration = (level < 0) ? -level + 1 : 1;

    return LZ4_compress_fast_extState_fastReset(
        (LZ4_stream_t *)ctx, (const char *)src, (char *)dest, src_len, dest_len,
        acceleration);
  } else {
    LZ4_resetStreamHC_fast((LZ4_streamHC_t *)ctx, level);
    /* this does a bit more than just attaching dictionary (needed?) */
    LZ4_attach_HC_dictionary((LZ4_streamHC_t *)ctx, NULL);
    return LZ4_compress_HC_extStateHC_fastReset(
        ctx, (const char *)src, (char *)dest, src_len, dest_len, level);
  }
}

int ac_lz4_finish(ac_lz4_t *l, void *dest) {
  char *destp = (char *)dest;
  if (l->ctx) {
    write_little_endian_32(destp, 0);
    destp += sizeof(uint32_t);
    if (l->content_checksum) {
      uint32_t crc = XXH32_digest(&(l->xxh));
      write_little_endian_32(destp, crc);
      return 8;
    }
    return 4;
  } else {
    if (l->content_checksum) {
      uint32_t crc = XXH32_digest(&(l->xxh));
      uint32_t content_crc = read_little_endian_32(destp);
      if (crc != content_crc)
        return -500;
    }
    return 0;
  }
}

/*
   make sure that the block checksum matches if desired.
   If this is being used for seeking, you should assume that
   the last block can be less than dest_len decompressed.
*/
bool ac_lz4_skip(ac_lz4_t *l, const void *src, uint32_t src_len, void *dest,
                 uint32_t dest_len, bool compressed) {
  if (l->block_checksum) {
    char *srcp = (char *)src;
    uint32_t checksum = read_little_endian_32(srcp + src_len - 4);
    uint32_t crc32 = XXH32(src, src_len - 4, 0);
    if (crc32 != checksum)
      return false;
    src_len -= 4;
  }
  if (l->content_checksum) {
    int r =
        LZ4_decompress_safe((const char *)src, (char *)dest, src_len, dest_len);
    if (r < 0)
      return false;
    if (l->content_checksum)
      (void)XXH32_update(&l->xxh, dest, r);
  }
  return true;
}

int ac_lz4_decompress(ac_lz4_t *l, const void *src, uint32_t src_len,
                      void *dest, uint32_t dest_len, bool compressed) {
  if (l->block_checksum) {
    char *srcp = (char *)src;
    uint32_t checksum = read_little_endian_32(srcp + src_len - 4);
    uint32_t crc32 = XXH32(src, src_len - 4, 0);
    if (crc32 != checksum)
      return -500;
    src_len -= 4;
  }

  int r = src_len;
  if (compressed)
    r = LZ4_decompress_safe((const char *)src, (char *)dest, src_len, dest_len);
  else
    memcpy(dest, src, src_len);
  if (r < 0)
    return r;
  if (l->content_checksum)
    (void)XXH32_update(&l->xxh, dest, r);
  return r;
}

uint32_t ac_lz4_compress_block(ac_lz4_t *l, const void *src, uint32_t src_len,
                               void *dest, uint32_t dest_len) {
  if (l->content_checksum)
    (void)XXH32_update(&l->xxh, src, src_len);

  char *destp = (char *)dest;
  uint32_t compressed_size =
      ac_lz4_compress(l, src, src_len, destp + sizeof(uint32_t),
                      dest_len - l->block_header_size);
  if (compressed_size >= src_len) {
    compressed_size = src_len;
    write_little_endian_32(destp, src_len | 0x80000000U);
    memcpy(destp + sizeof(uint32_t), src, src_len);
  } else
    write_little_endian_32(destp, compressed_size);
  destp += sizeof(uint32_t);

  if (l->block_checksum) {
    uint32_t crc32 = XXH32(destp, compressed_size, 0);
    write_little_endian_32(destp + compressed_size, crc32);
  }
  return compressed_size + l->block_header_size;
}

bool ac_lz4_check_header(ac_lz4_header_t *r, void *header,
                         uint32_t header_size) {
  if (header_size != 7 || !r)
    return false;
  uint8_t *h = (uint8_t *)header;
  if (h[0] != _64kb[0] || h[1] != _64kb[1] || h[2] != _64kb[2] ||
      h[3] != _64kb[3])
    return false;
  ac_lz4_block_size_t size;
  bool block_checksum;
  bool content_checksum;
  uint32_t block_size;
  uint8_t *headerp;

  if (h[5] == 0x40) {
    size = s64kb;
    block_size = 64 * 1024;
    if (h[4] == _64kb[4] && h[6] == _64kb[6]) {
      headerp = &_64kb[0];
      block_checksum = false;
      content_checksum = false;
    } else if (h[4] == b_64kb[4] && h[6] == b_64kb[6]) {
      headerp = &b_64kb[0];
      block_checksum = true;
      content_checksum = false;
    } else if (h[4] == c_64kb[4] && h[6] == c_64kb[6]) {
      headerp = &c_64kb[0];
      block_checksum = false;
      content_checksum = true;
    } else if (h[4] == cb_64kb[4] && h[6] == cb_64kb[6]) {
      headerp = &cb_64kb[0];
      block_checksum = true;
      content_checksum = true;
    } else
      return false;
  } else if (h[5] == 0x50) {
    size = s256kb;
    block_size = 256 * 1024;
    if (h[4] == _256kb[4] && h[6] == _256kb[6]) {
      headerp = &_256kb[0];
      block_checksum = false;
      content_checksum = false;
    } else if (h[4] == b_256kb[4] && h[6] == b_256kb[6]) {
      headerp = &b_256kb[0];
      block_checksum = true;
      content_checksum = false;
    } else if (h[4] == c_256kb[4] && h[6] == c_256kb[6]) {
      headerp = &c_256kb[0];
      block_checksum = false;
      content_checksum = true;
    } else if (h[4] == cb_256kb[4] && h[6] == cb_256kb[6]) {
      headerp = &cb_256kb[0];
      block_checksum = true;
      content_checksum = true;
    } else
      return false;
  } else if (h[5] == 0x60) {
    size = s1mb;
    block_size = 1024 * 1024;
    if (h[4] == _1mb[4] && h[6] == _1mb[6]) {
      headerp = &_1mb[0];
      block_checksum = false;
      content_checksum = false;
    } else if (h[4] == b_1mb[4] && h[6] == b_1mb[6]) {
      headerp = &b_1mb[0];
      block_checksum = true;
      content_checksum = false;
    } else if (h[4] == c_1mb[4] && h[6] == c_1mb[6]) {
      headerp = &c_1mb[0];
      block_checksum = false;
      content_checksum = true;
    } else if (h[4] == cb_1mb[4] && h[6] == cb_1mb[6]) {
      headerp = &cb_1mb[0];
      block_checksum = true;
      content_checksum = true;
    } else
      return false;
  } else if (h[5] == 0x70) {
    size = s4mb;
    block_size = 4 * 1024 * 1024;
    if (h[4] == _4mb[4] && h[6] == _4mb[6]) {
      headerp = &_4mb[0];
      block_checksum = false;
      content_checksum = false;
    } else if (h[4] == b_4mb[4] && h[6] == b_4mb[6]) {
      headerp = &b_4mb[0];
      block_checksum = true;
      content_checksum = false;
    } else if (h[4] == c_4mb[4] && h[6] == c_4mb[6]) {
      headerp = &c_4mb[0];
      block_checksum = false;
      content_checksum = true;
    } else if (h[4] == cb_4mb[4] && h[6] == cb_4mb[6]) {
      headerp = &cb_4mb[0];
      block_checksum = true;
      content_checksum = true;
    } else
      return false;
  } else
    return false;
  uint32_t compressed_size = LZ4_compressBound(block_size);
  r->compressed_size = compressed_size;
  r->block_size = block_size;
  r->block_checksum = block_checksum;
  r->content_checksum = content_checksum;
  r->header = (char *)headerp;
  return true;
}

#ifdef _AC_DEBUG_MEMORY_
ac_lz4_t *_ac_lz4_init_decompress(void *header, uint32_t header_size,
                                  const char *caller) {
#else
ac_lz4_t *_ac_lz4_init_decompress(void *header, uint32_t header_size) {
#endif
  ac_lz4_header_t h;
  if (!ac_lz4_check_header(&h, header, header_size))
    return NULL;

#ifdef _AC_DEBUG_MEMORY_
  ac_lz4_t *r = (ac_lz4_t *)_ac_malloc_d(NULL, caller, sizeof(ac_lz4_t), false);
#else
  ac_lz4_t *r = (ac_lz4_t *)ac_malloc(sizeof(ac_lz4_t));
#endif
  r->ctx = NULL;
  r->level = 1;
  r->block_size = h.block_size;
  r->compressed_size = h.compressed_size;
  r->size = h.size;
  r->content_checksum = h.content_checksum;
  r->block_checksum = h.block_checksum;
  r->block_header_size = h.block_checksum ? 4 : 0;
  r->header = (uint8_t *)h.header;
  r->header_size = 7;
  if (h.content_checksum)
    XXH32_reset(&(r->xxh), 0);
  return r;
}

#ifdef _AC_DEBUG_MEMORY_
ac_lz4_t *_ac_lz4_init(int level, ac_lz4_block_size_t size, bool block_checksum,
                       bool content_checksum, const char *caller) {
#else
ac_lz4_t *_ac_lz4_init(int level, ac_lz4_block_size_t size, bool block_checksum,
                       bool content_checksum) {
#endif
  uint32_t ctx_size =
      level < LZ4HC_CLEVEL_MIN ? sizeof(LZ4_stream_t) : sizeof(LZ4_streamHC_t);
  uint8_t *header;
  uint32_t block_size;
  if (size == s64kb) {
    if (!content_checksum)
      header = block_checksum ? &b_64kb[0] : &_64kb[0];
    else
      header = block_checksum ? &cb_64kb[0] : &c_64kb[0];
    block_size = 64 * 1024;
  } else if (size == s256kb) {
    if (!content_checksum)
      header = block_checksum ? &b_256kb[0] : &_256kb[0];
    else
      header = block_checksum ? &cb_256kb[0] : &c_256kb[0];
    block_size = 256 * 1024;
  } else if (size == s1mb) {
    if (!content_checksum)
      header = block_checksum ? &b_1mb[0] : &_1mb[0];
    else
      header = block_checksum ? &cb_1mb[0] : &c_1mb[0];
    block_size = 1024 * 1024;
  } else if (size == s4mb) {
    if (!content_checksum)
      header = block_checksum ? &b_4mb[0] : &_4mb[0];
    else
      header = block_checksum ? &cb_4mb[0] : &c_4mb[0];
    block_size = 4 * 1024 * 1024;
  } else
    return NULL;

  uint32_t compressed_size = LZ4_compressBound(block_size);

#ifdef _AC_DEBUG_MEMORY_
  ac_lz4_t *r = (ac_lz4_t *)_ac_malloc_d(NULL, caller,
                                         sizeof(ac_lz4_t) + ctx_size, false);
#else
  ac_lz4_t *r = (ac_lz4_t *)ac_malloc(sizeof(ac_lz4_t) + ctx_size);
#endif
  r->ctx = (void *)(r + 1);
  r->level = level;
  r->block_size = block_size;
  r->compressed_size = compressed_size;
  r->size = size;
  r->content_checksum = content_checksum;
  r->block_checksum = block_checksum;
  r->block_header_size = 4 + (block_checksum ? 4 : 0);
  r->header = header;
  r->header_size = 7;
  if (level < LZ4HC_CLEVEL_MIN) {
    LZ4_initStream((LZ4_stream_t *)r->ctx, sizeof(LZ4_stream_t));
  } else {
    LZ4_initStreamHC((LZ4_streamHC_t *)r->ctx, sizeof(LZ4_streamHC_t));
    LZ4_setCompressionLevel((LZ4_streamHC_t *)r->ctx, level);
  }
  return r;
}

void ac_lz4_destroy(ac_lz4_t *r) { ac_free(r); }

size_t ac_lz4_compress_appending_to_buffer(ac_buffer_t *dest, void *src, int src_size) {
  int max_dst_size = LZ4_compressBound(src_size);
  size_t olen = ac_buffer_length(dest);
  void *dst = (void *)ac_buffer_append_ualloc(dest, max_dst_size);
  int compressed_data_size = LZ4_compress_default((const char *)src, (char *)dst, src_size, max_dst_size);
  if (compressed_data_size <= 0) {
    ac_buffer_resize(dest, olen);
    return 0;
  }
  ac_buffer_resize(dest, olen + compressed_data_size);
  return compressed_data_size;
}

bool ac_lz4_decompress_into_fixed_buffer(void *dest, int dest_size, void *src, int src_size) {
  int decompressed_size = LZ4_decompress_safe((const char *)src, (char *)dest, src_size, dest_size);
  if (decompressed_size != dest_size)
    return false;
  return true;
}
