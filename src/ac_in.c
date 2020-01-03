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
typedef ac_io_record_t *(*ac_in_advance_unique_f)(ac_in_t *h, size_t *num_r);

static const int AC_IN_NORMAL_TYPE = 0; // default (due to memset)
static const int AC_IN_EXT_TYPE = 1;

struct ac_in_s {
  ac_in_options_t options;
  int type;
  ac_io_record_t rec;
  ac_io_record_t *current;
  size_t num_current;
  ac_io_record_t *current_tmp;
  size_t num_current_tmp;
  ac_in_advance_f advance;
  ac_in_advance_f advance_tmp;
  ac_in_advance_unique_f advance_unique;
  ac_in_advance_unique_f advance_unique_tmp;
  ac_buffer_t *group_bh;

  ac_in_base_t *base;

  char delimiter;
  uint32_t fixed;

  ac_lz4_t *lz4;
  ac_buffer_t *bh; // for overflow
  ac_in_buffer_t buf;
  uint32_t block_size;
  uint32_t block_header_size;
  char *zerop;
  char zero;
};

ac_io_record_t *ac_in_advance_unique_single(ac_in_t *h, size_t *num_r) {
  if (!h) {
    *num_r = 0;
    return NULL;
  }
  ac_io_record_t *r = h->advance(h);
  *num_r = h->num_current;
  return r;
}

ac_io_record_t *ac_in_advance_group(ac_in_t *h, size_t *num_r,
                                    ac_io_compare_f compare, void *tag) {
  if (!h) {
    *num_r = 0;
    return NULL;
  }

  if (!h->group_bh)
    h->group_bh = ac_buffer_init(4096);

  ac_buffer_t *bh = h->group_bh;
  ac_buffer_clear(bh);

  *num_r = 0;

  ac_io_record_t *r;
  if ((r = h->advance(h)) == NULL)
    return NULL;

  ac_buffer_set(bh, &(r->length), sizeof(r->length));
  ac_buffer_append(bh, &(r->tag), sizeof(r->tag));
  ac_buffer_append(bh, r->record, r->length);
  ac_buffer_appendc(bh, 0);
  size_t num_records = 1;

  ac_io_record_t r1;
  r1 = *r;
  while ((r = h->advance(h)) != NULL) {
    char *b = ac_buffer_data(bh);
    b += sizeof(r->length) + sizeof(r->tag);
    r1.record = b;

    if (compare(&r1, r, tag)) {
      ac_in_reset(h);
      break;
    }
    ac_buffer_set(bh, &(r->length), sizeof(r->length));
    ac_buffer_append(bh, &(r->tag), sizeof(r->tag));
    ac_buffer_append(bh, r->record, r->length);
    ac_buffer_appendc(bh, 0);
    num_records++;
  }
  size_t len = ac_buffer_length(bh);
  ac_io_record_t *res = (ac_io_record_t *)ac_buffer_append_alloc(
      bh, num_records * sizeof(ac_io_record_t));
  ac_io_record_t *rp = res;
  char *ptr = ac_buffer_data(bh);
  char *endp = ptr + len;
  while (ptr < endp) {
    rp->length = (*(uint32_t *)(ptr));
    ptr += sizeof(uint32_t);
    rp->tag = (*(int *)(ptr));
    ptr += sizeof(int);
    rp->record = ptr;
    ptr += rp->length;
    ptr++;
    rp++;
  }
  h->num_current = num_records;
  h->current = res;
  *num_r = num_records;
  return res;
}

ac_io_record_t *ac_in_advance_unique(ac_in_t *h, size_t *num_r) {
  if (!h) {
    *num_r = 0;
    return NULL;
  }
  return h->advance_unique(h, num_r);
}

ac_io_record_t *ac_in_current(ac_in_t *h) {
  if (!h)
    return NULL;
  return h->current;
}

ac_io_record_t *ac_in_advance(ac_in_t *h) {
  if (!h)
    return NULL;

  return h->advance(h);
}

ac_io_record_t *_advance_prefix(ac_in_t *h) {
  h->num_current = 1;
  char *p = ac_in_base_read(h->base, 4);
  if (!p) {
    h->current = NULL;
    h->num_current = 0;
    return NULL;
  }
  uint32_t length = (*(uint32_t *)p);
  if (length > 0) {
    int32_t len = 0;
    p = ac_in_base_readz(h->base, &len, length);
    if (len != length) {
      h->current = NULL;
      h->num_current = 0;
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
  h->num_current = 1;
  char *p = ac_in_base_readz(h->base, &len, h->fixed);
  if (len != length) {
    h->current = NULL;
    h->num_current = 0;
    return NULL;
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  return h->current;
}

ac_io_record_t *_advance_fixed(ac_in_t *h) {
  return _advance_fixed_(h, h->fixed);
}

ac_io_record_t *empty_record(ac_in_t *h) { return NULL; }

ac_io_record_t *empty_record_unique(ac_in_t *h, size_t *len) {
  *len = 0;
  return NULL;
}

static inline ac_io_record_t *_advance_delimited_(ac_in_t *h, char delimiter,
                                                  bool full_record_required) {
  int32_t len = 0;
  char *p =
      ac_in_base_read_delimited(h->base, &len, delimiter, full_record_required);
  if (!p) {
    h->current = NULL;
    h->num_current = 0;
    return NULL;
  }
  h->rec.length = len;
  h->rec.record = p;
  h->current = &(h->rec);
  h->num_current = 1;
  return h->current;
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
    h->num_current = 0;
    return NULL;
  }
  uint32_t length = (*(uint32_t *)p);
  if (length > 0) {
    int32_t len = 0;
    p = ac_in_lz4_readz(h, &len, length);
    if (len != length) {
      h->current = NULL;
      h->num_current = 0;
      return NULL;
    }
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  h->num_current = 1;
  return h->current;
}

static inline ac_io_record_t *_advance_fixed_lz4_(ac_in_t *h, uint32_t length) {
  int32_t len = 0;
  char *p = ac_in_lz4_readz(h, &len, length);
  if (len != length) {
    h->current = NULL;
    h->num_current = 0;
    return NULL;
  }
  h->rec.length = length;
  h->rec.record = p;
  h->current = &(h->rec);
  h->num_current = 1;
  return h->current;
}

ac_io_record_t *_advance_fixed_lz4(ac_in_t *h) {
  return _advance_fixed_lz4_(h, h->fixed);
}

static inline ac_io_record_t *
_advance_delimited_lz4_(ac_in_t *h, char delimiter, bool full_record_required) {
  int32_t len = 0;
  char *p = ac_in_lz4_read_delimited(h, &len, delimiter, full_record_required);
  if (!p) {
    h->current = NULL;
    h->num_current = 0;
    return NULL;
  }
  h->rec.length = len;
  h->rec.record = p;
  h->current = &(h->rec);
  h->num_current = 1;
  return h->current;
}

ac_io_record_t *_advance_delimited_lz4(ac_in_t *h) {
  return _advance_delimited_lz4_(h, h->delimiter,
                                 h->options.full_record_required);
}

static inline void post_reset(ac_in_t *h) {
  h->current = h->current_tmp;
  h->num_current = h->num_current_tmp;
  h->current_tmp = NULL;
  h->num_current_tmp = 0;
  h->advance = h->advance_tmp;
  h->advance_unique = h->advance_unique_tmp;
}

static ac_io_record_t *_reset_(ac_in_t *h) {
  post_reset(h);
  return h->current;
}

static ac_io_record_t *_reset_unique_(ac_in_t *h, size_t *num_r) {
  post_reset(h);
  *num_r = h->num_current;
  return h->current;
}

void ac_in_reset(ac_in_t *h) {
  h->advance = _reset_;
  h->advance_unique = _reset_unique_;
  h->current_tmp = h->current;
  h->current = NULL;
  h->num_current_tmp = h->num_current;
  h->num_current = 0;
}

void ac_in_ext_destroy(ac_in_t *h);

void ac_in_destroy(ac_in_t *h) {
  if (h->type == AC_IN_EXT_TYPE)
    ac_in_ext_destroy(h);
  else {
    if (h->base)
      ac_in_base_destroy(h->base);
    if (h->lz4)
      ac_lz4_destroy(h->lz4);
    ac_free(h);
  }
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

void ac_in_empty(ac_in_t *h) {
  if (h->group_bh)
    ac_buffer_destroy(h->group_bh);
  h->current = NULL;
  h->current_tmp = NULL;
  h->num_current = 0;
  h->num_current_tmp = 0;
  h->advance = empty_record;
  h->advance_unique = empty_record_unique;
  h->advance_unique_tmp = h->advance_unique;
  h->advance_tmp = h->advance;
}

ac_in_t *_ac_in_init(const char *filename, int fd, bool can_close, void *buf,
                     size_t buf_len, bool can_free, ac_in_options_t *options) {
  ac_in_options_t opts;
  if (!options) {
    options = &opts;
    ac_in_options_init(options);
  }

  if (!filename && fd == -1 && !buf)
    abort();

  bool is_lz4 = false;
  if ((options->lz4 && (buf || !filename)) ||
      ac_io_extension(filename, ".lz4")) {
    if (!options->compressed_buffer_size) {
      options->compressed_buffer_size = options->buffer_size;
    }
    is_lz4 = true;
    size_t tmp = options->buffer_size;
    options->buffer_size = options->compressed_buffer_size;
    options->compressed_buffer_size = tmp;
  }

  ac_in_base_t *base = NULL;
  if (buf) {
    if (options->gz)
      abort();

    base = ac_in_base_init_from_buffer(buf, buf_len, can_free);
  } else {
    if ((!filename && options->gz) || ac_io_extension(filename, ".gz"))
      base = ac_in_base_init_gz(filename, fd, can_close, options->buffer_size);
    else
      base = ac_in_base_init(filename, fd, can_close, options->buffer_size);
  }
  ac_in_t *h = NULL;
  if (!base) {
    if (options->abort_on_file_not_found)
      abort();

    h = (ac_in_t *)ac_calloc(sizeof(ac_in_t));
    ac_in_empty(h);
    return h;
  }

  if (is_lz4) {
    char *headerp = ac_in_base_read(base, 7);
    if (!headerp) {
      ac_in_base_destroy(base);
      if (options->abort_on_file_empty)
        abort();
      h = (ac_in_t *)ac_calloc(sizeof(ac_in_t));
      ac_in_empty(h);
      return h;
    }
    ac_lz4_t *lz4 = ac_lz4_init_decompress(headerp, 7);
    if (!lz4) {
      ac_in_base_destroy(base);
      if (options->abort_on_error)
        abort();
      h = (ac_in_t *)ac_calloc(sizeof(ac_in_t));
      ac_in_empty(h);
      return h;
    }
    uint32_t buffer_size = options->buffer_size;
    uint32_t block_size = ac_lz4_block_size(lz4);
    uint32_t block_header_size = ac_lz4_block_header_size(lz4);
    uint32_t compressed_size = ac_lz4_compressed_size(lz4);
    if (buffer_size < compressed_size + block_header_size + 4)
      base = ac_in_base_reinit(base, compressed_size + block_header_size + 4);

    buffer_size = options->compressed_buffer_size;
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
    if (options->format < 0) {
      h->delimiter = (-options->format) - 1;
      h->advance = _advance_delimited;
    } else if (options->format > 0) {
      h->fixed = options->format;
      h->advance = _advance_fixed;
    } else
      h->advance = _advance_prefix;
  }
  h->advance_unique = ac_in_advance_unique_single;
  h->advance_unique_tmp = h->advance_unique;
  h->advance_tmp = h->advance;
  return h;
}

ac_in_t *ac_in_init(const char *filename, ac_in_options_t *options) {
  return _ac_in_init(filename, -1, true, NULL, 0, false, options);
}

ac_in_t *ac_in_init_with_fd(int fd, bool can_close, ac_in_options_t *options) {
  return _ac_in_init(NULL, fd, can_close, NULL, 0, false, options);
}

ac_in_t *ac_in_init_with_buffer(void *buf, size_t len, bool can_free,
                                ac_in_options_t *options) {
  return _ac_in_init(NULL, -1, false, buf, len, can_free, options);
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
  h->buffer_size = 128 * 1024;
  h->compressed_buffer_size = 0;

  h->full_record_required = true;
  h->format = 0;
  h->abort_on_error = false;
  h->abort_on_partial_record = false;
  h->abort_on_file_not_found = false;
  h->abort_on_file_empty = false;
  h->tag = 0;
  h->gz = false;
  h->lz4 = false;
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

void ac_in_options_allow_partial_records(ac_in_options_t *h) {
  h->full_record_required = false;
  h->abort_on_partial_record = false;
}

void ac_in_options_abort_on_partial_record(ac_in_options_t *h) {
  h->abort_on_partial_record = true;
}

void ac_in_options_abort_on_file_not_found(ac_in_options_t *h) {
  h->abort_on_file_not_found = true;
}

void ac_in_options_abort_on_file_empty(ac_in_options_t *h) {
  h->abort_on_file_empty = true;
}

void ac_in_options_tag(ac_in_options_t *h, int tag) { h->tag = tag; }

void ac_in_options_gz(ac_in_options_t *h, size_t buffer_size) { h->gz = true; }

void ac_in_options_lz4(ac_in_options_t *h, size_t buffer_size) {
  h->lz4 = true;
  h->compressed_buffer_size = buffer_size;
}

/*****************************************************************************
  ac_in_ext... functionality

  in_heap_t tracks a heap full of ac_in_t objects and sorts based upon given
  comparison function.
*/

struct in_heap_s;
typedef struct in_heap_s in_heap_t;

struct in_heap_s {
  ssize_t size;
  ssize_t max_size;
  ac_in_t **heap;
  ac_io_compare_f compare;
  void *compare_tag;
};

static inline void in_heap_init(in_heap_t *h, ssize_t mx,
                                ac_io_compare_f compare, void *tag) {
  h->size = 0;
  if (mx < 2)
    mx = 2;
  h->max_size = mx;
  h->heap = (ac_in_t **)ac_malloc((mx + 1) * sizeof(ac_in_t *));
  h->compare = compare;
  h->compare_tag = tag;
}

static inline void in_heap_clear(in_heap_t *h) { h->size = 0; }

static inline size_t in_heap_max(in_heap_t *h) { return h->max_size; }

static inline void in_heap_destroy(in_heap_t *h) {
  if (!h)
    return;
  ac_free(h->heap);
}

static inline ac_in_t **in_heap_base(in_heap_t *h) { return h->heap; }

static inline size_t in_heap_size(in_heap_t *h) { return h->size; }

static inline int in_heap_compare(in_heap_t *h, ac_in_t *a, ac_in_t *b) {
  return h->compare(a->current, b->current, h->compare_tag);
}

static inline int in_heap_compare2(in_heap_t *h, ac_io_record_t *a,
                                   ac_in_t *b) {
  return h->compare(a, b->current, h->compare_tag);
}

static inline void in_heap_push(in_heap_t *h, ac_in_t *item) {
  if (h->size >= h->max_size) {
    h->max_size = h->size * 2;
    ac_in_t **heap =
        (ac_in_t **)ac_malloc((h->max_size + 1) * sizeof(ac_in_t *));
    memcpy(heap, h->heap, (h->size * sizeof(ac_in_t *)));
    ac_free(h->heap);
    h->heap = heap;
  }
  h->size++;
  ssize_t num = h->size;
  ac_in_t **heap = h->heap;
  ac_in_t *tmp = heap[num];
  heap[num] = item;
  heap[0] = tmp;
  ssize_t i = num;
  ssize_t j = i >> 1;

  while (j > 0 && in_heap_compare(h, heap[i], heap[j]) < 0) {
    tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;
    i = j;
    j = j >> 1;
  }
}

static inline ac_in_t *in_heap_pop(in_heap_t *h) {
  h->size--;
  ssize_t num = h->size;
  ac_in_t **heap = h->heap;
  ac_in_t *r = heap[1];
  heap[1] = heap[num + 1];

  ssize_t i = 1;
  ssize_t j = i << 1;
  ssize_t k = j + 1;

  if (k <= num && in_heap_compare(h, heap[k], heap[j]) < 0)
    j = k;

  while (j <= num && in_heap_compare(h, heap[j], heap[i]) < 0) {
    ac_in_t *tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;

    i = j;
    j = i << 1;
    k = j + 1;
    if (k <= num && in_heap_compare(h, heap[k], heap[j]) < 0)
      j = k;
  }
  return r;
}

/*
  The ac_in_ext_t structure needs to share the same members as ac_in_s up
  through group_bh.
*/

typedef struct {
  ac_in_options_t options;
  int type;
  ac_io_record_t rec;
  ac_io_record_t *current;
  size_t num_current;
  ac_io_record_t *current_tmp;
  size_t num_current_tmp;
  ac_in_advance_f advance;
  ac_in_advance_f advance_tmp;
  ac_in_advance_unique_f advance_unique;
  ac_in_advance_unique_f advance_unique_tmp;
  ac_buffer_t *group_bh;

  ac_in_t **active;
  size_t num_active;
  size_t active_size;
  ac_io_record_t *r;

  in_heap_t heap;

  ac_buffer_t *reducer_bh;
  ac_io_reducer_f reducer;
  void *reducer_tag;

  ac_io_compare_f compare;
  void *compare_tag;
} ac_in_ext_t;

ac_in_t *ac_in_ext_init(ac_io_compare_f compare, void *tag,
                        ac_in_options_t *options) {
  ac_in_options_t opts;
  if (!options) {
    options = &opts;
    ac_in_options_init(options);
  }

  ac_in_ext_t *h = (ac_in_ext_t *)ac_malloc(sizeof(ac_in_ext_t));
  memset(h, 0, sizeof(*h));
  h->type = AC_IN_EXT_TYPE;
  h->compare = compare;
  h->compare_tag = tag;
  h->options = *options;
  in_heap_init(&(h->heap), 0, compare, tag);

  ac_in_empty((ac_in_t *)h);
  return (ac_in_t *)h;
}

void ac_in_ext_destroy(ac_in_t *hp) {
  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  if (!h)
    return;

  for (size_t i = 0; i < h->num_active; i++)
    ac_in_destroy(h->active[i]);

  if (h->active)
    ac_free(h->active);

  in_heap_t *heap = &(h->heap);

  while (in_heap_size(heap))
    ac_in_destroy(in_heap_pop(heap));

  in_heap_destroy(heap);

  if (h->reducer_bh)
    ac_buffer_destroy(h->reducer_bh);
  ac_free(h);
}

static void move_active_to_heap(ac_in_ext_t *h, bool advance) {
  if (!h)
    return;

  in_heap_t *heap = &(h->heap);
  for (size_t i = 0; i < h->num_active; i++) {
    ac_in_t *in = h->active[i];
    if (advance) {
      if (!ac_in_advance(in)) {
        ac_in_destroy(in);
        continue;
      }
    }
    in_heap_push(heap, in);
  }
  h->num_active = 0;
}

ac_io_record_t *ac_in_ext_advance(ac_in_t *hp) {
  if (!hp)
    return NULL;

  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  move_active_to_heap(h, true);
  in_heap_t *heap = &(h->heap);
  if (in_heap_size(heap)) {
    ac_in_t *in = in_heap_pop(heap);
    h->active[0] = in;
    h->num_active = 1;
    h->current = ac_in_current(in);
    return h->current;
  }
  ac_in_empty(hp);
  return NULL;
}

ac_io_record_t *ac_in_ext_advance_unique(ac_in_t *hp, size_t *num_r) {
  ac_io_record_t *first = ac_in_ext_advance(hp);
  if (!first)
    return NULL;

  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  in_heap_t *heap = &(h->heap);
  ac_in_t **activep = h->active + 1;
  ac_io_record_t *rp = h->r;
  *rp++ = *first;
  ac_in_t *in = NULL;
  while (in_heap_size(heap)) {
    in = in_heap_pop(heap);
    if (!in_heap_compare2(heap, first, in)) {
      *activep++ = in;
      *rp++ = *ac_in_current(in);
    } else {
      in_heap_push(heap, in);
      break;
    }
  }
  h->num_active = activep - h->active;
  h->num_current = h->num_active;
  h->current = h->r;
  *num_r = h->num_current;
  return h->current;
}

ac_io_record_t *ac_in_ext_advance_reduce(ac_in_t *hp) {
  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  while (1) {
    size_t num_r = 0;
    ac_io_record_t *r = ac_in_ext_advance_unique(hp, &num_r);
    if (!r)
      return NULL;
    if (h->reducer(&h->rec, r, num_r, h->reducer_bh, h->reducer_tag)) {
      r = h->current = &(h->rec);
      h->num_current = 1;
      return r;
    }
  }
  return NULL; // should not happen
}

void ac_in_ext_add(ac_in_t *hp, ac_in_t *in, int tag) {
  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  if (!h || !in || h->type != AC_IN_EXT_TYPE)
    return;

  ac_in_reset(in);
  if (!ac_in_advance(in)) {
    ac_in_destroy(in);
    return;
  }
  in->options.tag = tag;

  in_heap_t *heap = &(h->heap);
  move_active_to_heap(h, false);
  in_heap_push(heap, in);

  if (h->active_size < in_heap_size(heap)) {
    if (h->active)
      ac_free(h->active);
    h->active_size = in_heap_max(heap);
    h->active = (ac_in_t **)ac_malloc(
        (sizeof(ac_in_t *) + sizeof(ac_io_record_t)) * h->active_size);
    h->r = (ac_io_record_t *)(h->active + h->active_size);
  }
  h->num_active = 0;

  if (h->advance == empty_record) {
    if (h->reducer)
      h->advance = h->advance_tmp = ac_in_ext_advance_reduce;
    else
      h->advance = h->advance_tmp = ac_in_ext_advance;
    h->advance_unique = h->advance_unique_tmp = ac_in_ext_advance_unique;
  }
}

void ac_in_ext_reducer(ac_in_t *hp, ac_io_reducer_f reducer, void *tag) {
  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  if (!h || h->type != AC_IN_EXT_TYPE)
    return;

  if (h->advance == ac_in_ext_advance)
    h->advance = h->advance_tmp = ac_in_ext_advance_reduce;

  h->reducer = reducer;
  h->reducer_tag = tag;
  h->reducer_bh = ac_buffer_init(1024);
}

void ac_in_ext_keep_first(ac_in_t *hp) {
  ac_in_ext_t *h = (ac_in_ext_t *)hp;
  if (!h || h->type != AC_IN_EXT_TYPE)
    return;

  if (h->advance == ac_in_ext_advance)
    h->advance = h->advance_tmp = ac_in_ext_advance_reduce;

  h->reducer = ac_io_keep_first;
  h->reducer_tag = NULL;
  h->reducer_bh = NULL;
}
