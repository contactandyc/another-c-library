#include "ac_svc.h"

int on_url(ac_svc_request_t *h) {
  static char hello_world_s[] = "Hello Service\n";
  h->output.base = (char *)hello_world_s;
  h->output.len = sizeof(hello_world_s) - 1;
  return 0;
}

int main(int argc, char *argv[]) {
  ac_svc_t *s = ac_svc_port_init(7000, on_url, NULL);
  ac_svc_threads(s, 10);
  ac_svc_backlog(s, 10000);
  ac_svc_run(s);
  ac_svc_destroy(s);
  return 0;
}
