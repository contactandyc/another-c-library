#include "ac_svc.h"

struct svc_request_s;
typedef struct svc_request_s svc_request_t;

struct svc_request_s {
  ac_svc_request_t request;
  uv_buf_t buf;
  uv_tcp_t stream;
  uv_shutdown_t shutdown;
  uv_write_t writer;
  char header[1024];

  uint64_t start;
  bool message_ended;
  uv_buf_t bufs[3];
  enum { OPEN = 0, CLOSING = 1, CLOSED = 2 } state;
};

void write_response(svc_request_t *sr);

void svc_request_on_url(ac_http_t *h) {
  svc_request_t *sr = (svc_request_t *)h->data;
  sr->start = uv_now(&(sr->request.svc->loop));
  if (!sr->request.on_url((ac_svc_request_t *)sr))
    write_response(sr);
  sr->message_ended = true;
}

void svc_request_on_chunk(ac_http_t *h) {
  ac_svc_request_t *sr = (ac_svc_request_t *)h->data;
  sr->on_chunk(sr);
}

void svc_request_on_chunk_encoding(ac_http_t *h) {
  svc_request_t *sr = (svc_request_t *)h->data;
  // do something?
}

void svc_request_on_chunks_complete(ac_http_t *h) {
  svc_request_t *sr = (svc_request_t *)h->data;
  sr->message_ended = true;
  // do something?
}

ac_svc_request_t *ac_svc_request_init(ac_svc_t *s) {
  s->active++;
  if (s->free_list) {
    ac_svc_request_t *r = s->free_list;
    if (r) {
      s->free_list = r->next;
      r->next = NULL;
      s->num_free--;
      svc_request_t *sr = (svc_request_t *)r;
      sr->message_ended = true;
      sr->state = OPEN;
      return (ac_svc_request_t *)r;
    }
  }

  svc_request_t *sr = (svc_request_t *)ac_calloc(sizeof(*sr) + 8192);
  sr->buf.base = (char *)(sr + 1);
  sr->buf.len = 8192;
  sr->stream.data = sr;
  sr->shutdown.data = sr;
  sr->writer.data = sr;
  sr->message_ended = true;
  sr->state = OPEN;

  ac_svc_request_t *request = (ac_svc_request_t *)sr;
  request->http =
      ac_http_service_init(svc_request_on_url, s->request_pool_size);
  ac_http_chunk(request->http, svc_request_on_chunk,
                svc_request_on_chunk_encoding, svc_request_on_chunks_complete);
  request->http->data = request;
  request->svc = s;
  request->next = NULL;
  request->on_url = s->on_url;
  request->on_chunk = s->on_chunk;
  return request;
}

void svc_request_clear(svc_request_t *sr) {
  sr->message_ended = false;
  ac_http_clear(sr->request.http);
}

void svc_request_destroy(svc_request_t *sr) {
  ac_http_destroy(sr->request.http);
  ac_free(sr);
}

static void on_close(uv_handle_t *stream) {
  // printf("on_close\n");
  svc_request_t *sr = (svc_request_t *)stream->data;

  if (sr->state != CLOSED) {
    sr->state = CLOSED;
    svc_request_clear(sr);

    ac_svc_request_t *req = (ac_svc_request_t *)sr;
    req->svc->active--;
    req->next = req->svc->free_list;
    req->svc->free_list = req;
    req->svc->num_free++;
  }
}

static const char access_control_headers_s[] =
    "\nAccess-Control-Allow-Headers: Origin, X-Atmosphere-tracking-id, "
    "X-Atmosphere-Framework, X-Cache-Date, Content-Type, "
    "X-Atmosphere-Transport, *\r\nAccess-Control-Allow-Methods: POST, GET, "
    "OPTIONS, PUT\r\nAccess-Control-Allow-Origin: "
    "*\r\nAccess-Control-Request-Headers: Origin, "
    "X-Atmosphere-tracking-id, X-Atmosphere-Framework, X-Cache-Date, "
    "Content-Type, X-Atmosphere-Transport, *\r\n";

static inline char *fill_default_access_control_headers(char *p) {
  p--;
  uint64_t *sh = (uint64_t *)access_control_headers_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;

  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  return (char *)wp;
}

static char content_type_s[] = "\r\nContent-type: ";

static inline char *fill_content_type(char *p, const char *content_type) {
  p -= 2;
  uint64_t *sh = (uint64_t *)content_type_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  sh = (uint64_t *)content_type;
  size_t len = strlen(content_type);
  content_type += len;
  while (len > 8) {
    *wp++ = *sh++;
    len -= 8;
  }
  p = (char *)wp;
  content_type -= len;
  while (*content_type)
    *p++ = *content_type++;
  *p++ = '\r';
  *p++ = '\n';
  return p;
}

static char anotherclibrary_s[] = "Server: AnotherCLibrary\r\n";

static inline char *fill_anotherclibrary(char *p) {
  uint64_t *sh = (uint64_t *)anotherclibrary_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = (char *)wp;
  *p++ = '\n';
  return p;
}

static inline char *u64_to_str(uint64_t num, char *str) {
  if (!num) {
    *str = '0';
    return str + 1;
  }
  char *p = str;
  while (num) {
    uint64_t digit = num % 10;
    *p++ = digit + '0';
    num /= 10;
  }
  char *ep = p - 1;
  while (str < ep) {
    char tmp = *str;
    *str = *ep;
    *ep = tmp;
    str++;
    ep--;
  }
  return p;
}

static char timing_s[] = "Timing: ";

static inline char *fill_timing(char *p, uint64_t ts) {
  uint64_t *sh = (uint64_t *)timing_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh;
  p = u64_to_str(ts, (char *)wp);
  *p++ = '\r';
  *p++ = '\n';
  return p;
}

static char http_s[] = "HTTP/1.1";

static inline char *fill_status_line(char *p, const char *status_line,
                                     const char *date_line) {
  uint64_t *wp = (uint64_t *)p;
  uint64_t *sh = (uint64_t *)http_s;
  *wp = *sh;
  p += 8;
  *p++ = ' ';
  while (*status_line)
    *p++ = *status_line++;
  *p++ = '\r';
  *p++ = '\n';
  wp = (uint64_t *)p;
  sh = (uint64_t *)date_line;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = (char *)wp;
  return p;
}

static const char content_length_s[] = "Content-length: ";

static inline char *fill_content_length(char *p, uint64_t len) {
  uint64_t *sh = (uint64_t *)content_length_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = u64_to_str(len, (char *)wp);
  *p++ = '\r';
  *p++ = '\n';
  return p;
}

/* The XXXX is chopped off at end */
static const char chunk_encoded_s[] = "Transfer-Encoding: chunked\r\nXXXX";

static inline char *fill_chunk_encoded(char *p) {
  uint64_t *sh = (uint64_t *)chunk_encoded_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  p = (char *)wp;
  return p - 4;
}

static const char keep_alive_s[] = "Connection: keep-alive\r\n";

static inline char *fill_keep_alive(char *p) {
  uint64_t *sh = (uint64_t *)keep_alive_s;
  uint64_t *wp = (uint64_t *)p;
  *wp++ = *sh++;
  *wp++ = *sh++;
  *wp++ = *sh;
  return (char *)wp;
}

// Thre ad-I D: 0 0000 00\r\n
static char date_s[] =
    "Date: Fri, 26 Aug 2011 00:31:53 GMT\r\nThread-ID: 000001\r\n";

static inline char *fill_header(svc_request_t *sr, const char *status_line,
                                uint64_t ts, const char *content_type,
                                int keep_alive) {
  char *p = sr->header;
  p = fill_status_line(p, status_line, sr->request.svc->date);
  p = fill_default_access_control_headers(p);
  p = fill_anotherclibrary(p);
  p = fill_timing(p, ts);
  if (content_type)
    p = fill_content_type(p, content_type);
  if (keep_alive)
    p = fill_keep_alive(p);

  if (sr->request.chunk_encoded)
    p = fill_chunk_encoded(p);
  else
    p = fill_content_length(p, sr->request.output.len);

  *p = 0;
  return p;
}

static void after_write(uv_write_t *req, int status);

void write_response_error(svc_request_t *sr, const char *error) {
  char *p = fill_header(sr, error, uv_now(&(sr->request.svc->loop)) - sr->start,
                        NULL, sr->request.http->keep_alive);
  *p++ = '\r';
  *p++ = '\n';

  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = sr->header;
  bufs[0].len = p - sr->header;
  uv_stream_t *stream = (uv_stream_t *)&sr->stream;
  if (uv_is_writable(stream))
    uv_write(&sr->writer, stream, bufs, 1, after_write);
}

void write_response(svc_request_t *sr) {
  const char *content_type = sr->request.content_type;
  if (!content_type)
    content_type = "text/plain";
  char *p = fill_header(sr, HTTP_STATUS_200,
                        uv_now(&(sr->request.svc->loop)) - sr->start,
                        content_type, sr->request.http->keep_alive);
  *p++ = '\r';
  *p++ = '\n';
  size_t num_bufs = 1;
  uv_buf_t *bufs = &sr->bufs[0];
  bufs[0].base = sr->header;
  bufs[0].len = p - sr->header;
  if (!sr->request.chunk_encoded) {
    bufs[1] = sr->request.output;
    num_bufs++;
  }

  uv_stream_t *stream = (uv_stream_t *)&sr->stream;
  if (uv_is_writable(stream))
    uv_write(&sr->writer, stream, bufs, num_bufs, after_write);
}

static void close_connection(svc_request_t *sr) {
  if (sr->state == OPEN) {
    sr->state = CLOSING;
    uv_close((uv_handle_t *)&sr->stream, on_close);
  }
}

static void after_write(uv_write_t *req, int status) {
  svc_request_t *sr = (svc_request_t *)req->data;
  if (!sr->request.http->keep_alive) {
    close_connection(sr);
  }
}

static void handle_request_error(svc_request_t *sr, const char *error) {
  uv_handle_t *stream = (uv_handle_t *)&sr->stream;
  if (sr->state == OPEN)
    uv_read_stop((uv_stream_t *)stream);

  sr->request.http->keep_alive = false;

  if (sr->request.http->headers) {
    if (sr->state == OPEN)
      write_response_error(sr, error);
  } else
    close_connection(sr);
}

static void on_alloc(uv_handle_t *client, size_t suggested_size,
                     uv_buf_t *buf) {
  // printf("on_alloc\n");
  svc_request_t *sr = (svc_request_t *)client->data;
  if (sr->message_ended)
    svc_request_clear(sr);
  *buf = sr->buf;
}

void on_shutdown(uv_shutdown_t *req, int status) {
  svc_request_t *sr = (svc_request_t *)req->data;
  // printf("on_shutdown: %d, %d\n", sr->request.svc->thread_id, sr->state);
  if (sr->state == OPEN)
    close_connection(sr);
}

static void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  // printf("on_read\n");
  svc_request_t *sr = (svc_request_t *)stream->data;
  if (nread > 0) {
    if (!ac_http_data(sr->request.http, buf->base, nread))
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
  ac_svc_t *svc = (ac_svc_t *)server->data;
  struct sockaddr_in clientaddr;
  socklen_t clientlen;
  while (true) {
    clientlen = sizeof(clientaddr);
    int fd = accept(svc->fd, (struct sockaddr *)&clientaddr, &clientlen);
    if (fd < 0)
      break;

    svc_request_t *request = (svc_request_t *)ac_svc_request_init(svc);
    uv_tcp_init(&svc->loop, &request->stream);
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
  ac_svc_t *w = (ac_svc_t *)handle->data;
  if (w->parent->done && !w->active) {
    printf("Thread %d done!\n", w->thread_id);
    uv_poll_stop(&w->server);
    uv_timer_stop(handle);
  }
  time_t t = time(NULL); // now;
  struct tm tm;
  gmtime_r(&t, &tm);
  if (strftime(w->date + 6, 30, "%a, %d %b %Y %H:%M:%S", &tm) != 0)
    w->date[31] = ' ';
}

void *run_loop(void *arg) {
  ac_svc_t *w = (ac_svc_t *)arg;

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

int default_on_url(ac_svc_request_t *h) {
  static const char standard_headers[] =
      "Access-Control-Allow-Headers: Origin, X-Atmosphere-tracking-id, "
      "X-Atmosphere-Framework, X-Cache-Date, Content-Type, "
      "X-Atmosphere-Transport, *\r\nAccess-Control-Allow-Methods: POST, GET, "
      "OPTIONS, PUT\r\nAccess-Control-Allow-Origin: "
      "*\r\nAccess-Control-Request-Headers: Origin, "
      "*\r\nAccess-Control-Request-Headers: Origin, "
      "X-Atmosphere-tracking-id, X-Atmosphere-Framework, X-Cache-Date, "
      "Content-Type, X-Atmosphere-Transport, *\r\n\r\n";

  h->output.base = (char *)standard_headers;
  h->output.len = sizeof(standard_headers) - 1;
  if (!strcmp(h->http->url.base, "/status")) {
    // printf("on_url: %d %lu %s / %s\n", h->svc->thread_id, h->svc->num_free,
    //       h->http->url.base, h->output.base);
  } else if (!strcmp(h->http->url.base, "/shutdown")) {
    h->svc->parent->done = true;
  }
  // printf("on_url: %d %s / %s\n", h->svc->thread_id, h->http->url.base,
  //       h->output.base);
  return 0;
}

int default_on_chunk(ac_svc_request_t *h) {
  // printf("on_chunk: %d %s\n", h->svc->thread_id, h->url.base);
  return 0;
}

ac_svc_t *ac_svc_init(int fd, ac_svc_f _on_url, ac_svc_f _on_chunk) {
  ac_svc_t *w = (ac_svc_t *)ac_calloc(sizeof(*w));
  w->request_pool_size = 16384;
  w->backlog = 1000;
  w->fd = fd;
  w->num_threads = 1;
  w->thread_id = -1;
  w->on_url = _on_url ? _on_url : default_on_url;
  w->on_chunk = _on_chunk ? _on_chunk : default_on_chunk;
  strcpy(w->date, date_s);
  return w;
}

void ac_svc_clone(ac_svc_t *dest, ac_svc_t *src, int thread_id) {
  dest += thread_id;
  *dest = *src;
  dest->thread_id = thread_id;
  dest->parent = src;
  sprintf(dest->date + 48, "%06d\r\n", thread_id);
}

ac_svc_t *ac_svc_port_init(int port, ac_svc_f on_url, ac_svc_f on_chunk) {
  int fd = get_port_fd(port, 1);
  if (fd == -1)
    return NULL;
  ac_svc_t *s = ac_svc_init(fd, on_url, on_chunk);
  s->socket_based = true;
  s->base.port = port;
  return s;
}

ac_svc_t *ac_svc_unix_domain_init(const char *path, ac_svc_f on_url,
                                  ac_svc_f on_chunk) {
  int fd = get_path_fd(path);
  if (fd == -1)
    return NULL;
  ac_svc_t *s = ac_svc_init(fd, on_url, on_chunk);
  s->socket_based = false;
  s->base.path = ac_strdup(path);
  return s;
}

void ac_svc_request_pool_size(ac_svc_t *w, size_t size) {
  if (size < 64)
    size = 64;
  if (w)
    w->request_pool_size = size;
}

void ac_svc_threads(ac_svc_t *w, int num_threads) {
  if (num_threads < 1)
    num_threads = 1;
  if (w)
    w->num_threads = num_threads;
}

void ac_svc_backlog(ac_svc_t *w, int backlog) {
  if (backlog < 0)
    backlog = 0;
  if (w)
    w->backlog = backlog;
}

void ac_svc_run(ac_svc_t *w) {
  if (!w)
    return;
  w->svcs = (ac_svc_t *)ac_malloc(sizeof(*w) * (w->num_threads));
  if (w->num_threads == 1) {
    ac_svc_clone(w->svcs, w, 0);
    run_loop(w->svcs + 0);
  } else {
    for (int i = 0; i < w->num_threads; i++) {
      ac_svc_clone(w->svcs, w, i);
      pthread_create(&(w->svcs[i].thread), NULL, run_loop, w->svcs + i);
    }
    for (int i = 0; i < w->num_threads; i++)
      pthread_join(w->svcs[i].thread, NULL);
  }
}

void ac_svc_destroy(ac_svc_t *w) {
  if (!w)
    return;

  for (int i = 0; i < w->num_threads; i++) {
    ac_svc_request_t *r = w->svcs[i].free_list;
    while (r) {
      ac_svc_request_t *next = r->next;
      svc_request_destroy((svc_request_t *)r);
      r = next;
    }
  }

  if (w->svcs)
    ac_free(w->svcs);
  if (!w->socket_based)
    ac_free(w->base.path);
  ac_free(w);
}
