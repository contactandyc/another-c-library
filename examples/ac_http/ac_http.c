#include "ac_http.h"
#include "ac_io.h"

#include <stdio.h>

static void on_url(ac_http_t *h) {
  printf("%s\n%s\n", h->url.base, h->body.base);
}

static void on_body(ac_http_t *h) { printf("%s\n", h->body.base); }

static void on_chunk(ac_http_t *h) { printf("%s\n", h->body.base); }

static void on_chunks_complete(ac_http_t *h) { printf("Chunks Done!\n"); }

int main(int argc, char *argv[]) {
  ac_http_t *lh = ac_http_client_init(NULL, 10000);
  ac_http_chunk(lh, on_chunk, NULL, on_chunks_complete);
  size_t len = 0;
  char *buffer = ac_io_read_file(&len, argv[1]);
  char *d = ac_memdup(buffer, len);
  for (size_t i = 0; i < 10000; i++) {
    ac_http_clear(lh);
    memcpy(buffer, d, len);
    ac_http_data(lh, buffer, len);
  }
  ac_free(d);
  ac_free(buffer);
  ac_http_destroy(lh);

  return 0;
}
