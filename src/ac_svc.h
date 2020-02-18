#ifndef ac_svc_H
#define ac_svc_H

#include "ac_http.h"
#include "ac_pool.h"

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <uv.h>

struct ac_svc_request_s;
typedef struct ac_svc_request_s ac_svc_request_t;

struct ac_svc_s;
typedef struct ac_svc_s ac_svc_t;

typedef int (*ac_svc_f)(ac_svc_request_t *r);

ac_svc_t *ac_svc_init(int fd, ac_svc_f on_url, ac_svc_f on_chunk);
ac_svc_t *ac_svc_port_init(int port, ac_svc_f on_url, ac_svc_f on_chunk);
ac_svc_t *ac_svc_unix_domain_init(const char *path, ac_svc_f on_url,
                                  ac_svc_f on_chunk);

void ac_svc_request_pool_size(ac_svc_t *w, size_t size);
void ac_svc_backlog(ac_svc_t *w, int backlog);
void ac_svc_threads(ac_svc_t *w, int num_threads);

void ac_svc_run(ac_svc_t *w);

void ac_svc_destroy(ac_svc_t *w);

struct ac_svc_request_s {
  ac_http_t *http;

  ac_svc_f on_url;
  ac_svc_f on_chunk;

  /* output parameters
      - status_string = NULL defaults to 200 OK
      - content_type = NULL defaults to text/plain
      - output = NULL, len output content
      - chunk_encoded = false, true if chunked output
  */
  char *status_string;
  char *content_type;
  uv_buf_t output;
  bool chunk_encoded;

  enum { OK = 0, SIZE_EXCEEDED = 1, BAD_REQUEST = 2, INTERNAL_ERROR = 3 } state;

  ac_svc_t *svc;
  ac_svc_request_t *next;
};

struct ac_svc_s {
  uv_loop_t loop;
  uv_poll_t server;
  uv_timer_t timer;

  ac_svc_f on_url;
  ac_svc_f on_chunk;

  int fd;
  int thread_id;
  int num_threads;
  int backlog;
  size_t request_pool_size;
  pthread_t thread;
  ac_svc_t *svcs;
  ac_svc_t *parent;
  bool done;

  /* Date: ... GMT\r\nThread-Id: 000001\r\n - 56 bytes */
  char date[64];

  size_t active;
  size_t num_free;
  ac_svc_request_t *free_list;

  bool socket_based;
  union {
    int port;
    char *path;
  } base;
};

#endif
