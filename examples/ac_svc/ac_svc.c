#include "ac_svc.h"

int main(int argc, char *argv[]) {
  ac_svc_t *s = ac_svc_port_init(7000, NULL, NULL);
  ac_svc_threads(s, 10);
  ac_svc_backlog(s, 10000);
  ac_svc_run(s);
  ac_svc_destroy(s);
  return 0;
}
