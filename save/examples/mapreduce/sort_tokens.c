#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
#include "ac_out.h"
#include "ac_pool.h"

#include <locale.h>
#include <stdio.h>

bool file_ok(const char *filename, void *arg) {
  char **extensions = (char **)arg;
  char **p = extensions;
  while (*p) {
    if (ac_io_extension(filename, *p))
      return true;
    p++;
  }
  return false;
}

int usage(const char *prog) {
  printf("%s <path> <extensions>\n", prog);
  printf("extensions - a comma delimited list of valid extensions\n");
  printf("\n");
  return 0;
}

void lowercase(char *s) {
  while (*s) {
    if (*s >= 'A' && *s <= 'Z')
      *s = *s - 'A' + 'a';
    s++;
  }
}

#define TO_SPLIT_ON "(*\"\',+-/\\| \t{});[].=&%<>!#`:"

void lowercase_tokenize_and_sort_tokens_by_token(ac_in_t *in, ac_out_t *out) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_buffer_t *bh = ac_buffer_init(1000);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    uint32_t one = 1;
    for (size_t i = 0; i < num_tokens; i++) {
      ac_buffer_set(bh, &one, sizeof(one));
      ac_buffer_appends(bh, tokens[i]);
      ac_out_write_record(out, ac_buffer_data(bh), ac_buffer_length(bh));
    }
  }
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
}

int compare_tokens(const ac_io_record_t *r1, const ac_io_record_t *r2,
                   void *arg) {
  char *a = r1->record + sizeof(uint32_t);
  char *b = r2->record + sizeof(uint32_t);
  return strcmp(a, b);
}

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");

  if (argc < 3)
    return usage(argv[0]);

  const char *path = argv[1];
  const char *ext = argv[2];

  char **extensions = ac_split(NULL, ',', ext);

  size_t num_files = 0;
  ac_io_file_info_t *files = ac_io_list(path, &num_files, file_ok, extensions);

  ac_in_options_t opts;
  ac_in_options_init(&opts);
  ac_in_options_format(&opts, ac_io_delimiter('\n'));

  ac_out_options_t out_opts;
  ac_out_options_init(&out_opts);
  ac_out_options_format(&out_opts, ac_io_prefix());

  ac_out_ext_options_t out_ext_opts;
  ac_out_ext_options_init(&out_ext_opts);
  ac_out_ext_options_compare(&out_ext_opts, compare_tokens, NULL);

  ac_out_t *out = ac_out_ext_init("sorted_tokens", &out_opts, &out_ext_opts);
  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  lowercase_tokenize_and_sort_tokens_by_token(in, out);
  ac_in_destroy(in);
  ac_out_destroy(out);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
