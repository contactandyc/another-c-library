#include "ac_in.h"
#include "ac_out.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("%s <input> [output]\n", argv[0]);
    return -1;
  }

  ac_in_options_t in_opts;
  ac_in_options_init(&in_opts);
  ac_in_options_format(&in_opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_delimiter('\n'));

  ac_in_t *in = ac_in_init(argv[1], &in_opts);
  ac_out_t *out;
  if (argc > 2)
    out = ac_out_init(argv[2], &out_opts);
  else
    out = ac_out_init_with_fd(1, false, &out_opts);

  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL)
    ac_out_write_record(out, r->record, r->length);

  ac_out_destroy(out);
  ac_in_destroy(in);
  return 0;
}
