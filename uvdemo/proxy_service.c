#include "ac_serve.h"
#include "ac_json.h"
#include "ac_cgi.h"

typedef struct socket_pair_s {
    int in_fd;
    int out_fd;
} socket_pair_t;

typedef struct {
    uv_tcp_t tcp_handle;
    uv_connect_t connect_request;
    uv_buf_t read_buffer;
    int request_count;
} http_client_t;


void connect_socket_pair(const char *server_ip, int server_port) {
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send three HTTP GET requests
    char request_template[] = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n";
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int i;

        for (i = 0; i < 100000; i++) {
            sprintf(request, request_template, server_ip);
            if (send(client_socket, request, strlen(request), 0) == -1) {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }

            // Receive response from the server
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received == -1) {
                perror("Receive failed");
                exit(EXIT_FAILURE);
            }
            buffer[bytes_received] = '\0';

            // printf("Response %d:\n%s\n", i + 1, buffer);

            // Delay between requests (optional)
            // sleep(1);
        }
        printf( "%s\n", buffer );
        // Close the socket
        close(client_socket);

}

/* the job of this service is to take pressure off of an actual service. */
int ac_serve_cb(ac_serve_request_t *r) {
    /* request for now will just be requestID\n, lastHB\n, URL\n, body length\n, body\n */


  r->status_string = NULL;
  r->content_type = "application/json";

  char *uri = (char *)ac_pool_alloc(r->pool, r->http->url.len+1);
  memcpy(uri, r->http->url.base, r->http->url.len);
  uri[r->http->url.len] = 0;

  ac_json_t *json_request = NULL;
  char *body = NULL;
  if(r->http->body.len) {
    body = (char *)ac_pool_alloc(r->pool, r->http->body.len+1);
    memcpy(body, r->http->body.base, r->http->body.len);
    body[r->http->body.len] = 0;
    ac_json_t *j = ac_json_parse(r->pool, body, body+r->http->body.len);
    if (!ac_json_is_error(j)) {
      json_request = j;
    }
  }
  ac_json_t *obj = r->service->on_json(r, uri, json_request);
  ac_buffer_clear(r->bh);
  if(obj)
      ac_json_dump_to_buffer(r->bh, obj);
  else
      ac_buffer_sets(r->bh, "{}");

  r->output.base = (char *)ac_buffer_data(r->bh);
  r->output.len = ac_buffer_length(r->bh);
  return 0;
}

}

ac_json_t * on_json(ac_serve_request_t *r, const char *uri, ac_json_t *json_request) {
    if(!json_request)
        json_request = ac_cgi_to_json(r->pool, uri);

    if(!json_request)
        json_request = ac_jsono(r->pool);
    /*
    ac_jsono_append(json_request, "fd", ac_json_number(r->pool, r->service->fd), true);
    ac_jsono_append(json_request, "thread_id", ac_json_number(r->pool, r->service->thread_id), true);
    ac_jsono_append(json_request, "num_active", ac_json_number(r->pool, r->service->active), true);
    ac_jsono_append(json_request, "num_free", ac_json_number(r->pool, r->service->num_free), true);
    ac_jsono_append(json_request, "pool_size", ac_json_number(r->pool, ac_pool_size(r->pool)), true);

    ac_jsono_append(json_request, "status", ac_json_str(r->pool, "OK"), true);
    */
    return json_request;
}


int main( int argc, char *argv[] ) {
    ac_serve_t *svc = ac_serve_port_init(1234, on_json);
    ac_serve_threads(svc, 10);
    ac_serve_run(svc);
    ac_serve_destroy(svc);
    return 0;
}