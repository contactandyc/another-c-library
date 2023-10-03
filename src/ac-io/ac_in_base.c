#include "another-c-library/ac_in_base.h"

#include "another-c-library/ac_allocator.h"
#include "another-c-library/ac_buffer.h"

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>

struct ac_in_base_s {
  ac_in_buffer_t buf;
  char *filename;
  int fd;
  gzFile gz;
  bool can_close;
  ac_buffer_t *bh;
  char *zerop;
  char zero;
};

static inline void reset_block(ac_in_buffer_t *b) {
  memmove(b->buffer, b->buffer + b->pos, b->used - b->pos);
  b->used -= b->pos;
  b->pos = 0;
}

static void fill_blocks(ac_in_base_t *h, ac_in_buffer_t *b) {
  if (b->eof)
    return;

  int bytes = b->size - b->used;
  int n;
  if (h->fd != -1)
    n = read(h->fd, b->buffer + b->used, bytes);
  else if (h->gz)
    n = gzread(h->gz, b->buffer + b->used, bytes);
  else
    return;

  if (n >= 0)
    b->used += n;
  if (n < bytes) {
    b->eof = true;
    b->size = b->used;
  }
}

static inline void cleanup_last_read(ac_in_base_t *h) {
  if (h->bh) {
    ac_buffer_destroy(h->bh);
    h->bh = NULL;
  }

  if (h->zerop) {
    (*h->zerop) = h->zero;
    h->zerop = NULL;
  }
}

const char *ac_in_base_filename(ac_in_base_t *h) { return h->filename; }

ac_in_base_t *ac_in_base_reinit(ac_in_base_t *base, size_t buffer_size) {
  if (base->fd == -1 && base->gz == NULL)
    return base;

  size_t filename_length = base->filename ? strlen(base->filename) + 1 : 0;
  ac_in_base_t *h = (ac_in_base_t *)ac_malloc(
      sizeof(ac_in_base_t) + buffer_size + 1 + filename_length);
  memcpy(h, base, sizeof(*h));
  h->buf.buffer = (char *)(h + 1);
  h->buf.size = buffer_size;
  if (filename_length) {
    h->filename = h->buf.buffer + buffer_size + 1;
    strcpy(h->filename, base->filename);
  }
  if (h->buf.used)
    memcpy(h->buf.buffer, base->buf.buffer, h->buf.used);

  ac_free(base);
  return h;
}

ac_in_base_t *ac_in_base_init_gz(const char *filename, int fd, bool can_close,
                                 size_t buffer_size) {
  gzFile gz = NULL;
  if (fd == -1)
    gz = gzopen(filename, "rb");
  else
    gz = gzdopen(fd, "rb");

  if (!gz)
    return NULL;

  if (buffer_size < 32000)
    buffer_size = 32000;

  size_t filename_length = filename ? strlen(filename) + 1 : 0;

  ac_in_base_t *h = (ac_in_base_t *)ac_malloc(
      sizeof(ac_in_base_t) + buffer_size + 1 + filename_length);
  memset(h, 0, sizeof(*h));
  h->buf.buffer = (char *)(h + 1);
  h->buf.size = buffer_size;
  if (filename_length) {
    h->filename = h->buf.buffer + buffer_size + 1;
    strcpy(h->filename, filename);
  }
  h->fd = -1;
  h->gz = gz;
  h->can_close = can_close;
  fill_blocks(h, &(h->buf));
  return h;
}

ac_in_base_t *ac_in_base_init(const char *filename, int fd, bool can_close,
                              size_t buffer_size) {
  if (fd == -1)
    fd = open(filename, O_RDONLY);
  if (fd == -1)
    return NULL;

  if (buffer_size < 256)
    buffer_size = 256;

  size_t filename_length = filename ? strlen(filename) + 1 : 0;
  ac_in_base_t *h = (ac_in_base_t *)ac_malloc(
      sizeof(ac_in_base_t) + buffer_size + 1 + filename_length);
  memset(h, 0, sizeof(*h));
  h->buf.buffer = (char *)(h + 1);
  h->buf.size = buffer_size;
  if (filename_length) {
    h->filename = h->buf.buffer + buffer_size + 1;
    strcpy(h->filename, filename);
  }
  h->fd = fd;
  h->can_close = can_close;
  fill_blocks(h, &(h->buf));
  return h;
}

ac_in_base_t *ac_in_base_init_from_buffer(char *buffer, size_t buffer_size,
                                          bool can_free) {
  ac_in_base_t *h = (ac_in_base_t *)ac_calloc(sizeof(ac_in_base_t));
  h->fd = -1;
  h->buf.buffer = buffer;
  h->buf.size = buffer_size;
  h->buf.used = buffer_size;
  h->buf.eof = true;
  h->buf.can_free = can_free;
  return h;
}

static inline char *end_of_block(ac_in_base_t *h, int32_t *rlen, char *p,
                                 char *ep, bool required) {
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

/*
https://en.wikipedia.org/wiki/Comma-separated_values

RFC 4180 and MIME standards
The 2005 technical standard RFC 4180 formalizes the CSV file format
and defines the MIME type "text/csv" for the handling of text-based
fields. However, the interpretation of the text of each field is still
application-specific. Files that follow the RFC 4180 standard can
simplify CSV exchange and should be widely portable. Among its requirements:

1. MS-DOS-style lines that end with (CR/LF) characters
   (optional for the last line).
2. An optional header record (there is no sure way to detect
   whether it is present, so care is required when importing).
3. Each record should contain the same number of comma-separated
   fields.
4. Any field may be quoted (with double quotes).
5. Fields containing a line-break, double-quote or commas
   should be quoted. (If they are not, the file will likely be
   impossible to process correctly.)
6. If double-quotes are used to enclose fields, then a
   double-quote in a field must be represented by two
   double-quote characters.
*/

char *ac_in_base_read_delimited(ac_in_base_t *h, int32_t *rlen, int delim,
                                bool required) {
  bool csv = false;
  if(delim >= 256) {
    csv = true;
    delim -= 256;
  }

  cleanup_last_read(h);

  *rlen = 0;

  ac_in_buffer_t *b = &(h->buf);
  char *p = b->buffer + b->pos;

  char *sp = p;
  // 2. search for delimiter between pos/used
  char *ep = b->buffer + b->used;
  while (p < ep) {
    if (*p == '\"') {
       if(csv) {
          p++;
        encoded_quote1:
          while(p < ep && *p != '\"')
            p++;
          if(p+1 < ep && p[1] == '\"') {
            p += 2;
            goto encoded_quote1;
          }
      }
      if(p < ep)
        p++;
    }
    else if (*p != delim)
      p++;
    else {
      *rlen = (p - sp);
      b->pos += (*rlen) + 1;
      if (b->pos > b->used)
        abort();

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
    // b->pos = b->used;
    fill_blocks(h, b);
    char *ep = sp + b->used;
    while (p < ep) {
      if (*p == '\"') {
         if(csv) {
            p++;
          encoded_quote2:
            while(p < ep && *p != '\"')
              p++;
            if(p+1 < ep && p[1] == '\"') {
              p += 2;
              goto encoded_quote2;
            }
        }
        if(p < ep)
          p++;
      }
      else if (*p != delim)
        p++;
      else {
        *rlen = (p - sp);
        b->pos += (*rlen) + 1;
        if (b->pos > b->used)
          abort();
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
  // printf("buffer_init(%lu) (2)\n", (b->used * 3) / 2);
  while (1) {
    ac_buffer_append(h->bh, b->buffer, b->used);
    b->used = 0;
    b->pos = 0;
    fill_blocks(h, b);
    p = b->buffer;
    sp = p;
    ep = p + b->used;
    while (p < ep) {
      if (*p == '\"') {
         if(csv) {
            p++;
          encoded_quote3:
            while(p < ep && *p != '\"')
              p++;
            if(p+1 < ep && p[1] == '\"') {
              p += 2;
              goto encoded_quote3;
            }
        }
        if(p < ep)
          p++;
      }
      else if (*p != delim)
        p++;
      else {
        size_t length = (p - sp);
        b->pos += length + 1;
        if (b->pos > b->used)
          abort();
        ac_buffer_append(h->bh, b->buffer, length);
        *rlen = ac_buffer_length(h->bh);
        return ac_buffer_data(h->bh);
      }
    }
    if (b->eof) {
      b->pos = b->used;
      if (required) {
        ac_buffer_destroy(h->bh);
        h->bh = NULL;
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

char *ac_in_base_readz(ac_in_base_t *h, int32_t *rlen, int32_t len) {
  cleanup_last_read(h);

  ac_in_buffer_t *b = &(h->buf);

  *rlen = 0;

  char *p = b->buffer + b->pos;
  if (b->pos + len <= b->used) {
    b->pos += len;
    if (b->pos > b->used)
      abort();
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
    // printf("buffer_init(%u)\n", len);
    ac_buffer_resize(h->bh, len);
    ac_in_buffer_t tmp;
    tmp.buffer = ac_buffer_data(h->bh);
    tmp.size = len;
    tmp.used = b->used - b->pos;
    tmp.pos = tmp.used;
    tmp.eof = false;
    if (tmp.used)
      memcpy(tmp.buffer, b->buffer + b->pos, tmp.used);
    b->pos = b->used = 0;
    fill_blocks(h, &tmp);
    if (tmp.eof)
      b->eof = true;
    *rlen = tmp.used;
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
    if (b->pos > b->used)
      abort();
    *rlen = len;
    char *ep = b->buffer + len;
    h->zero = *ep;
    h->zerop = ep;
    *ep = 0;
    return b->buffer;
  }
}

char *ac_in_base_read(ac_in_base_t *h, int32_t len) {
  ac_in_buffer_t *b = &(h->buf);

  if (h->zerop) {
    (*h->zerop) = h->zero;
    h->zerop = NULL;
  }

  char *p = b->buffer + b->pos;
  if (b->pos + len <= b->used) {
    b->pos += len;
    if (b->pos > b->used)
      abort();
    return p;
  } else {
    if (b->eof) {
      b->pos = b->used;
      return NULL;
    }
    reset_block(b);
    fill_blocks(h, b);
    if (len > b->used) {
      if(b->eof) {
        b->pos = b->used;
        return NULL;
      }
      abort();
      b->pos = b->used;
      return NULL;
    }
    b->pos = len;
    return b->buffer;
  }
}

void ac_in_base_destroy(ac_in_base_t *h) {
  if (h->bh)
    ac_buffer_destroy(h->bh);
  if (h->buf.can_free)
    ac_free(h->buf.buffer);
  if (h->fd != -1 && h->can_close)
    close(h->fd);
  // TODO: Support can_close properly for gz files
  if (h->gz)
    gzclose(h->gz);
  ac_free(h);
}
