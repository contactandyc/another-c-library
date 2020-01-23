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

#include "ac_http.h"

#include "ac_async_buffer.h"
#include "ac_buffer.h"
#include "ac_cgi.h"
#include "ac_conv.h"
#include "ac_pool.h"
#include <ctype.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static const int http_state_reading_headers = 1 << 1;
static const int http_state_reading_whole_body = 1 << 2;
static const int http_state_reading_chunk_size = 1 << 3;
static const int http_state_reading_chunk_data = 1 << 4;
static const int http_state_reading_footers = 1 << 5;
static const int http_state_read_complete = 1 << 6;

static const int max_parser_group = 256;
static bool white_space(int c) { return (c == ' ' || c == '\t'); }

struct ac_http_s {
  ac_http_group_t *group;
  ac_http_t *next;
  ac_async_buffer_t *async_buffer;
  ac_pool_t *pool;
  void *arg;
  char *uri;
  char *method;
  char *protocol;
  char *post_data;
  size_t post_size;
  char **headers;
  uint32_t num_headers;
  struct timeval session_start;
  uint32_t state;
  int pool_member;
  ac_buffer_t *chunk_body_cache;
  ac_cgi_t *query_cgi;
  ac_cgi_t *body_cgi;
};

struct ac_http_group_s {
  ac_http_t *parser_pool;
  uint32_t pool_size;
  pthread_mutex_t lock;
  ac_http_f on_headers;
  ac_http_data_f on_request_end;
  ac_http_f on_parsing_error;
  ac_http_data_f on_body_chunk;
};

ac_http_group_t *ac_http_group_init(ac_http_f on_headers,
                                    ac_http_data_f on_body_chunk,
                                    ac_http_data_f on_request_end,
                                    ac_http_f on_parsing_error) {
  ac_http_group_t *g = (ac_http_group_t *)ac_calloc(sizeof(*g));
  pthread_mutex_init(&g->lock, NULL);
  if (!(g->on_headers = on_headers) || !(g->on_request_end = on_request_end) ||
      !(g->on_parsing_error = on_parsing_error)) {
    fprintf(stderr, "ac_http_group_init() -- handlers must be "
                    "assigned for headers, end and error events\n");
    abort();
  }
  g->on_body_chunk = on_body_chunk;
  return g;
}

void ac_http_group_destroy(ac_http_group_t *g) {
  if (g) {
    uint32_t num_pool_destroyed = 0;
    while (num_pool_destroyed < g->pool_size) {
      pthread_mutex_lock(&g->lock);
      while (g->parser_pool) {
        ac_http_t *pool = g->parser_pool;
        g->parser_pool = pool->next;
        pool->pool_member = 0;
        ac_free(pool);
        num_pool_destroyed++;
      }
      pthread_mutex_unlock(&g->lock);
      if (num_pool_destroyed < g->pool_size) {
        sleep(1);
      }
    }
    ac_free(g);
  }
}

static int parse_request_and_headers(ac_http_t *parser, char *headers,
                                     size_t headers_length) {
  char *end_headers = headers + headers_length;
  char *req_line = headers;
  char *end_req_line;
  if ((headers = (char *)memmem(headers, headers_length, "\r\n", 2))) {
    end_req_line = headers;
    headers += 2;
    parser->num_headers = 2;
    char *p = (char *)memmem(headers, headers_length, "\r\n", 2);
    for (; p; p = (char *)memmem(p + 2, (end_headers - p) - 2, "\r\n", 2),
              parser->num_headers++)
      ;
    parser->headers = (char **)ac_pool_calloc(
        parser->pool, sizeof(char *) * parser->num_headers);
    parser->num_headers = 0;
    p = headers;
    char *ep;
    while (p < end_headers) {
      if (!(ep = (char *)memmem(p, end_headers - p, "\r\n", 2))) {
        ep = end_headers;
      }
      parser->headers[parser->num_headers++] =
          ac_pool_strndup(parser->pool, p, ep - p);
      p = ep + 2;
    }
  } else {
    end_req_line = end_headers;
  }
  req_line = ac_pool_strndup(parser->pool, req_line, end_req_line - req_line);
  char *p = req_line;
  // find beginning of method
  for (; white_space(*p); p++)
    ;
  parser->method = p;
  // find end of method
  for (; *p && !white_space(*p); p++)
    ;
  *p++ = 0;
  // find beginning of uri
  for (; white_space(*p); p++)
    ;
  parser->uri = p;
  // find end of uri
  char *end_space = strrchr(p, ' ');
  char *end_tab = strrchr(p, '\t');
  if (!end_space || (end_tab && (end_tab > end_space)))
    end_space = end_tab;
  if ((p = end_space)) {
    for (; (p > parser->uri && white_space(*p)); p--)
      ;
    if (!white_space(*p)) {
      p++;
    }
    *p++ = 0;
    for (; white_space(*p); p++)
      ;
    parser->protocol = p;
  }
  return (parser->method && parser->uri && parser->protocol);
}

static void on_data(ac_async_buffer_t *br) {
  ac_http_t *p = (ac_http_t *)ac_async_buffer_get_arg(br);
  char *data;
  size_t data_length;
  while (1) {
    data = ac_async_buffer_data(br);
    data_length = ac_async_buffer_data_length(br);
    if (p->state & http_state_reading_headers) {
      // First chunk of all requests.  Should include request and headers.
      if (!parse_request_and_headers(p, data, data_length)) {
        p->state ^= (http_state_reading_headers | http_state_read_complete);
        p->group->on_parsing_error(p);
        return;
      }
      p->state ^= http_state_reading_headers;
      p->group->on_headers(p);
      uint64_t content_length =
          ac_uint64_t(ac_http_param(p, header, "Content-Length", NULL), 0);
      char const *encoding =
          ac_http_param(p, header, "Transfer-Encoding", NULL);
      if (content_length) {
        // TODO read whole body in chunks instead of the complete body.
        // We know the length of the body.
        p->state |= http_state_reading_whole_body;
        if (!ac_async_buffer_advance_bytes(p->async_buffer, content_length,
                                           on_data)) {
          return;
        }
        data = ac_async_buffer_data(br);
        data_length = ac_async_buffer_data_length(br);
      } else if (encoding && !strcasecmp(encoding, "chunked")) {
        // We need to start reading chunked encoding
        p->state |= http_state_reading_chunk_size;
        if (!ac_async_buffer_advance_to_string(p->async_buffer, "\r\n",
                                               on_data)) {
          return;
        }
        data = ac_async_buffer_data(br);
        data_length = ac_async_buffer_data_length(br);
      } else {
        // No body.  We are done.
        p->state |= http_state_read_complete;
        p->group->on_request_end(p, NULL, 0);
        return;
      }
    }
    if (p->state & http_state_reading_whole_body) {
      // Body read finished.  We are done.
      p->post_data = data;
      p->post_size = data_length;
      p->state ^= (http_state_reading_whole_body | http_state_read_complete);
      p->group->on_request_end(p, data, data_length);
      return;
    }
    if (p->state & http_state_reading_chunk_size) {
      // We should have the length of the next chunk.
      // Format: "5E\r\n" or "5E;key=value\r\n"
      char buf[64];
      uint32_t i;
      for (i = 0; i < data_length && i < 63 && isxdigit(data[i]); i++) {
        buf[i] = data[i];
      }
      buf[i] = 0;
      size_t chunk_size;
      if (sscanf(buf, "%zu", &chunk_size) != 1) {
        p->state ^= (http_state_reading_chunk_size | http_state_read_complete);
        p->group->on_parsing_error(p);
        return;
      } else {
        if (chunk_size > 0) {
          // More data coming.
          chunk_size += 2; // \r\n at end of chunks
          p->state ^=
              (http_state_reading_chunk_size | http_state_reading_chunk_data);
          if (!ac_async_buffer_advance_bytes(p->async_buffer, chunk_size,
                                             on_data)) {
            return;
          }
          data = ac_async_buffer_data(br);
          data_length = ac_async_buffer_data_length(br);
        } else {
          // End of body.  Need to get any footers if sent
          p->state ^=
              (http_state_reading_chunk_size | http_state_reading_footers);
          if (!ac_async_buffer_advance_to_string(p->async_buffer, "\r\n",
                                                 on_data)) {
            return;
          }
          data = ac_async_buffer_data(br);
          data_length = ac_async_buffer_data_length(br);
        }
      }
    }
    if (p->state & http_state_reading_chunk_data) {
      // Actual chunk of body.  Not copying this, simply passing it around.
      data_length -= 2; // extra \r\n at end of chunk is ignored
      if (p->group->on_body_chunk) {
        p->group->on_body_chunk(p, data, data_length);
      } else {
        // no handler for chunk encoding.  buffer it up
        if (!p->chunk_body_cache) {
          p->chunk_body_cache = ac_buffer_init(1024 * 4);
        }
        ac_buffer_append(p->chunk_body_cache, data, data_length);
      }
      p->state ^=
          (http_state_reading_chunk_data | http_state_reading_chunk_size);
      if (!ac_async_buffer_advance_to_string(p->async_buffer, "\r\n",
                                             on_data)) {
        return;
      }
      data = ac_async_buffer_data(br);
      data_length = ac_async_buffer_data_length(br);
    }
    while (p->state & http_state_reading_footers) {
      if (data_length > 0) {
        // Footer received.
        if (!ac_async_buffer_advance_to_string(p->async_buffer, "\r\n",
                                               on_data)) {
          return;
        }
        data = ac_async_buffer_data(br);
        data_length = ac_async_buffer_data_length(br);
        continue;
      } else {
        // End of transmission. Done.
        p->state ^= (http_state_reading_footers | http_state_read_complete);
        p->post_data = NULL;
        p->post_size = 0;
        if (p->chunk_body_cache) {
          p->post_data = ac_buffer_data(p->chunk_body_cache);
          p->post_size = ac_buffer_length(p->chunk_body_cache);
        }
        p->group->on_request_end(p, p->post_data, p->post_size);
        return;
      }
    }
  }
}

void ac_http_set_arg(ac_http_t *p, void *arg) {
  if (p)
    p->arg = arg;
}

void *ac_http_get_arg(ac_http_t *p) { return (p) ? p->arg : NULL; }

ac_http_t *ac_http_init(ac_http_group_t *g) {
  ac_http_t *res = NULL;
  if (g) {
    int pool_member = 1;
    pthread_mutex_lock(&g->lock);
    if (g->parser_pool) {
      res = g->parser_pool;
      g->parser_pool = res->next;
    } else {
      pool_member = (g->pool_size + 1 < max_parser_group) ? 1 : 0;
      if (pool_member) {
        g->pool_size++;
      }
    }
    pthread_mutex_unlock(&g->lock);
    if (!res) {
      res = (ac_http_t *)ac_malloc(sizeof(*res));
      res->group = g;
      res->pool_member = pool_member;
      res->pool = ac_pool_init(1024);
      res->async_buffer = ac_async_buffer_init();
      ac_async_buffer_set_arg(res->async_buffer, res);
    }
    res->state = http_state_reading_headers;
    gettimeofday(&res->session_start, NULL);
    ac_async_buffer_advance_to_string(res->async_buffer, "\r\n\r\n", on_data);
  }
  return res;
}

void ac_http_release(ac_http_t *p) {
  if (p) {
    ac_buffer_destroy(p->chunk_body_cache);
    p->chunk_body_cache = NULL;
    if (p->pool_member) {
      ac_async_buffer_clear(p->async_buffer);
      ac_pool_clear(p->pool);
      pthread_mutex_lock(&p->group->lock); // lock
      p->next = p->group->parser_pool;
      p->group->parser_pool = p;
      pthread_mutex_unlock(&p->group->lock); // unlock
    } else {
      ac_async_buffer_destroy(p->async_buffer);
      ac_pool_destroy(p->pool);
      ac_free(p);
    }
  }
}

void ac_http_parse(ac_http_t *p, char const *data, size_t data_length) {
  if (p->state & http_state_read_complete) {
    p->group->on_parsing_error(p);
    return;
  }
  ac_async_buffer_parse(p->async_buffer, data, data_length);
}

char const *ac_http_method(ac_http_t *p) { return p ? p->method : NULL; }

char const *ac_http_uri(ac_http_t *p) { return p ? p->uri : NULL; }

static char *get_header_param(ac_http_t *p, char const *field,
                              size_t field_length) {
  char *res = NULL;
  if (p && field_length) {
    for (uint32_t i = 0; res == NULL && i < p->num_headers; ++i) {
      if (!strncasecmp(p->headers[i], field, field_length)) {
        res = p->headers[i] + field_length;
        while (*res == ' ')
          res++;
        if (*res != ':') {
          res = NULL;
        } else {
          do {
            res++;
          } while (*res == ' ');
          if (*res == 0)
            res = NULL;
        }
      }
    }
  }
  return res;
}

static ac_cgi_t *get_query_cgi(ac_http_t *p) {
  if (!p->query_cgi && p->uri) {
    p->query_cgi = ac_cgi_init(p->pool, p->uri);
  }
  return p->query_cgi;
}

static ac_cgi_t *get_body_cgi(ac_http_t *p) {
  if (!p->body_cgi) {
    char const *content_type = ac_http_param(p, header, "Content-Type", NULL);
    if (p->post_size && content_type &&
        !strncasecmp(content_type, "application/x-www-form-urlencoded",
                     strlen("application/x-www-form-urlencoded"))) {
      p->body_cgi = ac_cgi_init(
          p->pool, ac_pool_strndup(p->pool, p->post_data, p->post_size));
    }
  }
  return p->body_cgi;
}

char const *ac_http_param(ac_http_t *p, ac_http_param_location_t loc,
                          char const *key, char const *default_value) {
  char const *res = default_value;
  if (p && key && key[0]) {
    switch (loc) {
    case header:
      res = get_header_param(p, key, strlen(key));
      break;
    case query:
      res = ac_cgi_str(get_query_cgi(p), key, NULL);
      break;
    case body:
      res = ac_cgi_str(get_body_cgi(p), key, NULL);
      break;
    }
  }
  return (res == NULL) ? default_value : res;
}

char **ac_http_params(ac_http_t *p, ac_http_param_location_t loc,
                      char const *key) {

  static char *empty[1] = {NULL};
  char **res = empty;
  if (p && key && key[0]) {
    switch (loc) {
    case header:
      break;
    case query:
      res = ac_cgi_strs(get_query_cgi(p), key);
      break;
    case body:
      res = ac_cgi_strs(get_body_cgi(p), key);
      break;
    }
  }
  return (res == NULL) ? empty : res;
}
