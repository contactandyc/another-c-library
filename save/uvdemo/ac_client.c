#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define DEFAULT_PORT 80
#define BUFFER_SIZE 1024

typedef struct {
    uv_tcp_t tcp_handle;
    uv_connect_t connect_request;
    uv_buf_t read_buffer;
} http_client_t;

void on_connect(uv_connect_t* connect_request, int status);
void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buffer);
void on_write(uv_write_t* write_request, int status);
void on_close(uv_handle_t* handle);

int main() {
    uv_loop_t* loop = uv_default_loop();

    const char* server_ip = "example.com";
    const char* request_path = "/";

    // Resolve the server IP address
    struct sockaddr_in server_addr;
    uv_ip4_addr(server_ip, DEFAULT_PORT, &server_addr);

    // Create the TCP socket
    http_client_t* client = (http_client_t*)malloc(sizeof(http_client_t));
    uv_tcp_init(loop, &client->tcp_handle);

    // Connect to the server
    uv_tcp_connect(&client->connect_request, &client->tcp_handle, (const struct sockaddr*)&server_addr, on_connect);

    // Initialize the read buffer
    char buffer[BUFFER_SIZE];
    client->read_buffer = uv_buf_init(buffer, BUFFER_SIZE);

    // Start the event loop
    uv_run(loop, UV_RUN_DEFAULT);

    // Clean up resources
    uv_loop_close(loop);
    free(client);

    return 0;
}

void on_connect(uv_connect_t* connect_request, int status) {
    if (status < 0) {
        fprintf(stderr, "Connection error: %s\n", uv_strerror(status));
        uv_close((uv_handle_t*)connect_request->handle, on_close);
        return;
    }

    http_client_t* client = (http_client_t*)connect_request->handle->data;

    // Send an HTTP GET request
    const char* request = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    uv_write_t write_request;
    uv_buf_t write_buffer = uv_buf_init((char*)request, strlen(request));
    uv_write(&write_request, (uv_stream_t*)&client->tcp_handle, &write_buffer, 1, on_write);

    // Start reading the server response
    uv_read_start((uv_stream_t*)&client->tcp_handle, (uv_alloc_cb)uv_buf_init, on_read);
}

void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buffer) {
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
        // Process and print the received data
        printf("%.*s", (int)nread, buffer->base);
    }

    // Clean up the read buffer
    free(buffer->base);
}

void on_write(uv_write_t* write_request, int status) {
    if (status < 0) {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    }
    free(write_request);
}

void on_close(uv_handle_t* handle) {
    free(handle);
}
