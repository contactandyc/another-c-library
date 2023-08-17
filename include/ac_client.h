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

#ifndef ac_client_H
#define ac_client_H

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <uv.h>

#include "ac_buffer.h"
#include "ac_http_parser.h"
#include "ac_pool.h"
#include "ac_json.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ac_client_request_s;
typedef struct ac_client_request_s ac_client_request_t;

typedef int (*ac_client_cb)(ac_client_request_t *r);
typedef int (*ac_client_status_cb)(ac_client_request_t *r, int status);

struct ac_client_request_s {
    ac_http_parser_t *http;
    int fd; // only to be used for reference

    ac_pool_t *pool;

    void *data;
    ac_client_cb on_response;
    ac_client_cb on_request;
    ac_client_status_cb on_connect_error;
    ac_client_cb on_chunk_start;
    ac_client_cb on_chunk;
    ac_client_cb on_chunk_end;

    struct sockaddr_in dest;
    uv_tcp_t tcp;
    uv_connect_t connect;
    uv_buf_t read_buf;
    uv_loop_t *loop;
    uv_write_t write_req;
};

ac_client_request_t *ac_client_request_init(int pool_size);

void ac_client_request_destroy(ac_client_request_t* h);

/* connect a client and associate it to a loop */
ac_client_request_t *ac_client_request_connect(ac_client_request_t* r, uv_loop_t *loop, int port);

/* this clears the http parsing, typically is only needed in the on_response handler if another request
   is issued. */
void ac_client_clear(ac_client_request_t* r);

/* trigger a generic request to the already connected client */
void ac_client_make_request(ac_client_request_t* r, const char *request, size_t len);

/* close the connection */
void ac_client_close(ac_client_request_t *r);

#ifdef __cplusplus
}
#endif

#endif
