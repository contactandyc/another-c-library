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

#ifndef ac_serve_H
#define ac_serve_H

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <uv.h>

#include "another-c-library/ac_buffer.h"
#include "another-c-library/ac_http_parser.h"
#include "another-c-library/ac_pool.h"
#include "another-c-library/ac_json.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ac_serve_request_s;
typedef struct ac_serve_request_s ac_serve_request_t;

struct ac_serve_s;
typedef struct ac_serve_s ac_serve_t;

typedef void *(*ac_serve_create_thread_data_cb)(void *gbl);
typedef void (*ac_serve_destroy_thread_data_cb)(void *gbl, void *tdata);

typedef int (*ac_serve_cb)(ac_serve_request_t *r);

ac_serve_t *ac_serve_init(int fd, ac_serve_cb on_url, ac_serve_cb on_chunk);
void ac_serve_new_cors(ac_serve_t *s);
ac_serve_t *ac_serve_port_init(int port, ac_serve_cb on_url, ac_serve_cb on_chunk);
ac_serve_t *ac_serve_unix_domain_init(const char *path, ac_serve_cb on_url,
                                  ac_serve_cb on_chunk);

/* specify a list of URIs (no host or port) and the number of times to repeat */
ac_serve_t *ac_serve_hammer_init(ac_serve_cb on_url, ac_serve_cb on_chunk, char **urls, size_t num_urls, int repeat);

void ac_serve_thread_data(ac_serve_t *service,
                        ac_serve_create_thread_data_cb create,
                        ac_serve_destroy_thread_data_cb destroy);

void ac_serve_request_pool_size(ac_serve_t *w, size_t size);
void ac_serve_backlog(ac_serve_t *w, int backlog);
void ac_serve_threads(ac_serve_t *w, int num_threads);

void ac_serve_run(ac_serve_t *w);

void ac_serve_destroy(ac_serve_t *w);

char *ac_serve_uri(ac_serve_request_t *r, ac_pool_t *pool);
ac_json_t *ac_serve_parse_body_as_json(ac_serve_request_t *r, ac_pool_t *pool);

void ac_serve_http_200(ac_serve_request_t *r,
                       const char *content_type,
                       void *body, uint64_t body_length);

void ac_serve_start_chunk_encoding(ac_serve_request_t *r,
                                   ac_pool_t *pool,
                                   const char *content_type,
                                   ac_serve_cb cb);

void ac_serve_chunk(ac_serve_request_t *r,
                    void *body, uint32_t body_length,
                    ac_serve_cb cb);

void ac_serve_chunk2(ac_serve_request_t *r,
                     void *body, uint32_t body_length,
                     void *body2, uint32_t body2_length,
                     ac_serve_cb cb);

/* if cb is NULL, default on_request_complete will be called */
void ac_serve_finish_chunk_encoding(ac_serve_request_t *r, ac_serve_cb cb);


struct ac_serve_request_s {
  ac_http_parser_t *http;

  ac_pool_t *pool;
 // ac_buffer_t *bh;
  void *thread_data;
  void *data;

  ac_serve_cb on_request_complete;

  /* output parameters
    - status_string = NULL defaults to 200 OK
    - content_type = NULL defaults to text/plain
    - output = NULL, len output content
  */
  enum { OK = 0,
         SIZE_EXCEEDED = 1,
         BAD_REQUEST = 2,
         INTERNAL_ERROR = 3 } state;

  int fd; // only to be used for reference

  ac_serve_t *service;
  ac_serve_request_t *next;
};

struct ac_serve_s {
  uv_loop_t loop;
  uv_poll_t server;
  uv_timer_t timer;

  ac_serve_cb on_url;
  ac_serve_cb on_chunk;
  ac_serve_cb on_request_complete;

  ac_serve_create_thread_data_cb create_thread_data;
  ac_serve_destroy_thread_data_cb destroy_thread_data;

  int fd;
  int thread_id;
  int chunk_id;
  int num_threads;
  int backlog;
  size_t request_pool_size;
  pthread_t thread;
  ac_serve_t *services;
  ac_serve_t *parent;
  bool shutting_down;

  bool old_style_cors;

  /* Date: ... GMT\r\nThread-Id: 000001\r\n - 56 bytes */
  char date[64];

  size_t active;
  size_t num_free;
  ac_serve_request_t *free_list;

  bool socket_based;
  union {
    int port;
    char *path;
  } base;

  // only used when hammer is true
  bool hammer;
  char **hammer_urls;
  char **hammer_urls_curp;
  char **hammer_urls_ep;
  int num_times_to_repeat_hammering_url;
  double time_spent_hammering;
};

#ifdef __cplusplus
}
#endif

#endif
