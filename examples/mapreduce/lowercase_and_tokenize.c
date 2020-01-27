#include "ac_allocator.h"
#include "ac_conv.h"
#include "ac_in.h"
#include "ac_io.h"
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

void lowercase_and_tokenize(ac_in_t *in) {
  ac_pool_t *pool = ac_pool_init(4096);
  ac_io_record_t *r;
  while ((r = ac_in_advance(in)) != NULL) {
    ac_pool_clear(pool);
    /* okay to change inline because this will be only use */
    lowercase(r->record);
    size_t num_tokens = 0;
    char **tokens = ac_pool_tokenize(pool, &num_tokens, TO_SPLIT_ON, r->record);
    for (size_t i = 0; i < num_tokens; i++)
      printf("[%lu] %s\n", i, tokens[i]);
  }
  ac_pool_destroy(pool);
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

  ac_in_t *in = ac_in_init_from_list(files, num_files, &opts);
  lowercase_and_tokenize(in);
  ac_in_destroy(in);

  if (extensions)
    ac_free(extensions);
  if (files)
    ac_free(files);
  return 0;
}
