#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "ac_pool.h"
#include "ac_http_parser.h"

#define DEFAULT_PORT 1234
#define BUFFER_SIZE 1024

typedef struct {
    uv_tcp_t tcp_handle;
    uv_connect_t connect_request;
    uv_buf_t read_buffer;
    uv_buf_t write_buffer;
    uv_write_t write_request;
    ac_http_parser_t *http;
    char *request;
    char buffer[BUFFER_SIZE];
    int request_count;
} http_client_t;

void on_body(ac_http_parser_t *lh) {
    printf( "on_body\n" );
}

void on_chunk(ac_http_parser_t *lh) {
    printf( "on_chunk\n" );
}
void on_chunk_encoding(ac_http_parser_t *lh) {
    printf( "on_chunk_encoding\n" );
}
void on_chunk_complete(ac_http_parser_t *lh) {
    printf( "on_chunk_complete\n" );
}


void on_connect(uv_connect_t* connect_request, int status);
void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buffer);
void on_write(uv_write_t* write_request, int status);
void on_close(uv_handle_t* handle);

http_client_t * do_connect(uv_loop_t* loop, const char *request, const char *server_ip, int server_port) {
    // Resolve the server IP address
    struct sockaddr_in server_addr;
    uv_ip4_addr(server_ip, DEFAULT_PORT, &server_addr);

    ac_http_parser_t *http = ac_http_parser_client_init(on_body, 16384);
    ac_http_parser_chunk(http, on_chunk, on_chunk_encoding, on_chunk_complete);

    // Create the TCP socket
    http_client_t* client = (http_client_t*)ac_pool_alloc(http->pool, sizeof(http_client_t) + strlen(request) + 1);
    strcpy((char *)(client+1), request);
    client->http = http;
    client->write_buffer = uv_buf_init((char*)(client+1), strlen(request));
    client->request_count = 0;
    uv_tcp_init(loop, &client->tcp_handle);
    client->tcp_handle.data = client;
    // Connect to the server
    uv_tcp_connect(&client->connect_request, &client->tcp_handle, (const struct sockaddr*)&server_addr, on_connect);

    // Initialize the read buffer
    client->read_buffer = uv_buf_init(client->buffer, BUFFER_SIZE);
    return client;
}

int main() {
    uv_loop_t* loop = uv_default_loop();
    const char* request = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nXConnection: keep-alive\r\n\r\n";
    http_client_t *client = do_connect(loop, request, "127.0.0.1", 1234);

    // Start the event loop
    uv_run(loop, UV_RUN_DEFAULT);

    // Clean up resources
    uv_loop_close(loop);
    ac_http_parser_t *http = client->http;
    ac_http_parser_destroy(http);

    return 0;
}

static void on_alloc(uv_handle_t *client, size_t suggested_size,
                     uv_buf_t *buf) {
  // printf("on_alloc\n");
  http_client_t *h = (http_client_t *)client->data;
  *buf = h->read_buffer;
}

void on_connect(uv_connect_t* connect_request, int status) {
    printf( "on_connect\n" );

    if (status < 0) {
        fprintf(stderr, "Connection error: %s\n", uv_strerror(status));
        uv_close((uv_handle_t*)connect_request->handle, on_close);
        return;
    }

    http_client_t* client = (http_client_t*)connect_request->handle->data;
    uv_write(&client->write_request, (uv_stream_t*)&client->tcp_handle, &client->write_buffer, 1, on_write);

    // Start reading the server response
    uv_read_start((uv_stream_t*)&client->tcp_handle, (uv_alloc_cb)on_alloc, on_read);
}

void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buffer) {
    printf( "on_read\n" );
    if (nread < 0) {
        if (nread == UV_EOF) {
            // End of file reached (connection closed)
            uv_close((uv_handle_t*)stream, on_close);
        } else {
            fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
        }
        return;
    }

    if (nread > 0) {
        http_client_t* client = (http_client_t*)stream->data;
        client->request_count++;

        // Process and print the received data
        printf("Response %d:\n%.*s\n", client->request_count, (int)nread, buffer->base);
        ac_http_parser_data(client->http, buffer->base, nread);
        // uv_close((uv_handle_t*)stream, on_close);
    }
}

void on_write(uv_write_t* write_request, int status) {
    printf( "on_write\n" );
    if (status < 0) {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    }
    // free(write_request);
}

void on_close(uv_handle_t* handle) {
    printf( "on_close\n" );
    // free(handle);
}
