#include "ac_serve.h"
#include "ac_json.h"
#include "ac_cgi.h"

/*
    - connect to service and wait for response which has unique id
    - connect to service as chunk encoded with unique id in header
    - each request will be handled and then responded to through response socket

    - packets can be forwarded to leader as needed and response could work in same way
    - like a proxy
*/

void * add_chunks(void *arg) {
    ac_serve_request_t *r = (ac_serve_request_t *)arg;
    while(true) {
        ac_serve_chunk(r, "Hello\n", 6 );
        sleep(1);
    }
}

int on_url(ac_serve_request_t *r) {
    char *uri = (char *)ac_pool_alloc(r->pool, r->http->url.len+1);
    memcpy(uri, r->http->url.base, r->http->url.len);
    uri[r->http->url.len] = 0;
    if(!strcmp(uri, "/chunk")) {
        ac_serve_begin_chunking(r);
        pthread_t thread;
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &thread_attr, add_chunks, r);
        pthread_attr_destroy(&thread_attr);
    }
    return 1;
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
    ac_serve_t *svc = ac_serve_port_init(1234, on_url, NULL);
    ac_serve_threads(svc, 10);
    ac_serve_run(svc);
    ac_serve_destroy(svc);
    return 0;
}