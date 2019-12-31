#ifndef _ac_out_H
#define _ac_out_H

#include "ac_common.h"
#include "ac_io.h"
#include "ac_lz4.h"

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

void ac_out_init_options(ac_out_options_t *r);
void ac_out_fd(ac_out_options_t *r, int fd, bool owner);
void ac_out_buffer_size(ac_out_options_t *r, size_t buffer_size);
void ac_out_format(ac_out_options_t *r, ac_io_format_t format);
void ac_out_abort_on_error(ac_out_options_t *r);
void ac_out_append_mode(ac_out_options_t *r);
void ac_out_safe_mode(ac_out_options_t *r);
void ac_out_write_ack_file(ac_out_options_t *r);
void ac_out_gz(ac_out_options_t *r, int level);
void ac_out_lz4(ac_out_options_t *r, int level, ac_lz4_block_size_t size,
                bool block_checksum, bool content_checksum);

ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options);

bool ac_out_write(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_prefix(ac_out_t *h, const void *d, size_t len);
bool ac_out_write_delimiter(ac_out_t *h, const void *d, size_t len,
                            char delimiter);
bool ac_out_write_record(ac_out_t *h, const void *d, size_t len);

void ac_out_destroy(ac_out_t *h);

#endif
