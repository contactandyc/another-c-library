#ifndef _ac_http_h
#define _ac_http_h

#include "ac_allocator.h"
#include "ac_pool.h"
#include "llhttp/llhttp.h"

#include <stdbool.h>
#include <uv.h>

typedef struct ac_http_header_s {
  uv_buf_t key;
  uv_buf_t value;
  struct ac_http_header_s *next;
} ac_http_header_t;

typedef struct {
  ac_pool_t *pool;

  void *data;

  ac_http_header_t *headers;
  // only for servers
  uv_buf_t url;
  uv_buf_t body;

  size_t content_length;
  unsigned short http_major;
  unsigned short http_minor;

  // only for servers
  unsigned char method;

  // only for clients
  int status_code;

  int keep_alive;
  bool chunked;
} ac_http_t;

typedef void (*on_http_f)(ac_http_t *lh);

/* init the http parser for parsing a service request.  The pool_size
   should be zero for default size or a user specified size. */
ac_http_t *ac_http_service_init(on_http_f on_url, size_t pool_size);

/* init the http parser for parsing a client response.  The pool_size
   should be zero for default size or a user specified size. */
ac_http_t *ac_http_client_init(on_http_f on_body, size_t pool_size);

/* set functions to handle chunk encoding.  on_chunk is typically the only
   function required.  Specify NULL if no callback is specified. */
void ac_http_chunk(ac_http_t *h, on_http_f on_chunk,
                   on_http_f on_chunk_encoding, on_http_f on_chunks_complete);

/* make the http parser reusable */
void ac_http_clear(ac_http_t *h);

/* write data to the http parser */
bool ac_http_data(ac_http_t *h, const void *d, size_t len);

/* destroy the http parser */
void ac_http_destroy(ac_http_t *h);

#endif
