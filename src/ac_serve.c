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

#include "ac_serve.h"
#include "ac_timer.h"
#include <pthread.h>

struct serve_request_s;
typedef struct serve_request_s serve_request_t;

#define AC_SERVE_OPEN 0
#define AC_SERVE_CLOSING 1
#define AC_SERVE_CLOSED 2

struct serve_request_s {
  ac_serve_request_t request;

  uv_buf_t buf;
  uv_tcp_t stream;
  uv_shutdown_t shutdown;
  uv_write_t writer;
  char header[1024];
  int request_state;

  ac_serve_cb after_write;
  ac_serve_cb on_url;
  ac_serve_cb on_chunk;

  char chunk_header[24];

  uint64_t request_start_time;
  bool request_completed;
  uv_buf_t bufs[3];
  size_t num_bufs;
};

#include "impl/ac_serve_fill.h"

static void after_chunking(uv_write_t *req, int status);
static void close_connection(serve_request_t *sr);

void serve_request_on_url(ac_http_parser_t *h) {
  serve_request_t *sr = (serve_request_t *)h->data;
  sr->request_start_time = uv_now(&(sr->request.service->loop));
  // construct header and content to respond with
  sr->on_url((ac_serve_request_t *)sr);
}

void serve_request_on_chunk(ac_http_parser_t *h) {
  ac_serve_request_t *r = (ac_serve_request_t *)h->data;
  serve_request_t *sr = (serve_request_t *)r;
  sr->on_chunk(r);
}

void serve_request_on_chunk_encoding(ac_http_parser_t *h) {
}

void serve_request_on_chunks_complete(ac_http_parser_t *h) {
  serve_request_t *sr = (serve_request_t *)h->data;
  sr->request_completed = true;
}

ac_serve_request_t *ac_serve_request_init(ac_serve_t *s) {
  s->active++;
  if (s->free_list) {
    ac_serve_request_t *r = s->free_list;
    if (r) {
      s->free_list = r->next;
      r->next = NULL;
      s->num_free--;
      serve_request_t *sr = (serve_request_t *)r;
      sr->request_completed = true;
      sr->request_state = AC_SERVE_OPEN;
      ac_http_parser_clear(sr->request.http);
      r->on_request_complete = s->on_request_complete;
      // ac_buffer_clear(sr->request.bh);
      return (ac_serve_request_t *)r;
    }
  }

  serve_request_t *sr = (serve_request_t *)ac_calloc(sizeof(*sr) + 8192);
  sr->buf.base = (char *)(sr + 1);
  sr->buf.len = 8192;
  sr->stream.data = sr;
  sr->shutdown.data = sr;
  sr->writer.data = sr;
  sr->request_completed = true;
  sr->request_state = AC_SERVE_OPEN;
  ac_serve_request_t *request = (ac_serve_request_t *)sr;
  request->http =
      ac_http_parser_service_init(serve_request_on_url, s->request_pool_size);
  request->pool = request->http->pool;
  // request->bh = ac_buffer_init(4096);
  request->thread_data = s->create_thread_data ? s->create_thread_data(NULL) : NULL;
  ac_http_parser_chunk(request->http, serve_request_on_chunk,
                serve_request_on_chunk_encoding, serve_request_on_chunks_complete);
  request->http->data = request;
  request->service = s;
  request->next = NULL;
  sr->on_url = s->on_url;
  sr->on_chunk = s->on_chunk;
  request->on_request_complete = s->on_request_complete;
  return request;
}

void serve_request_clear(serve_request_t *sr) {
  sr->request_completed = false;
  ac_http_parser_clear(sr->request.http);
}

void serve_request_destroy(serve_request_t *sr) {
  if (sr->request.service->destroy_thread_data)
    sr->request.service->destroy_thread_data(NULL, sr->request.thread_data);
  // ac_buffer_destroy(sr->request.bh);
  ac_http_parser_destroy(sr->request.http);
  ac_free(sr);
}

static void on_close(uv_handle_t *stream) {
  serve_request_t *sr = (serve_request_t *)stream->data;

  if (sr->request_state != AC_SERVE_CLOSED) {
      sr->request_state = AC_SERVE_CLOSED;
    serve_request_clear(sr);

    ac_serve_request_t *req = (ac_serve_request_t *)sr;
    req->service->active--;
    req->next = req->service->free_list;
    req->service->free_list = req;
    req->service->num_free++;
  }
}

static void after_write(uv_write_t *req, int status);
static void after_last_write(uv_write_t *req, int status);

void write_response_error(serve_request_t *sr, const char *error) {
  char *p = (char *)ac_pool_alloc(sr->request.pool, 1024);
  char *sp = p;

  p = fill_header(p, error, sr->request.service->date, 0,
                  uv_now(&(sr->request.service->loop)) - sr->request_start_time,
                  NULL, sr->request.http->keep_alive, sr->request.service->old_style_cors);
  *p++ = '\r';
  *p++ = '\n';

  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = sp;
  bufs[0].len = p - sp;
  sr->num_bufs = 1;
  if (!sr->request.service->hammer) {
    uv_stream_t *stream = (uv_stream_t *)&sr->stream;
    if (uv_is_writable(stream))
      uv_write(&sr->writer, stream, bufs, 1, after_last_write);
  }
}

/*
    ac_serve_http_200(req, content_type)
    ac_serve_http_200_chunk_encoding(req, on_complete);
    ac_serve_write_chunk(req, data, len, on_complete);
    when a chunk is complete, read next part in on_complete OR
    note that structure is writable when new data arrives.

    ac_serve_write(...);
*/
/*
static inline char *fill_chunk_encoded_header(char *p, const char *status_line,
                                              const char *date,
                                              uint64_t ts, const char *content_type,
                                              int keep_alive, bool old_style) {
*/

void ac_serve_start_chunk_encoding(ac_serve_request_t *r,
                                   ac_pool_t *pool,
                                   const char *content_type,
                                   ac_serve_cb cb) {
  char *p = (char *)ac_pool_alloc(pool, 1024);
  char *sp = p;
  if (!content_type)
    content_type = "text/plain";

  serve_request_t *sr = (serve_request_t*)r;
  p = fill_chunk_encoded_header(p, HTTP_STATUS_200, sr->request.service->date,
                  uv_now(&(r->service->loop)) - sr->request_start_time,
                  content_type, r->http->keep_alive,
                  r->service->old_style_cors);
  *p++ = '\r';
  *p++ = '\n';
  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = sp;
  bufs[0].len = p - sp;
  sr->num_bufs = 1;
  sr->after_write = cb;
  if (!sr->request.service->hammer) {
    uv_stream_t *stream = (uv_stream_t *)&sr->stream;
    if (uv_is_writable(stream))
      uv_write(&sr->writer, stream, bufs, sr->num_bufs, after_write);
  }
}

void ac_serve_chunk(ac_serve_request_t *r,
                    void *body, uint32_t body_length,
                    ac_serve_cb cb) {
  if(!r) return;

  serve_request_t *sr = (serve_request_t*)r;
  snprintf(sr->chunk_header, sizeof(sr->chunk_header), "%x\r\n", body_length);
  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = sr->chunk_header;
  bufs[0].len = strlen(sr->chunk_header);
  bufs[1].base = body;
  bufs[1].len = body_length;
  bufs[2].base = "\r\n";
  bufs[2].len = 2;
  sr->num_bufs = 3;
  sr->after_write = cb;
  if (!sr->request.service->hammer) {
    uv_stream_t *stream = (uv_stream_t *)&sr->stream;
    if (uv_is_writable(stream))
      uv_write(&sr->writer, stream, bufs, sr->num_bufs, after_write);
  }
}

void ac_serve_finish_chunk_encoding(ac_serve_request_t *r, ac_serve_cb cb) {
  if(!r) return;
  serve_request_t *sr = (serve_request_t*)r;
  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = "0\r\n";
  bufs[0].len = 3;
  sr->num_bufs = 1;
  if(cb)
    sr->request.on_request_complete = cb;
  if (!sr->request.service->hammer) {
    uv_stream_t *stream = (uv_stream_t *)&sr->stream;
    if (uv_is_writable(stream))
      uv_write(&sr->writer, stream, bufs, sr->num_bufs, after_last_write);
  }
}

void ac_serve_http_200(ac_serve_request_t *r,
                       ac_pool_t *pool,
                       const char *content_type,
                       void *body, uint64_t body_length) {
  char *p = (char *)ac_pool_alloc(pool, 1024);
  char *sp = p;
  if (!content_type)
    content_type = "text/plain";

  serve_request_t *sr = (serve_request_t*)r;
  p = fill_header(p, HTTP_STATUS_200, sr->request.service->date, body_length,
                  uv_now(&(r->service->loop)) - sr->request_start_time,
                  content_type, r->http->keep_alive,
                  r->service->old_style_cors);
  *p++ = '\r';
  *p++ = '\n';
  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = sp;
  bufs[0].len = p - sp;
  sr->num_bufs = 1;
  if(body_length > 0) {
    bufs[1].base = body;
    bufs[1].len = body_length;
    sr->num_bufs = 2;
  }
  if (!sr->request.service->hammer) {
    uv_stream_t *stream = (uv_stream_t *)&sr->stream;
    if (uv_is_writable(stream))
      uv_write(&sr->writer, stream, bufs, sr->num_bufs, after_last_write);
  }
}

static void close_connection(serve_request_t *sr) {
  if (sr->request_state == AC_SERVE_OPEN) {
    if(sr->request.on_request_complete)
      sr->request.on_request_complete((ac_serve_request_t*)sr);

    sr->request_state = AC_SERVE_CLOSING;
    uv_close((uv_handle_t *)&sr->stream, on_close);
  }
}

static void after_write(uv_write_t *req, int status) {
  serve_request_t *sr = (serve_request_t *)req->data;
  if(sr->after_write)
    sr->after_write((ac_serve_request_t*)sr);
}

static void after_last_write(uv_write_t *req, int status) {
  serve_request_t *sr = (serve_request_t *)req->data;
  // callback?
  if(sr->request.http->keep_alive && sr->request.on_request_complete)
    sr->request.on_request_complete((ac_serve_request_t*)sr);
  sr->request_completed = true;
  if (!sr->request.http->keep_alive) {
    close_connection(sr);
  }
}

static void handle_request_error(serve_request_t *sr, const char *error) {
  uv_handle_t *stream = (uv_handle_t *)&sr->stream;
  if (sr->request_state == AC_SERVE_OPEN)
    uv_read_stop((uv_stream_t *)stream);

  sr->request.http->keep_alive = false;

  if (sr->request.http->headers) {
    if (sr->request_state == AC_SERVE_OPEN)
      write_response_error(sr, error);
  } else
    close_connection(sr);
}

static void on_alloc(uv_handle_t *client, size_t suggested_size,
                     uv_buf_t *buf) {
  // printf("on_alloc\n");
  serve_request_t *sr = (serve_request_t *)client->data;
  if (sr->request_completed)
    serve_request_clear(sr);
  *buf = sr->buf;
}

void on_shutdown(uv_shutdown_t *req, int status) {
  serve_request_t *sr = (serve_request_t *)req->data;
  if (sr->request_state == AC_SERVE_OPEN)
    close_connection(sr);
}

static void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  // printf("on_read\n");
  serve_request_t *sr = (serve_request_t *)stream->data;
  if (nread > 0) {
    if (!ac_http_parser_data(sr->request.http, buf->base, nread))
      handle_request_error(sr, HTTP_STATUS_413);
  } else if (nread == 0) {
      /* no-op - there's no data to be read, but there might be later */
  } else if (nread == UV_ENOBUFS)
    handle_request_error(sr, HTTP_STATUS_413);
  else if (nread == UV_EOF) {
    sr->shutdown.data = sr;
    uv_shutdown(&sr->shutdown, (uv_stream_t *)&sr->stream, on_shutdown);
  } else if (nread == UV_ECONNRESET || nread == UV_ECONNABORTED)
    close_connection(sr);
  else
    handle_request_error(sr, HTTP_STATUS_500);
}

void on_accept(uv_poll_t *server, int status, int events) {
    // printf("on_accept\n");
  ac_serve_t *service = (ac_serve_t *)server->data;
  struct sockaddr_in clientaddr;
  socklen_t clientlen;
  while (true) {
    clientlen = sizeof(clientaddr);
    int fd = accept(service->fd, (struct sockaddr *)&clientaddr, &clientlen);
    if (fd < 0)
      break;

    serve_request_t *request = (serve_request_t *)ac_serve_request_init(service);
    uv_tcp_init(&service->loop, &request->stream);
    uv_tcp_open((uv_tcp_t *)&request->stream, fd);
    request->stream.data = request;
    uv_tcp_keepalive((uv_tcp_t *)&request->stream, 1, 60);
    uv_read_start((uv_stream_t *)&request->stream, on_alloc, on_read);
  }
}

static int get_port_fd(int port, int exit_on_non_bind) {
  int res = -1;
  struct sockaddr_in listen_addr;
  int reuseaddr_on = 1;
  res = socket(AF_INET, SOCK_STREAM, 0);
  if (res < 0)
    abort();
  if (setsockopt(res, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
                 sizeof(reuseaddr_on)) == -1)
    abort();
  memset(&listen_addr, 0, sizeof(listen_addr));
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_addr.s_addr = INADDR_ANY;
  listen_addr.sin_port = htons(port);
  if (bind(res, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
    if (exit_on_non_bind)
      exit(0);
    else
      abort();
  }
  if (listen(res, 65000) < 0)
    abort();
  return res;
}

int get_path_fd(const char *socket_path) {
  struct sockaddr_un addr;
  int fd;

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    abort();
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '@') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path);
  }

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    abort();
  }

  if (listen(fd, 65000) == -1) {
    perror("listen error");
    abort();
  }
  return fd;
}

void timer_cb(uv_timer_t *handle) {
  ac_serve_t *w = (ac_serve_t *)handle->data;
  if (w->parent->shutting_down && !w->active) {
    printf("Thread %d is shutting down!\n", w->thread_id);
    uv_poll_stop(&w->server);
    uv_timer_stop(handle);
  }
  time_t t = time(NULL);  // now;
  struct tm tm;
  gmtime_r(&t, &tm);
  if (strftime(w->date + 6, 30, "%a, %d %b %Y %H:%M:%S", &tm) != 0)
    w->date[31] = ' ';
}

void *run_hammer_loop(void *arg) {
  ac_serve_t *w = (ac_serve_t *)arg;

  uv_loop_init(&w->loop);
  /* The timer loop is used to shutdown the service and to update the date
   string used in the HTTP/1.1 response. */
  uv_timer_init(&w->loop, &w->timer);
  w->timer.data = w;
  uv_timer_start(&w->timer, timer_cb, 0, 500);

  w->server.data = w;

  /* Use the default loop and run until no events are on loop. */
  serve_request_t *request = (serve_request_t *)ac_serve_request_init(w);
  ac_buffer_t *bh = ac_buffer_init(8192);
  ac_pool_t *pool = ac_pool_init(1024);
  request->stream.data = request;
  ac_timer_t *t = ac_timer_init(1);
  ac_timer_start(t);
  while (true) {
    uv_run(&w->loop, UV_RUN_NOWAIT);
    ac_pool_clear(pool);
    ac_buffer_clear(bh);
    if (w->hammer_urls_curp >= w->hammer_urls_ep) {
      w->num_times_to_repeat_hammering_url--;
      if (w->num_times_to_repeat_hammering_url <= 0)
        break;
      w->hammer_urls_curp = w->hammer_urls + w->thread_id;
    }
    if (w->hammer_urls_curp > w->hammer_urls_ep)
      break;
    ac_buffer_setf(bh, "GET %s HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n", *(w->hammer_urls_curp));
    serve_request_clear(request);
    if (!ac_http_parser_data(request->request.http, ac_buffer_data(bh), ac_buffer_length(bh)))
      break;

    w->hammer_urls_curp += w->num_threads;
  }
  ac_timer_stop(t);
  w->time_spent_hammering = ac_timer_sec(t);
  ac_timer_destroy(t);

  ac_pool_destroy(pool);
  ac_buffer_destroy(bh);
  serve_request_destroy(request);
  return NULL;
}

void *run_loop(void *arg) {
  ac_serve_t *w = (ac_serve_t *)arg;
  if (w->hammer)
    return run_hammer_loop(arg);

  uv_loop_init(&w->loop);
  /* The timer loop is used to shutdown the service and to update the date
   string used in the HTTP/1.1 response. */
  uv_timer_init(&w->loop, &w->timer);
  w->timer.data = w;
  uv_timer_start(&w->timer, timer_cb, 0, 500);

  /* Poll the file descriptor for new connections */
  uv_poll_init(&w->loop, &w->server, w->fd);
  w->server.data = w;
  uv_poll_start(&w->server, UV_READABLE, on_accept);

  /* Use the default loop and run until no events are on loop. */
  uv_run(&w->loop, UV_RUN_DEFAULT);

  return NULL;
}

char *ac_serve_uri(ac_serve_request_t *r, ac_pool_t *pool) {
    char *uri = (char *)ac_pool_alloc(pool, r->http->url.len+1);
    memcpy(uri, r->http->url.base, r->http->url.len);
    uri[r->http->url.len] = 0;
    return uri;
}

ac_json_t *ac_serve_parse_body_as_json(ac_serve_request_t *r, ac_pool_t *pool) {
  ac_json_t *json_request = NULL;
  char *body = NULL;
  if(r->http->body.len) {
    body = (char *)ac_pool_alloc(pool, r->http->body.len+1);
    memcpy(body, r->http->body.base, r->http->body.len);
    body[r->http->body.len] = 0;
    ac_json_t *j = ac_json_parse(pool, body, body+r->http->body.len);
    if (!ac_json_is_error(j)) {
      json_request = j;
    }
  }
  return json_request;
}


void ac_serve_http_200_json(ac_serve_request_t *r, ac_pool_t *pool, void *body, uint64_t body_length ) {
       ac_serve_http_200(r, pool, "application/json", body, body_length);
}

int default_on_url(ac_serve_request_t *h) {
  return 0;
}

int default_on_chunk(ac_serve_request_t *h) {
  return 0;
}

void ac_serve_new_cors(ac_serve_t *s) {
  s->old_style_cors = false;
}

ac_serve_t *_ac_serve_init(int fd, ac_serve_cb _on_url, ac_serve_cb _on_chunk) {
  ac_serve_t *w = (ac_serve_t *)ac_calloc(sizeof(*w));
  w->request_pool_size = 16384;
  w->backlog = 1000;
  w->fd = fd;
  w->old_style_cors = true;
  w->num_threads = 1;
  w->thread_id = -1;
  w->on_url = _on_url ? _on_url : default_on_url;
  w->on_chunk = _on_chunk ? _on_chunk : default_on_chunk;
  w->create_thread_data = NULL;
  w->destroy_thread_data = NULL;

  strcpy(w->date, date_s);
  return w;
}

ac_serve_t *ac_serve_init(int fd, ac_serve_cb _on_url, ac_serve_cb _on_chunk) {
    return _ac_serve_init(fd, _on_url, _on_chunk);
}

void ac_serve_thread_data(ac_serve_t *service,
                        ac_serve_create_thread_data_cb create,
                        ac_serve_destroy_thread_data_cb destroy) {
  service->create_thread_data = create;
  service->destroy_thread_data = destroy;
}

void ac_serve_clone(ac_serve_t *dest, ac_serve_t *src, int thread_id) {
  dest += thread_id;
  *dest = *src;
  dest->thread_id = thread_id;
  dest->parent = src;
  if (dest->hammer_urls_curp)
    dest->hammer_urls_curp = dest->hammer_urls + thread_id;
  snprintf(dest->date + 48, 10, "%06d\r\n", thread_id);
}

ac_serve_t *ac_serve_hammer_init(ac_serve_cb on_url, ac_serve_cb on_chunk, char **urls, size_t num_urls, int repeat) {
  ac_serve_t *s = ac_serve_init(0, on_url, on_chunk);
  s->socket_based = false;
  s->base.port = 0;
  s->num_times_to_repeat_hammering_url = repeat;
  s->hammer_urls = urls;
  s->hammer_urls_curp = urls;
  s->hammer_urls_ep = urls + num_urls;
  s->hammer = true;
  return s;
}

ac_serve_t *ac_serve_port_init(int port, ac_serve_cb on_url, ac_serve_cb on_chunk) {
  int fd = get_port_fd(port, 1);
  if (fd == -1)
    return NULL;
  ac_serve_t *s = ac_serve_init(fd, on_url, on_chunk);
  s->socket_based = true;
  s->base.port = port;
  return s;
}

ac_serve_t *ac_serve_unix_domain_init(const char *path, ac_serve_cb on_url,
                                  ac_serve_cb on_chunk) {
  int fd = get_path_fd(path);
  if (fd == -1)
    return NULL;
  ac_serve_t *s = ac_serve_init(fd, on_url, on_chunk);
  s->socket_based = false;
  s->base.path = ac_strdup(path);
  return s;
}

void ac_serve_request_pool_size(ac_serve_t *w, size_t size) {
  if (size < 64)
    size = 64;
  if (w)
    w->request_pool_size = size;
}

void ac_serve_threads(ac_serve_t *w, int num_threads) {
  if (num_threads < 1)
    num_threads = 1;
  if (w)
    w->num_threads = num_threads;
}

void ac_serve_backlog(ac_serve_t *w, int backlog) {
  if (backlog < 0)
    backlog = 0;
  if (w)
    w->backlog = backlog;
}

void ac_serve_run(ac_serve_t *w) {
  if (!w)
    return;
  w->services = (ac_serve_t *)ac_calloc(sizeof(*w) * (w->num_threads));
  double time_spent_hammering = 0.0;
  if (w->num_threads == 1) {
    ac_serve_clone(w->services, w, 0);
    run_loop(w->services + 0);
    time_spent_hammering = w->services[0].time_spent_hammering;
  } else {
    for (int i = 0; i < w->num_threads; i++) {
      ac_serve_clone(w->services, w, i);
      pthread_create(&(w->services[i].thread), NULL, run_loop, w->services + i);
    }
    for (int i = 0; i < w->num_threads; i++) {
      pthread_join(w->services[i].thread, NULL);
      time_spent_hammering += w->services[i].time_spent_hammering;
    }
  }
  if (w->hammer) {
    double num_threads = w->num_threads;
    double total_urls = w->hammer_urls_ep - w->hammer_urls;
    total_urls = total_urls * w->num_times_to_repeat_hammering_url;
    printf("Time Spent: %0.4f second(s) across %d thread(s) / %0.4f total seconds / %0.4f microseconds per query / %0.4f qps\n", time_spent_hammering, w->num_threads, time_spent_hammering / num_threads, ((time_spent_hammering * 1000.0 / num_threads) / total_urls) * 1000.0, 1.0 / ((time_spent_hammering / num_threads) / total_urls));
  }
}

void ac_serve_destroy(ac_serve_t *w) {
  if (!w)
    return;

  for (int i = 0; i < w->num_threads; i++) {
    ac_serve_request_t *r = w->services[i].free_list;
    while (r) {
      ac_serve_request_t *next = r->next;
      serve_request_destroy((serve_request_t *)r);
      r = next;
    }
  }

  if (w->services)
    ac_free(w->services);
  if (!w->socket_based)
    ac_free(w->base.path);
  ac_free(w);
}
