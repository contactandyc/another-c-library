#include "ac_buffer.h"
#include "ac_json.h"
#include "ac_pool.h"

#include <stdio.h>
#include <string.h>

const char *json = "{ \"name\" : { \"first: \"Andy\", \"last\": \"Curtis\"}, "
                   "\"region\": \"Greater Seattle Area\" }";

int main(int argc, char *argv[]) {
  ac_pool_t *pool = ac_pool_init(4096);
  char *json_copy = ac_pool_strdup(pool, json);

  ac_json_t *j = ac_json_parse(pool, json_copy, json_copy + strlen(json_copy));
  if (ac_json_is_error(j)) {
    ac_buffer_t *bh = ac_buffer_pool_init(pool, 1000);
    ac_json_dump_error_to_buffer(bh, j);
    printf("%s\n", ac_buffer_data(bh));
    ac_pool_destroy(pool);
    return -1;
  }
  printf("JSON okay\n");
  ac_pool_destroy(pool);
  return 0;
}
