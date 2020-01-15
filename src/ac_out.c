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

#include "ac_out.h"

#include "ac_allocator.h"
#include "ac_lz4.h"
#include "lz4/lz4.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>

/* options for fixed output -- TODO */
void ac_out_ext_options_fixed_compare(ac_out_ext_options_t *h,
                                      ac_io_fixed_compare_f compare, void *arg);
void ac_out_ext_options_fixed_sort(ac_out_ext_options_t *h,
                                   ac_io_fixed_sort_f sort, void *arg);
void ac_out_ext_options_fixed_reducer(ac_out_ext_options_t *h,
                                      ac_io_fixed_reducer_f reducer, void *arg);

typedef bool (*ac_out_write_f)(ac_out_t *h, const void *d, size_t len);

const int AC_OUT_NORMAL_TYPE = 0;
const int AC_OUT_PARTITIONED_TYPE = 1;
const int AC_OUT_SORTED_TYPE = 2;

struct ac_out_s {
  int type;
  ac_out_options_t options;
  ac_out_write_f write_record;

  int fd;
  bool fd_owner;
  char *filename;
  char *buffer;
  size_t buffer_pos;
  size_t buffer_size;

  // for lz4
  char *buffer2;
  size_t buffer_pos2;
  size_t buffer_size2;

  ac_out_write_f write_d;
  gzFile gz;

  ac_lz4_t *lz4;

  unsigned char delimiter;
  uint32_t fixed;
};

static bool _write_to_gz(gzFile *fd, const char *p, size_t len) {
  ssize_t n;
  const char *ep = p + len;
  while (p < ep) {
    if (ep - p > 0x7FFFFFFFU)
      n = gzwrite(*fd, p, 0x7FFFFFFFU);
    else
      n = gzwrite(*fd, p, ep - p);
    if (n > 0)
      p += n;
    else {
      int gzerrno;
      gzerror(*fd, &gzerrno);
      if (gzerrno == Z_ERRNO && errno == ENOSPC) {
        time_t cur_time = time(NULL);
        fprintf(stderr, "%s ERROR DISK FULL %s\n", __AC_FILE_LINE__,
                ctime(&cur_time));
      }
      gzclose(*fd);
      *fd = NULL;
      return false;
    }
  }
  return true;
}

static bool _write_to_fd(int *fd, const char *p, size_t len) {
  ssize_t n;
  const char *ep = p + len;
  while (p < ep) {
    if (ep - p > 0x7FFFFFFFU)
      n = write(*fd, p, 0x7FFFFFFFU);
    else
      n = write(*fd, p, ep - p);
    if (n > 0)
      p += n;
    else {
      if (n == -1 && errno == ENOSPC) {
        time_t cur_time = time(NULL);
        fprintf(stderr, "%s ERROR DISK FULL %s\n", __AC_FILE_LINE__,
                ctime(&cur_time));
      }
      return false;
    }
  }
  return true;
}

static bool _write_to_lz4(ac_out_t *h, const char *p, size_t len) {
start:;
  bool written = true;
  if (len) {
    char *wp = h->buffer2 + h->buffer_pos2;
    char *ep = h->buffer2 + h->buffer_size2;
    char *mp = wp + LZ4_compressBound(len) + 8;
    written = false;
    if (mp <= ep) {
      uint32_t n = ac_lz4_compress_block(h->lz4, p, len, wp, mp - wp);
      wp += n;
      h->buffer_pos2 += n;
      if (wp < ep)
        return true;
      written = true;
    }
  }
  if (!_write_to_fd(&(h->fd), h->buffer2, h->buffer_pos2)) {
    if (h->fd_owner)
      close(h->fd);
    h->fd = -1;
    return false;
  }

  h->buffer_pos2 = 0;
  if (!written)
    goto start;
  return true;
}

static bool _ac_out_write(ac_out_t *h, const void *d, size_t len) {
  if (h->buffer_pos + len < h->buffer_size) {
    if (len) {
      memcpy(h->buffer + h->buffer_pos, d, len);
      h->buffer_pos += len;
    }
    if (len)
      return true;
    else {
      if (!_write_to_fd(&(h->fd), h->buffer, h->buffer_pos)) {
        if (h->fd_owner)
          close(h->fd);
        h->fd = -1;
        return false;
      }
      h->buffer_pos = 0;
      return true;
    }
  }
  size_t diff = h->buffer_size - h->buffer_pos;
  memcpy(h->buffer + h->buffer_pos, d, diff);
  h->buffer_pos += diff;
  if (!_write_to_fd(&(h->fd), h->buffer, h->buffer_pos)) {
    if (h->fd_owner)
      close(h->fd);
    h->fd = -1;
    return false;
  }
  char *p = (char *)d;
  p += diff;
  len -= diff;
  h->buffer_pos = 0;
  if (len >= h->buffer_size) {
    if (!_write_to_fd(&(h->fd), p, len)) {
      if (h->fd_owner)
        close(h->fd);
      h->fd = -1;

      return false;
    }
  } else {
    memcpy(h->buffer, p, len);
    h->buffer_pos = len;
  }
  return true;
}

static bool _ac_out_write_lz4(ac_out_t *h, const void *d, size_t len) {
  // printf("writing %lu\n", len);
  if (h->buffer_pos + len <= h->buffer_size) {
    if (len)
      memcpy(h->buffer + h->buffer_pos, d, len);
    h->buffer_pos += len;
    if (len)
      return true;
    else {
      if (!_write_to_lz4(h, h->buffer, h->buffer_pos))
        return false;
      h->buffer_pos = 0;
      char *wp = h->buffer2 + h->buffer_pos2;
      uint32_t n = ac_lz4_finish(h->lz4, wp);
      h->buffer_pos2 += n;
      if (!_write_to_lz4(h, NULL, 0))
        return false;
      return true;
    }
  }
  size_t diff = h->buffer_size - h->buffer_pos;
  memcpy(h->buffer + h->buffer_pos, d, diff);
  h->buffer_pos += diff;
  if (!_write_to_lz4(h, h->buffer, h->buffer_pos))
    return false;
  char *p = (char *)d;
  p += diff;
  len -= diff;
  h->buffer_pos = 0;
  while (len >= h->buffer_size) {
    if (!_write_to_lz4(h, p, h->buffer_size))
      return false;
    len -= h->buffer_size;
    p += h->buffer_size;
  }
  if (len) {
    memcpy(h->buffer, p, len);
    h->buffer_pos = len;
  }
  return true;
}

static bool _ac_out_write_gz(ac_out_t *h, const void *d, size_t len) {
  if (h->buffer_pos + len < h->buffer_size) {
    memcpy(h->buffer + h->buffer_pos, d, len);
    h->buffer_pos += len;
    if (len)
      return true;
    else {
      if (!_write_to_gz(&(h->gz), h->buffer, h->buffer_pos))
        return false;
      h->buffer_pos = 0;
      return true;
    }
  }
  size_t diff = h->buffer_size - h->buffer_pos;
  memcpy(h->buffer + h->buffer_pos, d, diff);
  h->buffer_pos += diff;
  if (!_write_to_gz(&(h->gz), h->buffer, h->buffer_pos))
    return false;
  char *p = (char *)d;
  p += diff;
  len -= diff;
  h->buffer_pos = 0;
  while (len >= h->buffer_size) {
    if (!_write_to_gz(&(h->gz), p, h->buffer_size))
      return false;
    len -= h->buffer_size;
    p += h->buffer_size;
  }
  if (len) {
    memcpy(h->buffer, p, len);
    h->buffer_pos = len;
  }
  return true;
}

static ac_out_t *_ac_out_init_lz4(const char *filename, int fd, bool fd_owner,
                                  ac_out_options_t *options) {
  bool append_mode = options->append_mode;
  if (append_mode) {
    abort(); // for now

    int fd = open(filename, O_RDONLY);
    char header[7];
    int n = read(fd, header, 7);
    close(fd);
    if (n == 0)
      append_mode = false;
    else if (n != 7)
      return NULL;
    ac_lz4_header_t h;
    if (!ac_lz4_check_header(&h, header, 7))
      return NULL;
    if (h.content_checksum) /* cannot append if content_checksum exists */
      return NULL;
    /* seek to last valid block - for now, disallow this?  maybe create
       a routine which will trim file based upon last valid record.  It
       should check blocksum if they exist within lz4 */
    return NULL; // for now
  }

  size_t buffer_size = options->buffer_size;
  ac_lz4_t *lz4 =
      ac_lz4_init(options->level, options->size, options->block_checksum,
                  options->content_checksum);
  uint32_t compressed_size = ac_lz4_compressed_size(lz4);
  uint32_t block_size = ac_lz4_block_size(lz4);

  if (buffer_size < compressed_size + block_size + 8)
    buffer_size = compressed_size + block_size + 8;

  int filename_length = filename ? strlen(filename) + 1 : 0;

  int extra = options->safe_mode ? (filename_length * 2) + 20 : 0;
  extra += options->write_ack_file ? 5 : 0;

  ac_out_t *h = (ac_out_t *)ac_malloc(sizeof(ac_out_t) + buffer_size + 8 +
                                      filename_length + extra);
  memset(h, 0, sizeof(*h));
  h->fd = fd;
  h->lz4 = lz4;
  h->buffer = (char *)(h + 1);
  h->buffer2 = h->buffer + block_size;
  h->filename = filename_length ? h->buffer + buffer_size + 8 : NULL;
  if (h->filename) {
    strcpy(h->filename, filename);
    if (!ac_io_make_path_valid(h->filename)) {
      ac_free(h);
      return NULL;
    }
  }
  h->buffer_size = block_size;
  h->buffer_size2 = buffer_size - block_size;
  char *tmp = h->filename;
  if (options->safe_mode) {
    tmp = tmp + strlen(h->filename) + 1;
    strcpy(tmp, h->filename);
    tmp[strlen(tmp) - 4] = 0;
    strcat(tmp, "-safe.lz4");
  }

  if (h->fd == -1)
    h->fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0777);
  uint32_t header_size = 0;
  const char *header = ac_lz4_get_header(lz4, &header_size);

  memcpy(h->buffer2, header, header_size);
  h->buffer_pos2 = header_size;
  h->options = *options;
  h->write_d = _ac_out_write_lz4;
  return h;
}

static ac_out_t *_ac_out_init_gz(const char *filename, int fd, bool fd_owner,
                                 ac_out_options_t *options) {
  size_t buffer_size = options->buffer_size;
  bool append_mode = options->append_mode;

  if (buffer_size < (64 * 1024))
    buffer_size = 64 * 1024;

  int filename_length = filename ? strlen(filename) + 1 : 0;

  int extra = options->safe_mode ? (filename_length * 2) + 20 : 0;
  extra += options->write_ack_file ? 5 : 0;

  ac_out_t *h = (ac_out_t *)ac_malloc(sizeof(ac_out_t) + buffer_size +
                                      filename_length + extra);
  memset(h, 0, sizeof(*h));
  h->fd = -1;
  h->buffer = (char *)(h + 1);

  h->filename = filename_length ? h->buffer + buffer_size : NULL;
  if (h->filename) {
    strcpy(h->filename, filename);
    if (!ac_io_make_path_valid(h->filename)) {
      ac_free(h);
      return NULL;
    }
  }
  h->buffer_size = buffer_size;
  h->options = *options;
  char *tmp = h->filename;
  if (options->safe_mode) {
    tmp = tmp + strlen(h->filename) + 1;
    strcpy(tmp, h->filename);
    tmp[strlen(tmp) - 3] = 0;
    strcat(tmp, "-safe.gz");
  }

  char mode[3];
  mode[0] = append_mode ? 'a' : 'w';
  mode[1] = options->level + '0';
  mode[2] = 0;

  if (fd != -1)
    h->gz = gzdopen(fd, mode);
  else
    h->gz = gzopen(tmp, mode);
  h->write_d = _ac_out_write_gz;
  return h;
}

static ac_out_t *_ac_out_init(const char *filename, int fd, bool fd_owner,
                              ac_out_options_t *options) {
  size_t buffer_size = options->buffer_size;
  bool append_mode = options->append_mode;

  int filename_length = filename ? strlen(filename) + 1 : 0;

  int extra = options->safe_mode ? (filename_length * 2) + 20 : 0;
  extra += options->write_ack_file ? 5 : 0;
  ac_out_t *h = (ac_out_t *)ac_malloc(sizeof(ac_out_t) + buffer_size +
                                      filename_length + extra);
  memset(h, 0, sizeof(*h));
  h->buffer = (char *)(h + 1);
  h->filename = filename_length ? h->buffer + buffer_size : NULL;
  if (h->filename) {
    strcpy(h->filename, filename);
    if (!ac_io_make_path_valid(h->filename)) {
      ac_free(h);
      return NULL;
    }
  }
  h->options = *options;
  h->buffer_size = buffer_size;
  char *tmp = h->filename;
  if (options->safe_mode) {
    tmp = tmp + strlen(h->filename) + 1;
    strcpy(tmp, h->filename);
    strcat(tmp, "-safe");
  }

  if (fd != -1)
    h->fd = fd;
  else if (append_mode)
    h->fd = open(tmp, O_WRONLY | O_CREAT | O_APPEND, 0777);
  else {
    h->fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (h->fd == -1) {
      perror("Unable to open file\n");
    }
  }
  h->write_d = _ac_out_write;
  return h;
}

void ac_out_options_init(ac_out_options_t *h) {
  memset(h, 0, sizeof(*h));

  h->buffer_size = 64 * 1024;
  h->append_mode = false;
  h->safe_mode = false;
  h->write_ack_file = false;
  h->level = 1;
  h->size = s64kb;
  h->block_checksum = false;
  h->content_checksum = false;
  h->abort_on_error = false;
  h->format = 0;
  h->lz4 = false;
  h->gz = false;
}

void ac_out_options_buffer_size(ac_out_options_t *h, size_t buffer_size) {
  h->buffer_size = buffer_size;
}

void ac_out_options_format(ac_out_options_t *h, ac_io_format_t format) {
  h->format = format;
}

void ac_out_options_abort_on_error(ac_out_options_t *h) {
  h->abort_on_error = true;
}

void ac_out_options_append_mode(ac_out_options_t *h) { h->append_mode = true; }

void ac_out_options_safe_mode(ac_out_options_t *h) { h->safe_mode = true; }

void ac_out_options_write_ack_file(ac_out_options_t *h) {
  h->write_ack_file = true;
}

void ac_out_options_gz(ac_out_options_t *h, int level) {
  h->gz = true;
  h->level = level;
}

void ac_out_options_lz4(ac_out_options_t *h, int level,
                        ac_lz4_block_size_t size, bool block_checksum,
                        bool content_checksum) {
  h->lz4 = true;
  h->level = level;
  h->size = size;
  h->block_checksum = block_checksum;
  h->content_checksum = content_checksum;
}

void ac_out_ext_options_init(ac_out_ext_options_t *h) {
  memset(h, 0, sizeof(*h));
  // h->lz4_tmp = false;
  h->lz4_tmp = true;
}

void ac_out_ext_options_sort_while_partitioning(ac_out_ext_options_t *h) {
  h->sort_while_partitioning = true;
}

void ac_out_ext_options_num_sort_threads(ac_out_ext_options_t *h,
                                         size_t num_sort_threads) {
  h->num_sort_threads = num_sort_threads;
}

void ac_out_ext_options_sort_before_partitioning(ac_out_ext_options_t *h) {
  h->sort_before_partitioning = true;
}

void ac_out_ext_options_use_extra_thread(ac_out_ext_options_t *h) {
  h->use_extra_thread = true;
}

void ac_out_ext_options_dont_compress_tmp(ac_out_ext_options_t *h) {
  h->lz4_tmp = false;
}

/* options for creating a partitioned output */
void ac_out_ext_options_partition(ac_out_ext_options_t *h,
                                  ac_io_partition_f part, void *arg) {
  h->partition = part;
  h->partition_arg = arg;
}

void ac_out_ext_options_num_partitions(ac_out_ext_options_t *h,
                                       size_t num_partitions) {
  h->num_partitions = num_partitions;
}

/* options for sorting the output */
void ac_out_ext_options_compare(ac_out_ext_options_t *h,
                                ac_io_compare_f compare, void *arg) {
  h->compare = compare;
  h->compare_arg = arg;
  if (!h->int_compare) {
    h->int_compare = compare;
    h->int_compare_arg = arg;
  }
}

void ac_out_ext_options_intermediate_group_size(ac_out_ext_options_t *h,
                                                size_t num_per_group) {
  h->num_per_group = num_per_group;
}

void ac_out_ext_options_intermediate_compare(ac_out_ext_options_t *h,
                                             ac_io_compare_f compare,
                                             void *arg) {
  h->int_compare = compare;
  h->int_compare_arg = arg;
}

/* set the reducer */
void ac_out_ext_options_reducer(ac_out_ext_options_t *h,
                                ac_io_reducer_f reducer, void *arg) {
  h->reducer = reducer;
  h->reducer_arg = arg;
  if (!h->int_reducer) {
    h->int_reducer = reducer;
    h->int_reducer_arg = arg;
  }
}

void ac_out_ext_options_intermediate_reducer(ac_out_ext_options_t *h,
                                             ac_io_reducer_f reducer,
                                             void *arg) {
  h->int_reducer = reducer;
  h->int_reducer_arg = arg;
}

/* options for fixed output */
void ac_out_ext_options_fixed_reducer(ac_out_ext_options_t *h,
                                      ac_io_fixed_reducer_f reducer,
                                      void *arg) {
  h->fixed_reducer = reducer;
  h->fixed_reducer_arg = arg;
}

void ac_out_ext_options_fixed_compare(ac_out_ext_options_t *h,
                                      ac_io_fixed_compare_f compare,
                                      void *arg) {
  h->fixed_compare = compare;
  h->fixed_compare_arg = arg;
}

void ac_out_ext_options_fixed_sort(ac_out_ext_options_t *h,
                                   ac_io_fixed_sort_f sort, void *arg) {
  h->fixed_sort = sort;
  h->fixed_sort_arg = arg;
}

bool _ac_out_write_prefix(ac_out_t *h, const void *d, size_t len) {
  uint32_t length = len;
  if (!ac_out_write(h, &length, sizeof(length)) || !ac_out_write(h, d, length))
    return false;
  return true;
}

bool ac_out_write_prefix(ac_out_t *h, const void *d, size_t len) {
  if (h->type)
    return false;
  return _ac_out_write_prefix(h, d, len);
}

static bool _ac_out_write_delimiter(ac_out_t *h, const void *d, size_t len) {
  if (!ac_out_write(h, d, len) ||
      !ac_out_write(h, &h->delimiter, sizeof(h->delimiter)))
    return false;
  return true;
}

static bool _ac_out_write_fixed(ac_out_t *h, const void *d, size_t len) {
  if (len != h->fixed)
    abort();
  return ac_out_write(h, d, len);
}

bool ac_out_write_delimiter(ac_out_t *h, const void *d, size_t len,
                            char delim) {
  if (h->type)
    return false;
  if (!ac_out_write(h, d, len) || !ac_out_write(h, &delim, sizeof(delim)))
    return false;
  return true;
}

ac_out_t *_ac_out_init_(const char *filename, int fd, bool fd_owner,
                        ac_out_options_t *options) {
  ac_out_options_t opts;
  if (!options) {
    options = &opts;
    ac_out_options_init(options);
  }

  if (!filename && fd == -1)
    abort();
  if (fd != -1 && options->append_mode)
    abort();
  if (options->safe_mode && options->append_mode) /* not a valid combination */
    abort();
  if (fd != -1 && (options->safe_mode || options->write_ack_file))
    abort();

  ac_out_t *h;
  if ((!filename && options->lz4) || ac_io_extension(filename, ".lz4"))
    h = _ac_out_init_lz4(filename, fd, fd_owner, options);
  else if ((!filename && options->gz) || ac_io_extension(filename, ".gz"))
    h = _ac_out_init_gz(filename, fd, fd_owner, options);
  else
    h = _ac_out_init(filename, fd, fd_owner, options);

  if (h) {
    if (options->format < 0) {
      int delim = (-options->format) - 1;
      h->delimiter = delim;
      h->write_record = _ac_out_write_delimiter;
    } else if (options->format > 0) {
      h->fixed = options->format;
      h->write_record = _ac_out_write_fixed;
    } else
      h->write_record = _ac_out_write_prefix;
  } else if (options->abort_on_error)
    abort();
  return h;
}

ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options) {
  return _ac_out_init_(filename, -1, true, options);
}

ac_out_t *ac_out_init_with_fd(int fd, bool fd_owner,
                              ac_out_options_t *options) {
  return _ac_out_init_(NULL, fd, fd_owner, options);
}

bool ac_out_write_record(ac_out_t *h, const void *d, size_t len) {
  return h->write_record(h, d, len);
}

bool ac_out_write(ac_out_t *h, const void *d, size_t len) {
  if (h->type)
    return false;
  if (!len)
    return true;
  if (h->write_d) {
    if (!h->write_d(h, d, len)) {
      h->write_d = NULL;
      if (h->options.abort_on_error)
        abort();
      return false;
    }
    return true;
  }
  if (h->options.abort_on_error)
    abort();
  return false;
}

static bool ac_out_flush(ac_out_t *h) {
  if (h->write_d) {
    if (!h->write_d(h, NULL, 0)) {
      h->write_d = NULL;
      if (h->options.abort_on_error)
        abort();
      return false;
    }
    return true;
  }
  if (h->options.abort_on_error)
    abort();
  return false;
}

static void ac_out_ext_destroy(ac_out_t *hp);

void ac_out_destroy(ac_out_t *h) {
  if (h->type != AC_OUT_NORMAL_TYPE) {
    ac_out_ext_destroy(h);
    return;
  }

  ac_out_flush(h);
  if (h->fd > -1 && h->fd_owner)
    close(h->fd);

  if (h->lz4)
    ac_lz4_destroy(h->lz4);
  if (h->gz)
    gzclose(h->gz);

  if (h->options.safe_mode)
    rename(h->filename + strlen(h->filename) + 1, h->filename);

  if (h->options.write_ack_file) {
    strcat(h->filename, ".ack");
    FILE *out = fopen(h->filename, "wb");
    fclose(out);
  }

  ac_free(h);
}

/** ac_out_ext functionality **/
static void suffix_filename_with_id(char *dest, const char *filename, size_t id,
                                    const char *extra, bool use_lz4) {
  strcpy(dest, filename);
  if (ac_io_extension(filename, ".lz4"))
    sprintf(dest + strlen(dest) - 4, "%s%s_%lu.lz4", extra ? "_" : "",
            extra ? extra : "", id);
  else if (ac_io_extension(filename, ".gz")) {
    if (use_lz4)
      sprintf(dest + strlen(dest) - 3, "%s%s_%lu.lz4", extra ? "_" : "",
              extra ? extra : "", id);
    else
      sprintf(dest + strlen(dest) - 3, "%s%s_%lu.gz", extra ? "_" : "",
              extra ? extra : "", id);
  } else {
    if (use_lz4)
      sprintf(dest + strlen(dest), "%s%s_%lu.lz4", extra ? "_" : "",
              extra ? extra : "", id);
    else
      sprintf(dest + strlen(dest), "%s%s_%lu", extra ? "_" : "",
              extra ? extra : "", id);
  }
}

/** ac_out_partitioned_t **/
typedef struct {
  int type;
  ac_out_options_t options;
  ac_out_write_f write_record;

  char *filename;

  ac_out_ext_options_t ext_options;

  ac_out_options_t part_options;
  ac_out_ext_options_t ext_part_options;

  ac_in_options_t in_options;

  ac_out_t **partitions;
  size_t num_partitions;
  ac_io_partition_f partition;
  void *partition_arg;

  size_t *tasks;
  size_t *taskp;
  size_t *taskep;
  pthread_mutex_t mutex;
} ac_out_partitioned_t;

bool write_partitioned_record(ac_out_t *hp, const void *d, size_t len) {
  ac_out_partitioned_t *h = (ac_out_partitioned_t *)hp;

  ac_io_record_t r;
  r.length = len;
  r.record = (char *)d;
  r.tag = 0;

  size_t partition = h->partition(&r, h->num_partitions, h->partition_arg);
  if (partition >= h->num_partitions)
    return false;

  ac_out_t *o = h->partitions[partition];
  return o->write_record(o, d, len);
}

ac_out_t *ac_out_partitioned_init(const char *filename,
                                  ac_out_options_t *options,
                                  ac_out_ext_options_t *ext_options) {
  if (ext_options->num_partitions == 0) {
    ac_io_partition_f partition = ext_options->partition;
    ext_options->partition = NULL;
    ac_out_t *r = ac_out_ext_init(filename, options, ext_options);
    ext_options->partition = partition;
    return r;
  } else if (ext_options->num_partitions == 1) {
    if (!filename)
      abort();
    // give suffix to filename
    char *tmp_name = (char *)ac_malloc(strlen(filename) + 20);
    ac_io_partition_f partition = ext_options->partition;
    ext_options->partition = NULL;
    suffix_filename_with_id(tmp_name, filename, 0, NULL, false);
    ac_out_t *r = ac_out_ext_init(tmp_name, options, ext_options);
    ext_options->partition = partition;
    ac_free(tmp_name);
    return r;
  } else {
    if (!filename)
      abort();

    ac_out_partitioned_t *h = (ac_out_partitioned_t *)ac_malloc(
        sizeof(ac_out_partitioned_t) + strlen(filename) + 1 +
        (sizeof(ac_out_t *) * ext_options->num_partitions));
    h->options = *options;
    h->part_options = *options;
    h->ext_options = *ext_options;
    h->ext_part_options = *ext_options;
    h->partitions = (ac_out_t **)(h + 1);
    h->num_partitions = ext_options->num_partitions;
    h->filename = (char *)(h->partitions + ext_options->num_partitions);
    strcpy(h->filename, filename);
    h->partition = ext_options->partition;
    h->partition_arg = ext_options->partition_arg;

    h->part_options.buffer_size = options->buffer_size / h->num_partitions;
    h->ext_part_options.partition = NULL;

    if (!h->ext_options.sort_while_partitioning) {
      ac_out_options_format(&(h->part_options), ac_io_prefix());
      h->part_options.write_ack_file = false;
    }

    char *tmp_name = (char *)ac_malloc(strlen(filename) + 40);
    for (size_t i = 0; i < h->num_partitions; i++) {
      // printf("%s\n", tmp_name);
      if (h->ext_options.sort_while_partitioning || !h->ext_options.compare) {
        suffix_filename_with_id(tmp_name, filename, i, NULL, false);
        h->partitions[i] = ac_out_ext_init(tmp_name, &(h->part_options),
                                           &(h->ext_part_options));
      } else {
        suffix_filename_with_id(tmp_name, filename, i, "unsorted",
                                h->ext_options.lz4_tmp);
        h->partitions[i] = ac_out_init(tmp_name, &(h->part_options));
      }
    }
    h->write_record = write_partitioned_record;
    ac_free(tmp_name);
    h->type = AC_OUT_PARTITIONED_TYPE;
    return (ac_out_t *)h;
  }
}

void *sort_partitions(void *arg) {
  ac_out_partitioned_t *h = (ac_out_partitioned_t *)arg;
  char *filename = h->filename;
  char *tmp_name = (char *)ac_malloc(strlen(h->filename) + 40);

  while (true) {
    pthread_mutex_lock(&h->mutex);
    size_t *tp = h->taskp;
    h->taskp++;
    pthread_mutex_unlock(&h->mutex);
    if (tp >= h->taskep)
      break;

    suffix_filename_with_id(tmp_name, filename, *tp, "unsorted",
                            h->ext_options.lz4_tmp);
    ac_in_t *in = ac_in_init(tmp_name, &(h->in_options));
    suffix_filename_with_id(tmp_name, filename, *tp, NULL, false);
    ac_out_t *out =
        ac_out_ext_init(tmp_name, &(h->part_options), &(h->ext_part_options));
    ac_io_record_t *r;
    while ((r = ac_in_advance(in)) != NULL)
      ac_out_write_record(out, r->record, r->length);
    ac_out_destroy(out);
    ac_in_destroy(in);
  }
  ac_free(tmp_name);
  return NULL;
}

void ac_out_partitioned_destroy(ac_out_t *hp) {
  ac_out_partitioned_t *h = (ac_out_partitioned_t *)hp;
  for (size_t i = 0; i < h->num_partitions; i++) {
    ac_out_destroy(h->partitions[i]);
  }
  if (!h->ext_options.sort_while_partitioning && h->ext_options.compare) {
    /*  buffer_size memory, num_threads, input, output - prefer input
       because OS will buffer output.
      */
    size_t num_threads = h->ext_options.num_sort_threads;
    if (num_threads < 1)
      num_threads = 1;
    if (num_threads > h->num_partitions)
      num_threads = h->num_partitions;

    size_t buffer_size = h->options.buffer_size / (num_threads * 2);

    ac_out_options_buffer_size(&(h->part_options), buffer_size);
    ac_out_options_format(&(h->part_options), h->options.format);
    h->ext_part_options.use_extra_thread = false;
    ac_in_options_init(&(h->in_options));
    ac_in_options_buffer_size(&(h->in_options), buffer_size);
    ac_in_options_format(&(h->in_options), ac_io_prefix());

    h->tasks = (size_t *)ac_malloc(sizeof(size_t) * h->num_partitions);
    h->taskp = h->tasks;
    h->taskep = h->tasks + h->num_partitions;
    for (size_t i = 0; i < h->num_partitions; i++)
      h->tasks[i] = i;

    pthread_mutex_init(&h->mutex, NULL);
    pthread_t *threads =
        (pthread_t *)ac_malloc(sizeof(pthread_t) * num_threads);
    for (size_t i = 0; i < num_threads; i++)
      pthread_create(threads + i, NULL, sort_partitions, h);
    for (size_t i = 0; i < num_threads; i++)
      pthread_join(threads[i], NULL);
    pthread_mutex_destroy(&h->mutex);
    ac_free(h->tasks);
    ac_free(threads);
    char *filename = h->filename;
    char *tmp_name = (char *)ac_malloc(strlen(h->filename) + 40);
    for (size_t i = 0; i < h->num_partitions; i++) {
      suffix_filename_with_id(tmp_name, filename, i, "unsorted",
                              h->ext_options.lz4_tmp);
      remove(tmp_name);
    }
    ac_free(tmp_name);
  }
  ac_free(h);
}

typedef struct {
  char *buffer;
  char *bp;
  char *ep;
  size_t num_records;
  size_t size;
} ac_out_buffer_t;

const int EXTRA_IN = 0;
const int EXTRA_FILENAME = 1;
const int EXTRA_FILE_TO_REMOVE = EXTRA_FILENAME;
const int EXTRA_ACK_FILE = EXTRA_FILENAME | 2;

typedef struct extra_s {
  int type;
  void *p;
  struct extra_s *next;
} extra_t;

typedef struct {
  int type;
  ac_out_options_t options;
  ac_out_write_f write_record;

  ac_in_options_t file_options;

  char *filename;
  char *suffix;

  char *tmp_filename;

  ac_out_buffer_t buf1, buf2;
  ac_out_buffer_t *b, *b2;

  size_t num_written;
  size_t num_group_written;

  bool thread_started;
  pthread_t thread;
  bool out_in_called;
  extra_t *extras;

  int tag;

  ac_out_ext_options_t ext_options;
  ac_out_ext_options_t partition_options;
} ac_out_sorted_t;

bool write_sorted_record(ac_out_t *hp, const void *d, size_t len);

static void _extra_add(ac_out_t *hp, void *p, int type) {
  ac_out_sorted_t *h = (ac_out_sorted_t *)hp;
  extra_t *extra;
  if (type & EXTRA_FILENAME) {
    char *f = (char *)p;
    extra = (extra_t *)ac_malloc(sizeof(extra_t) + strlen(f) + 1);
    extra->p = (void *)(extra + 1);
    strcpy((char *)extra->p, f);
  } else {
    extra = (extra_t *)ac_malloc(sizeof(extra_t));
    extra->p = p;
  }
  extra->type = type;
  extra->next = h->extras;
  h->extras = extra;
}

void ac_out_sorted_add_in(ac_out_t *hp, ac_in_t *in) {
  _extra_add(hp, in, EXTRA_IN);
}

void ac_out_sorted_add_file_to_remove(ac_out_t *hp, const char *filename) {
  _extra_add(hp, (void *)filename, EXTRA_FILE_TO_REMOVE);
}

void ac_out_sorted_add_ack_file(ac_out_t *hp, const char *filename) {
  _extra_add(hp, (void *)filename, EXTRA_ACK_FILE);
}

static void tmp_filename(char *dest, const char *filename, uint32_t n,
                         const char *suffix) {
  sprintf(dest, "%s_%u_tmp%s", filename, n, suffix);
}

static void group_tmp_filename(char *dest, const char *filename, uint32_t n,
                               const char *suffix) {
  sprintf(dest, "%s_%u_gtmp%s", filename, n, suffix);
}

static inline void clear_buffer(ac_out_buffer_t *b) {
  b->bp = b->buffer;
  b->ep = b->bp + b->size;
  b->num_records = 0;
}

static inline void init_buffer(ac_out_buffer_t *b, size_t buffer_size) {
  b->buffer = (char *)ac_malloc(buffer_size);
  b->size = buffer_size;
  clear_buffer(b);
}

static ac_in_t *_in_from_buffer(ac_out_sorted_t *h, ac_out_buffer_t *b) {
  if (!b->num_records)
    return NULL;

  ac_io_record_t *r = (ac_io_record_t *)b->buffer;
  uint32_t num_r = b->num_records;
  ac_io_sort_records(r, num_r, h->ext_options.int_compare,
                     h->ext_options.int_compare_arg);

  clear_buffer(b);
  return ac_in_records_init(r, num_r, &(h->file_options));
}

ac_out_t *ac_out_sorted_init(const char *filename, ac_out_options_t *options,
                             ac_out_ext_options_t *ext_options) {
  ac_out_options_t opts;
  if (!options) {
    options = &opts;
    ac_out_options_init(options);
  }
  size_t buffer_size = options->buffer_size;
  ac_out_sorted_t *h = (ac_out_sorted_t *)ac_calloc(
      sizeof(ac_out_sorted_t) + (strlen(filename) * 3) + 100);
  h->filename = (char *)(h + 1);
  strcpy(h->filename, filename);
  h->type = AC_OUT_SORTED_TYPE;

  h->out_in_called = false;

  h->tmp_filename = h->filename + strlen(filename) + 1;
  if (ac_io_extension(filename, ".lz4")) {
    h->filename[strlen(filename) - 4] = 0;
    h->suffix = (char *)".lz4";
  } else if (ac_io_extension(filename, ".gz")) {
    h->suffix = (char *)".gz";
    h->filename[strlen(filename) - 3] = 0;
  }

  h->thread_started = false;

  h->ext_options = *ext_options;
  h->partition_options = *ext_options;
  h->partition_options.compare = NULL;
  h->options = *options;

  ac_in_options_init(&(h->file_options));
  if (ext_options->int_reducer)
    ac_in_options_reducer(&(h->file_options), ext_options->int_compare,
                          ext_options->int_compare_arg,
                          ext_options->int_reducer,
                          ext_options->int_reducer_arg);

  if (ext_options->use_extra_thread) {
    buffer_size /= 2;
    init_buffer(&h->buf1, buffer_size);
    init_buffer(&h->buf2, buffer_size);
    h->b = &(h->buf1);
    h->b2 = &(h->buf2);
  } else {
    init_buffer(&h->buf1, buffer_size);
    h->b = &(h->buf1);
    h->b2 = &(h->buf1);
  }
  h->write_record = write_sorted_record;
  return (ac_out_t *)h;
}

static inline void wait_on_thread(ac_out_sorted_t *h) {
  if (h->thread_started) {
    pthread_join(h->thread, NULL);
    h->thread_started = false;
  }
}

ac_out_t *get_next_tmp(ac_out_sorted_t *h, bool tmp_only) {
  const char *suffix = h->ext_options.lz4_tmp ? ".lz4" : "";
  if (!tmp_only && h->ext_options.num_per_group) {
    group_tmp_filename(h->tmp_filename, h->filename, h->num_group_written,
                       suffix);
    h->num_group_written++;
  } else {
    tmp_filename(h->tmp_filename, h->filename, h->num_written, suffix);
    h->num_written++;
  }
  // allow output buffer to be supplied to ac_out_options...
  // allow input buffer to be supplied as well
  ac_out_options_t options;
  ac_out_options_init(&options);
  ac_out_options_format(&options, ac_io_prefix());
  /* reuse the same buffer? */
  ac_out_options_buffer_size(&options, 10 * 1024 * 1024);
  return ac_out_init(h->tmp_filename, &options);
}

void check_for_merge(ac_out_sorted_t *h) {
  if (!h->ext_options.num_per_group ||
      h->num_group_written < h->ext_options.num_per_group)
    return;

  ac_out_t *out = get_next_tmp(h, true);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_prefix());
  ac_in_t *in =
      ac_in_ext_init(h->ext_options.compare, h->ext_options.compare_arg, &opts);
  if (h->ext_options.reducer)
    ac_in_ext_reducer(in, h->ext_options.reducer, h->ext_options.reducer_arg);

  const char *suffix = h->ext_options.lz4_tmp ? ".lz4" : "";
  for (size_t i = 0; i < h->num_group_written; i++) {
    group_tmp_filename(h->tmp_filename, h->filename, i, suffix);
    ac_in_ext_add(in, ac_in_init(h->tmp_filename, &opts), 0);
  }
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    ac_out_write_record(out, r->record, r->length);

  ac_out_destroy(out);
  ac_in_destroy(in);
  h->num_group_written = 0;
}

void *write_sorted_thread(void *arg) {
  ac_out_sorted_t *h = (ac_out_sorted_t *)arg;
  ac_in_t *in = _in_from_buffer(h, h->b2);
  ac_out_t *out = get_next_tmp(h, false);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    ac_out_write_record(out, r->record, r->length);
  ac_in_destroy(in);
  ac_out_destroy(out);

  if (h->ext_options.num_per_group)
    check_for_merge(h);
  return NULL;
}

void write_sorted(ac_out_sorted_t *h) {
  if (h->b->bp == h->b->buffer)
    return;
  wait_on_thread(h);
  if (h->ext_options.use_extra_thread) {
    ac_out_buffer_t *tmp = h->b;
    h->b = h->b2;
    h->b2 = tmp;

    h->thread_started = true;
    pthread_create(&h->thread, NULL, write_sorted_thread, h);
  } else
    write_sorted_thread(h);
}

void ac_out_tag(ac_out_t *hp, int tag) {
  ac_out_sorted_t *h = (ac_out_sorted_t *)hp;
  if (h->type != AC_OUT_SORTED_TYPE)
    return;

  h->tag = tag;
}

ac_in_t *ac_out_in(ac_out_t *hp) {
  ac_out_sorted_t *h = (ac_out_sorted_t *)hp;
  if (h->type != AC_OUT_SORTED_TYPE)
    return NULL;

  if (h->out_in_called)
    return NULL;

  h->out_in_called = true;

  if (!h->num_written && !h->num_group_written)
    return _in_from_buffer(h, h->b);

  if (h->b->num_records) {
    wait_on_thread(h);
    if (h->ext_options.use_extra_thread) {
      ac_out_buffer_t *tmp = h->b;
      h->b = h->b2;
      h->b2 = tmp;
    }
    if (h->ext_options.num_per_group)
      h->ext_options.num_per_group =
          h->num_group_written ? h->num_group_written : 1;
    write_sorted_thread(h);
  }

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_prefix());
  ac_in_t *in =
      ac_in_ext_init(h->ext_options.compare, h->ext_options.compare_arg, &opts);
  if (h->ext_options.reducer)
    ac_in_ext_reducer(in, h->ext_options.reducer, h->ext_options.reducer_arg);

  const char *suffix = h->ext_options.lz4_tmp ? ".lz4" : "";
  for (size_t i = 0; i < h->num_written; i++) {
    tmp_filename(h->tmp_filename, h->filename, i, suffix);
    ac_in_ext_add(in, ac_in_init(h->tmp_filename, &opts), 0);
  }
  return in;
}

/*
TODO: Add support for fixed length records.  These records don't need the
ac_io_record_t record array and can simply be sorted in place.

bool write_fixed_sorted_record(ac_out_t *hp, const void *d, size_t len) {
  ac_out_sorted_t *h = (ac_out_sorted_t *)hp;
  if (len != h->fixed)
    abort();

  char *bp = h->b->bp;
  if (bp + len > h->b->ep) {
    write_sorted(h);
    bp = h->b->bp;
  }

  memcpy(bp, d, len);
  bp += len;
  h->b->bp = bp;
  h->b->num_records++;
  return true;
}
*/

bool write_sorted_record(ac_out_t *hp, const void *d, size_t len) {
  if (len > 0xffffffffU)
    return false;
  ac_out_sorted_t *h = (ac_out_sorted_t *)hp;

  size_t length = len + sizeof(ac_io_record_t) + 5;
  char *bp = h->b->bp;
  if (bp + length > h->b->ep) {
    write_sorted(h);
    bp = h->b->bp;
    // TODO: Support records that are larger than the buffer
    // if (bp + length > h->b->ep)
    //  return write_one_record(h, d, len);
  }

  /* Write data to the end of the buffer and the records to the beginning.
     This has the effect of keeping the records in the original order and
     makes effective use of the buffer from both ends.  Later, the records
     will be sorted.  The data is written with a zero terminator to make it
     easy for string comparison functions.
  */
  char *ep = h->b->ep;
  ep--;
  *ep = 0;
  ep -= len;
  memcpy(ep, d, len);

  ac_io_record_t *r = (ac_io_record_t *)bp;
  r->record = ep;
  r->length = len;
  r->tag = h->tag;
  bp += sizeof(*r);

  h->b->bp = bp;
  h->b->ep = ep;
  h->b->num_records++;

  return true;
}

void ac_out_ext_remove_tmp_files(char *tmp, const char *filename,
                                 bool lz4_tmp) {
  const char *suffix = lz4_tmp ? ".lz4" : "";
  uint32_t skipped = 0;
  for (uint32_t i = 0; skipped < 4; i++) {
    tmp_filename(tmp, filename, i, suffix);
    if (ac_io_file_exists(tmp))
      remove(tmp);
    else
      skipped++;
  }
  skipped = 0;
  for (uint32_t i = 0; skipped < 4; i++) {
    group_tmp_filename(tmp, filename, i, suffix);
    if (ac_io_file_exists(tmp))
      remove(tmp);
    else
      skipped++;
  }
}

static void remove_extras(ac_out_sorted_t *h) {
  extra_t *extra = h->extras;
  while (extra) {
    if (extra->type == EXTRA_FILE_TO_REMOVE)
      remove((char *)extra->p);
    extra = extra->next;
  }
}

static void touch_extras(ac_out_sorted_t *h) {
  extra_t *extra = h->extras;
  while (extra) {
    if (extra->type == EXTRA_ACK_FILE) {
      FILE *out = fopen((char *)extra->p, "wb");
      fclose(out);
    }
    extra = extra->next;
  }
}

static void destroy_extra_ins(ac_out_sorted_t *h) {
  extra_t *extra = h->extras;
  while (extra) {
    if (extra->type == EXTRA_IN) {
      ac_in_t *in = (ac_in_t *)extra->p;
      if (in)
        ac_in_destroy(in);
      extra->p = NULL;
    }
    extra = extra->next;
  }
}

void ac_out_sorted_destroy(ac_out_t *hp) {
  ac_out_sorted_t *h = (ac_out_sorted_t *)hp;
  ac_in_t *in = ac_out_in(hp);
  if (in) {
    sprintf(h->tmp_filename, "%s%s", h->filename, h->suffix ? h->suffix : "");
    ac_out_t *out = ac_out_ext_init(h->tmp_filename, &(h->options),
                                    &(h->partition_options));
    ac_io_record_t *r;
    while ((r = ac_in_advance(in)) != NULL)
      ac_out_write_record(out, r->record, r->length);
    ac_out_destroy(out);
    ac_in_destroy(in);
  }
  if (h->buf1.buffer)
    ac_free(h->buf1.buffer);
  if (h->buf2.buffer)
    ac_free(h->buf2.buffer);
  ac_out_ext_remove_tmp_files(h->tmp_filename, h->filename,
                              h->ext_options.lz4_tmp);
  destroy_extra_ins(h);
  remove_extras(h);
  touch_extras(h);

  extra_t *extra = h->extras;
  while (extra) {
    extra_t *next = extra->next;
    ac_free(extra);
    extra = next;
  }

  ac_free(h);
}

static void ac_out_ext_destroy(ac_out_t *hp) {
  if (hp->type == AC_OUT_PARTITIONED_TYPE)
    ac_out_partitioned_destroy(hp);
  else if (hp->type == AC_OUT_SORTED_TYPE)
    ac_out_sorted_destroy(hp);
  else
    abort();
}

ac_out_t *ac_out_ext_init(const char *filename, ac_out_options_t *options,
                          ac_out_ext_options_t *ext_options) {
  if (ext_options->partition && !ext_options->sort_before_partitioning)
    return ac_out_partitioned_init(filename, options, ext_options);
  else if (ext_options->compare)
    return ac_out_sorted_init(filename, options, ext_options);
  else if (ext_options->partition)
    return ac_out_partitioned_init(filename, options, ext_options);
  return ac_out_init(filename, options);
}
