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
#include <time.h>
#include <unistd.h>
#include <zlib.h>

typedef bool (*ac_out_write_f)(ac_out_t *h, const void *d, size_t len);

const int AC_OUT_NORMAL_TYPE = 0;
const int AC_OUT_PARTITIONED_TYPE = 1;
const int AC_OUT_SORTED_TYPE = 2;

struct ac_out_s {
  int type;
  ac_out_options_t options;
  ac_out_write_f write_record;

  int fd;
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
    if (h->options.fd_owner)
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
        if (h->options.fd_owner)
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
    if (h->options.fd_owner)
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
      if (h->options.fd_owner)
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

static ac_out_t *_ac_out_init_lz4(const char *filename,
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
  h->fd = options->fd;
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

static ac_out_t *_ac_out_init_gz(const char *filename,
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

  if (options->fd != -1)
    h->gz = gzdopen(options->fd, mode);
  else
    h->gz = gzopen(tmp, mode);
  h->write_d = _ac_out_write_gz;
  return h;
}

static ac_out_t *_ac_out_init(const char *filename, ac_out_options_t *options) {
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

  if (options->fd != -1)
    h->fd = options->fd;
  else if (append_mode)
    h->fd = open(tmp, O_WRONLY | O_CREAT | O_APPEND, 0777);
  else
    h->fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0777);
  h->write_d = _ac_out_write;
  return h;
}

void ac_out_options_init(ac_out_options_t *h) {
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
  h->fd = -1;
  h->fd_owner = true;
}

void ac_out_options_fd(ac_out_options_t *h, int fd, bool owner) {
  h->fd = fd;
  h->fd_owner = owner;
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
}

/* options for creating a partitioned output */
void ac_out_ext_options_partition(ac_out_ext_options_t *h,
                                  ac_io_partition_f part, void *tag) {
  h->partition = part;
  h->partition_tag = tag;
}

void ac_out_ext_options_num_partitions(ac_out_ext_options_t *h,
                                       size_t num_partitions) {
  h->num_partitions = num_partitions;
}

/* options for sorting the output */
void ac_out_ext_options_compare(ac_out_ext_options_t *h,
                                ac_io_compare_f compare, void *tag) {
  h->compare = compare;
  h->compare_tag = tag;
}

void ac_out_ext_options_intermediate_group_size(ac_out_ext_options_t *h,
                                                size_t num_per_group) {
  h->num_per_group = num_per_group;
}

void ac_out_ext_options_intermediate_compare(ac_out_ext_options_t *h,
                                             ac_io_compare_f compare,
                                             void *tag) {
  h->int_compare = compare;
  h->int_compare_tag = tag;
}

/* set the reducer */
void ac_out_ext_options_reducer(ac_out_ext_options_t *h,
                                ac_io_reducer_f reducer, void *tag) {
  h->reducer = reducer;
  h->reducer_tag = tag;
}

/* options for fixed output */
void ac_out_ext_options_fixed_reducer(ac_out_ext_options_t *h,
                                      ac_io_fixed_reducer_f reducer,
                                      void *tag) {
  h->fixed_reducer = reducer;
  h->fixed_reducer_tag = tag;
}

void ac_out_ext_options_fixed_compare(ac_out_ext_options_t *h,
                                      ac_io_fixed_compare_f compare,
                                      void *tag) {
  h->fixed_compare = compare;
  h->fixed_compare_tag = tag;
}

void ac_out_ext_options_fixed_sort(ac_out_ext_options_t *h,
                                   ac_io_fixed_sort_f sort, void *tag) {
  h->fixed_sort = sort;
  h->fixed_sort_tag = tag;
}

bool ac_out_write_prefix(ac_out_t *h, const void *d, size_t len) {
  uint32_t length = len;
  if (!ac_out_write(h, &length, sizeof(length)) || !ac_out_write(h, d, length))
    return false;
  return true;
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
  if (!ac_out_write(h, d, len) || !ac_out_write(h, &delim, sizeof(delim)))
    return false;
  return true;
}

ac_out_t *ac_out_init(const char *filename, ac_out_options_t *options) {
  ac_out_options_t opts;
  if (!options) {
    options = &opts;
    ac_out_options_init(options);
  }

  if (!filename && options->fd == -1)
    abort();
  if (options->fd != -1 && options->append_mode)
    abort();
  if (options->safe_mode && options->append_mode) /* not a valid combination */
    abort();
  if (options->fd != -1 && (options->safe_mode || options->write_ack_file))
    abort();

  ac_out_t *h;
  if ((!filename && options->lz4) || ac_io_extension(filename, ".lz4"))
    h = _ac_out_init_lz4(filename, options);
  else if ((!filename && options->gz) || ac_io_extension(filename, ".gz"))
    h = _ac_out_init_gz(filename, options);
  else
    h = _ac_out_init(filename, options);

  if (h) {
    if (options->format < 0) {
      int delim = (-options->format) - 1;
      h->delimiter = delim;
      h->write_record = _ac_out_write_delimiter;
    } else if (options->format > 0) {
      h->fixed = options->format;
      h->write_record = _ac_out_write_fixed;
    } else
      h->write_record = ac_out_write_prefix;
  } else if (options->abort_on_error)
    abort();
  return h;
}

bool ac_out_write_record(ac_out_t *h, const void *d, size_t len) {
  return h->write_record(h, d, len);
}

bool ac_out_write(ac_out_t *h, const void *d, size_t len) {
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
  if (h->fd > -1 && h->options.fd_owner)
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
static void suffix_filename_with_id(char *dest, const char *filename,
                                    size_t id) {
  strcpy(dest, filename);
  if (ac_io_extension(filename, ".lz4"))
    sprintf(dest + strlen(dest) - 4, "_%lu.lz4", id);
  else if (ac_io_extension(filename, ".gz"))
    sprintf(dest + strlen(dest) - 3, "_%lu.gz", id);
  else
    sprintf(dest + strlen(dest), "_%lu", id);
}

/** ac_out_partitioned_t **/
typedef struct {
  int type;
  ac_out_options_t options;
  ac_out_write_f write_record;

  ac_out_ext_options_t ext_options;

  ac_out_t **partitions;
  size_t num_partitions;
  ac_io_partition_f partition;
  void *partition_tag;
} ac_out_partitioned_t;

bool write_partitioned_record(ac_out_t *hp, const void *d, size_t len) {
  ac_out_partitioned_t *h = (ac_out_partitioned_t *)hp;

  ac_io_record_t r;
  r.length = len;
  r.record = (char *)d;
  r.tag = 0;

  size_t partition = h->partition(&r, h->num_partitions, h->partition_tag);
  if (partition >= h->num_partitions)
    return false;

  ac_out_t *o = h->partitions[partition];
  return o->write_record(o, d, len);
}

ac_out_t *ac_out_partitioned_init(const char *filename,
                                  ac_out_options_t *options,
                                  ac_out_ext_options_t *ext_options) {
  if (ext_options->num_partitions == 0)
    return ac_out_init(filename, options);
  else if (ext_options->num_partitions == 1) {
    if (!filename)
      abort();
    // give suffix to filename
    char *tmp_name = (char *)ac_malloc(strlen(filename) + 3);
    suffix_filename_with_id(tmp_name, filename, 0);
    ac_out_t *r = ac_out_init(tmp_name, options);
    ac_free(tmp_name);
    return r;
  } else {
    if (!filename)
      abort();

    ac_out_partitioned_t *h = (ac_out_partitioned_t *)ac_malloc(
        sizeof(ac_out_partitioned_t) +
        (sizeof(ac_out_t *) * ext_options->num_partitions));
    h->options = *options;
    h->ext_options = *ext_options;
    h->partitions = (ac_out_t **)(h + 1);
    h->num_partitions = ext_options->num_partitions;
    h->partition = ext_options->partition;
    h->partition_tag = ext_options->partition_tag;

    char *tmp_name = (char *)ac_malloc(strlen(filename) + 20);
    for (size_t i = 0; i < h->num_partitions; i++) {
      suffix_filename_with_id(tmp_name, filename, i);
      // printf("%s\n", tmp_name);
      h->partitions[i] = ac_out_init(tmp_name, options);
    }
    h->write_record = write_partitioned_record;
    ac_free(tmp_name);
    h->type = AC_OUT_PARTITIONED_TYPE;
    return (ac_out_t *)h;
  }
}

void ac_out_partitioned_destroy(ac_out_t *hp) {
  ac_out_partitioned_t *h = (ac_out_partitioned_t *)hp;
  for (size_t i = 0; i < h->num_partitions; i++)
    ac_out_destroy(h->partitions[i]);
  ac_free(h);
}

typedef struct {
  int type;
  ac_out_options_t options;
  ac_out_write_f write_record;

  ac_out_ext_options_t ext_options;
} ac_out_sorted_t;

ac_out_t *ac_out_sorted_init(const char *filename, ac_out_options_t *options,
                             ac_out_ext_options_t *ext_options) {
  return NULL;
}

void ac_out_sorted_destroy(ac_out_t *hp) {}

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
  if (ext_options->compare)
    return ac_out_sorted_init(filename, options, ext_options);
  else if (ext_options->partition)
    return ac_out_partitioned_init(filename, options, ext_options);
  return ac_out_init(filename, options);
}
