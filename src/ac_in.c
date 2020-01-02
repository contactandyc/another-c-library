#include "ac_in.h"

#include "ac_buffer.h"
#include "ac_common.h"
#include "ac_io.h"
#include "ac_lz4.h"

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>

typedef ac_io_record_t *(*ac_in_advance_f)(ac_in_t *h);
typedef ac_io_record_t *(*ac_in_advance_fixed_f)(ac_in_t *h, uint32_t length);
typedef ac_io_record_t *(*ac_in_advance_delimited_f)(ac_in_t *h, char delimiter,
                                                     bool full_record_required);
typedef void *(*ac_in_read_f)(ac_in_t *h, int32_t *rlen, uint32_t length);

struct ac_in_s {
  ac_in_options_t options;
  ac_in_base_t *base;

  ac_lz4_t *lz4;
  ac_buffer_t *bh; // for overflow
  ac_in_buffer_t buf;
  uint32_t block_size;
  uint32_t block_header_size;
  char *zerop;
  char zero;

  char delimiter;
  uint32_t fixed;

  ac_io_record_t rec;
  ac_io_record_t *current;

  ac_in_read_f read_data;
  ac_in_advance_f advance;
  ac_in_advance_f advance_prefix;
  ac_in_advance_fixed_f advance_fixed;
  ac_in_advance_delimited_f advance_delimited;
};

ac_io_record_t *ac_in_advance(ac_in_t *h) { return h->advance(h); }

ac_io_record_t *ac_in_advance_prefix(ac_in_t *h) {
  return h->advance_prefix(h);
}

ac_io_record_t *ac_in_advance_fixed(ac_in_t *h, uint32_t length) {
  return h->advance_fixed(h, length);
}

ac_io_record_t *ac_in_advance_delimited(ac_in_t *h, char delimiter,
                                        bool full_record_required) {
  return h->advance_delimited(h, delimiter, full_record_required);
}

void *ac_in_read(ac_in_t *h, int32_t *rlen, uint32_t length) {
  return h->read_data(h, rlen, length);
}

ac_io_record_t *_advance_prefix(ac_in_t *h) {
  char *p = ac_in_base_read(h->base, 4);
  if (!p) {
    h->current = NULL;
    return NULL;
  }
  uint32_t length = (*(uint32_t *)p);
  if (length > 0) {
    int32_t len = 0;
    p = ac_in_base_readz(h->base, &len, length);
    if (len != length) {
      h->current = NULL;
      return NULL;
    }
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

static inline ac_io_record_t *_advance_fixed_(ac_in_t *h, uint32_t length) {
  int32_t len = 0;
  char *p = ac_in_base_readz(h->base, &len, length);
  if (len != length) {
    h->current = NULL;
    return NULL;
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

ac_io_record_t *_advance_fixed_int(ac_in_t *h, uint32_t length) {
  return _advance_fixed_(h, length);
}

ac_io_record_t *_advance_fixed(ac_in_t *h) {
  return _advance_fixed_(h, h->fixed);
}

void *_read_data(ac_in_t *h, int32_t *rlen, uint32_t length) {
  return ac_in_base_readz(h->base, rlen, length);
}

static inline ac_io_record_t *_advance_delimited_(ac_in_t *h, char delimiter,
                                                  bool full_record_required) {
  int32_t len = 0;
  char *p =
      ac_in_base_read_delimited(h->base, &len, delimiter, full_record_required);
  if (!p) {
    h->current = NULL;
    return NULL;
  }
  h->rec.length = len;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

ac_io_record_t *_advance_delimited_int(ac_in_t *h, char delimiter,
                                       bool full_record_required) {
  return _advance_delimited_(h, delimiter, full_record_required);
}

ac_io_record_t *_advance_delimited(ac_in_t *h) {
  return _advance_delimited_(h, h->delimiter, h->options.full_record_required);
}

char *ac_in_lz4_read(ac_in_t *h, uint32_t len);
char *ac_in_lz4_readz(ac_in_t *h, int32_t *rlen, uint32_t len);
char *ac_in_lz4_read_delimited(ac_in_t *h, int32_t *rlen, char delim,
                               bool required);

ac_io_record_t *_advance_prefix_lz4(ac_in_t *h) {
  char *p = ac_in_lz4_read(h, 4);
  if (!p) {
    h->current = NULL;
    return NULL;
  }
  uint32_t length = (*(uint32_t *)p);
  if (length > 0) {
    int32_t len = 0;
    p = ac_in_lz4_readz(h, &len, length);
    if (len != length) {
      h->current = NULL;
      return NULL;
    }
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

static inline ac_io_record_t *_advance_fixed_lz4_(ac_in_t *h, uint32_t length) {
  int32_t len = 0;
  char *p = ac_in_lz4_readz(h, &len, length);
  if (len != length) {
    h->current = NULL;
    return NULL;
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

ac_io_record_t *_advance_fixed_lz4_int(ac_in_t *h, uint32_t length) {
  return _advance_fixed_lz4_(h, length);
}

ac_io_record_t *_advance_fixed_lz4(ac_in_t *h) {
  return _advance_fixed_lz4_(h, h->fixed);
}

void *_read_data_lz4(ac_in_t *h, int32_t *rlen, uint32_t length) {
  return ac_in_lz4_readz(h, rlen, length);
}

static inline ac_io_record_t *
_advance_delimited_lz4_(ac_in_t *h, char delimiter, bool full_record_required) {
  int32_t len = 0;
  char *p = ac_in_lz4_read_delimited(h, &len, delimiter, full_record_required);
  if (!p) {
    h->current = NULL;
    return NULL;
  }
  h->rec.length = len;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

ac_io_record_t *_advance_delimited_lz4_int(ac_in_t *h, char delimiter,
                                           bool full_record_required) {
  return _advance_delimited_lz4_(h, delimiter, full_record_required);
}

ac_io_record_t *_advance_delimited_lz4(ac_in_t *h) {
  return _advance_delimited_lz4_(h, h->delimiter,
                                 h->options.full_record_required);
}

void ac_in_destroy(ac_in_t *h) {
  ac_in_base_destroy(h->base);
  if (h->lz4)
    ac_lz4_destroy(h->lz4);
  ac_free(h);
}

void reset_block(ac_in_buffer_t *b) {
  memmove(b->buffer, b->buffer + b->pos, b->used - b->pos);
  b->used -= b->pos;
  b->pos = 0;
}

static int read_lz4_block(ac_in_t *h, ac_in_buffer_t *dest) {
  uint32_t *s = (uint32_t *)ac_in_base_read(h->base, 4);
  if (!s)
    return 0;

  uint32_t length = *s;
  bool compressed = true;
  if (length & 0x80000000U) {
    compressed = false;
    length -= 0x80000000U;
  }
  if (!length)
    return 0;

  length += h->block_header_size;
  char *p = ac_in_base_read(h->base, length);
  if (!p)
    return 0;

  char *dp = dest->buffer + dest->used;
  int n = ac_lz4_decompress(h->lz4, p, length, dp, h->block_size, compressed);
  if (n < 0)
    return -1;

  dest->used += n;
  return n;
}

static void fill_blocks(ac_in_t *h, ac_in_buffer_t *dest) {
  while (1) {
    if (dest->used + h->block_size <= dest->size) {
      if (read_lz4_block(h, dest) <= 0) {
        dest->eof = true;
        return;
      }
    } else
      return;
  }
}

ac_in_t *ac_in_init(const char *filename, ac_in_options_t *options) {
  ac_in_options_t opts;
  if (!options) {
    options = &opts;
    ac_in_options_init(options);
  }

  if (!filename && options->fd == -1 && !options->buffer)
    abort();

  bool is_lz4 = false;
  if ((options->lz4 && (options->buffer || !filename)) ||
      ac_io_extension(filename, ".lz4")) {
    if (!options->lz4_buffer_size) {
      options->lz4_buffer_size = options->buffer_size;
    }
    is_lz4 = true;
    size_t tmp = options->buffer_size;
    options->buffer_size = options->lz4_buffer_size;
    options->lz4_buffer_size = tmp;
  }

  ac_in_base_t *base = NULL;
  if (options->buffer) {
    if (options->gz)
      abort();

    base = ac_in_base_init_from_buffer(options->buffer, options->buffer_length,
                                       options->buffer_owned);
  } else {
    if ((!filename && options->gz) || ac_io_extension(filename, ".gz"))
      base = ac_in_base_init_gz(filename, options->fd, options->buffer_size);
    else
      base = ac_in_base_init(filename, options->fd, options->buffer_size);
  }
  if (!base)
    return NULL;

  ac_in_t *h = NULL;
  if (is_lz4) {
    char *headerp = ac_in_base_read(base, 7);
    if (!headerp) {
      ac_in_base_destroy(base);
      return NULL;
    }
    ac_lz4_t *lz4 = ac_lz4_init_decompress(headerp, 7);
    if (!lz4) {
      ac_in_base_destroy(base);
      return NULL;
    }
    uint32_t buffer_size = options->buffer_size;
    uint32_t block_size = ac_lz4_block_size(lz4);
    uint32_t block_header_size = ac_lz4_block_header_size(lz4);
    uint32_t compressed_size = ac_lz4_compressed_size(lz4);
    if (buffer_size < compressed_size + block_header_size + 4)
      base = ac_in_base_reinit(base, compressed_size + block_header_size + 4);

    buffer_size = options->lz4_buffer_size;
    if (buffer_size < block_size)
      buffer_size = block_size;

    h = (ac_in_t *)ac_malloc(sizeof(ac_in_t) + buffer_size + 1);
    memset(h, 0, sizeof(*h));
    h->lz4 = lz4;
    h->buf.buffer = (char *)(h + 1);
    h->buf.size = buffer_size;
    h->block_size = block_size;
    h->block_header_size = block_header_size;

    h->options = *options;
    h->base = base;
    h->rec.tag = options->tag;
    h->advance_prefix = _advance_prefix_lz4;
    h->advance_fixed = _advance_fixed_lz4_int;
    h->advance_delimited = _advance_delimited_lz4_int;
    h->read_data = _read_data_lz4;
    if (options->format < 0) {
      h->delimiter = (-options->format) - 1;
      h->advance = _advance_delimited_lz4;
    } else if (options->format > 0) {
      h->fixed = options->format;
      h->advance = _advance_fixed_lz4;
    } else
      h->advance = _advance_prefix_lz4;
    fill_blocks(h, &(h->buf));
  } else {
    h = (ac_in_t *)ac_malloc(sizeof(ac_in_t));
    h->options = *options;
    h->base = base;
    h->rec.tag = options->tag;
    h->advance_prefix = _advance_prefix;
    h->advance_fixed = _advance_fixed_int;
    h->advance_delimited = _advance_delimited_int;
    h->read_data = _read_data;
    if (options->format < 0) {
      h->delimiter = (-options->format) - 1;
      h->advance = _advance_delimited;
    } else if (options->format > 0) {
      h->fixed = options->format;
      h->advance = _advance_fixed;
    } else
      h->advance = _advance_prefix;
  }
  return h;
}

static inline char *end_of_block(ac_in_t *h, int32_t *rlen, char *p, char *ep,
                                 bool required) {
  if (required)
    return NULL;
  else {
    h->zerop = ep;
    h->zero = *ep;
    *ep = 0;
    *rlen = (ep - p);
    return p;
  }
}

static inline void cleanup_last_read(ac_in_t *h) {
  if (h->bh) {
    ac_buffer_destroy(h->bh);
    h->bh = NULL;
  }

  if (h->zerop) {
    (*h->zerop) = h->zero;
    h->zerop = NULL;
  }
}

char *ac_in_lz4_read_delimited(ac_in_t *h, int32_t *rlen, char delim,
                               bool required) {
  cleanup_last_read(h);
  *rlen = 0;

  ac_in_buffer_t *b = &(h->buf);
  char *p = b->buffer + b->pos;

  char *sp = p;
  // 2. search for delimiter between pos/used
  char *ep = b->buffer + b->used;
  while (p < ep) {
    if (*p != delim)
      p++;
    else {
      *rlen = (p - sp);
      b->pos += (*rlen) + 1;
      h->zerop = p;
      h->zero = *p;
      *p = 0;
      return sp;
    }
  }

  // 3. if finished, there is no more data to read, return what is present
  if (b->eof) {
    b->pos = b->used;
    return end_of_block(h, rlen, sp, p, required);
  }

  // 4. reset the block such that the pos starts at zero and fill rest of
  //    block.  If pos was zero, nothing to do here
  if (b->pos > 0) {
    reset_block(b);
    sp = b->buffer;
    p = sp + b->used;
    b->pos = b->used;
    fill_blocks(h, b);
    char *ep = p + b->used;
    while (p < ep) {
      if (*p != delim)
        p++;
      else {
        *rlen = (p - sp);
        b->pos += (*rlen) + 1;
        h->zerop = p;
        h->zero = *p;
        *p = 0;
        return sp;
      }
    }
    if (b->eof) {
      b->pos = b->used;
      return end_of_block(h, rlen, sp, p, required);
    }
  }
  // 5. The delimiter was not found in b->size bytes, create a tmp buffer
  //    that can be used to handle full result.  Make 1.5x because it'll have
  //    to grow at least once most of the time if less than this.
  h->bh = ac_buffer_init((b->used * 3) / 2);
  while (1) {
    ac_buffer_append(h->bh, b->buffer, b->used);
    b->used = 0;
    b->pos = 0;
    fill_blocks(h, b);
    p = b->buffer;
    sp = p;
    ep = p + b->used;
    while (p < ep) {
      if (*p != delim)
        p++;
      else {
        size_t length = (p - sp);
        b->pos += length + 1;
        ac_buffer_append(h->bh, b->buffer, length);
        *rlen = ac_buffer_length(h->bh);
        return ac_buffer_data(h->bh);
      }
    }
    if (b->eof) {
      b->pos = b->used;
      if (required) {
        ac_buffer_destroy(h->bh);
        return NULL;
      } else {
        ac_buffer_append(h->bh, sp, p - sp);
        *rlen = ac_buffer_length(h->bh);
        return ac_buffer_data(h->bh);
      }
    }
  }
  // should not happen
  return NULL;
}

char *ac_in_lz4_readz(ac_in_t *h, int32_t *rlen, uint32_t len) {
  cleanup_last_read(h);

  ac_in_buffer_t *b = &(h->buf);
  *rlen = 0;

  char *p = b->buffer + b->pos;
  if (b->pos + len <= b->used) {
    b->pos += len;
    *rlen = len;
    char *ep = p + len;
    h->zero = *ep;
    h->zerop = ep;
    *ep = 0;
    return p;
  } else if (len > b->size) {
    if (b->eof) {
      *rlen = b->used - b->pos;
      b->pos = b->used;
      char *ep = p + (*rlen);
      h->zero = *ep;
      h->zerop = ep;
      *ep = 0;
      return p;
    }

    /* where length is greater than the
       internal buffer.  In this case, a buffer is used and
       all data is copied into it. */
    h->bh = ac_buffer_init(len);
    ac_buffer_resize(h->bh, len);
    ac_in_buffer_t tmp;
    tmp.buffer = ac_buffer_data(h->bh);
    tmp.size = len;
    tmp.used = b->used - b->pos;
    tmp.pos = tmp.used;
    if (tmp.used)
      memcpy(tmp.buffer, b->buffer + b->pos, tmp.used);
    b->pos = b->used = 0;
    fill_blocks(h, &tmp);
    tmp.pos = tmp.used;
    len -= tmp.used;
    if (len) {
      fill_blocks(h, b);
      if (len > b->used)
        len = b->used;
      b->pos = len;
      tmp.pos += len;
      memcpy(tmp.buffer + tmp.used, b->buffer, len);
    }
    *rlen = tmp.pos;
    return tmp.buffer;
  } else {
    if (b->eof) {
      *rlen = b->used - b->pos;
      b->pos = b->used;
      char *ep = p + (*rlen);
      h->zero = *ep;
      h->zerop = ep;
      *ep = 0;
      return p;
    }
    reset_block(b);
    fill_blocks(h, b);
    if (len > b->used)
      len = b->used;
    b->pos = len;
    *rlen = len;
    char *ep = b->buffer + len;
    h->zero = *ep;
    h->zerop = ep;
    *ep = 0;
    return b->buffer;
  }
}

char *ac_in_lz4_read(ac_in_t *h, uint32_t len) {
  cleanup_last_read(h);

  ac_in_buffer_t *b = &(h->buf);

  char *p = b->buffer + b->pos;
  if (b->pos + len <= b->used) {
    b->pos += len;
    return p;
  } else if (len > b->size) {
    if (b->eof) {
      b->pos = b->used;
      return NULL;
    }

    /* where length is greater than the
       internal buffer.  In this case, a buffer is used and
       all data is copied into it. */
    h->bh = ac_buffer_init(len);
    ac_buffer_resize(h->bh, len);
    ac_in_buffer_t tmp;
    tmp.buffer = ac_buffer_data(h->bh);
    tmp.size = len;
    tmp.used = b->used - b->pos;
    tmp.pos = tmp.used;
    if (tmp.used)
      memcpy(tmp.buffer, b->buffer + b->pos, tmp.used);
    b->pos = b->used = 0;
    fill_blocks(h, &tmp);
    tmp.pos = tmp.used;
    len -= tmp.used;
    if (len) {
      fill_blocks(h, b);
      if (len > b->used) {
        b->pos = b->used;
        ac_buffer_destroy(h->bh);
        h->bh = NULL;
        return NULL;
      }
      b->pos = len;
      tmp.pos += len;
      memcpy(tmp.buffer + tmp.used, b->buffer, len);
    }
    return tmp.buffer;
  } else {
    if (b->eof) {
      b->pos = b->used;
      return NULL;
    }
    reset_block(b);
    fill_blocks(h, b);
    if (len > b->used) {
      b->pos = b->used;
      return NULL;
    }
    b->pos = len;
    return b->buffer;
  }
}

void ac_in_options_init(ac_in_options_t *h) {
  h->buffer = NULL;
  h->buffer_length = 0;
  h->buffer_owned = false;
  h->buffer_size = 128 * 1024;
  h->lz4_buffer_size = 0;

  h->full_record_required = true;
  h->fd = -1;
  h->fd_owner = true;
  h->format = 0;
  h->abort_on_error = false;
  h->tag = 0;
  h->gz = false;
  h->lz4 = false;
}

void ac_in_options_fd(ac_in_options_t *h, int fd, bool owner) {
  h->fd = fd;
  h->fd_owner = owner;
}

void ac_in_options_buffer(ac_in_options_t *h, void *d, size_t len, bool owned) {
  h->buffer = (char *)d;
  h->buffer_length = len;
  h->buffer_owned = owned;
}

void ac_in_options_buffer_size(ac_in_options_t *h, size_t buffer_size) {
  h->buffer_size = buffer_size;
}

void ac_in_options_format(ac_in_options_t *h, ac_io_format_t format) {
  h->format = format;
}

void ac_in_options_abort_on_error(ac_in_options_t *h) {
  h->abort_on_error = true;
}

void ac_in_options_tag(ac_in_options_t *h, int tag) { h->tag = tag; }

void ac_in_options_gz(ac_in_options_t *h) { h->gz = true; }

void ac_in_options_lz4(ac_in_options_t *h, size_t buffer_size) {
  h->lz4 = true;
  h->lz4_buffer_size = buffer_size;
}
