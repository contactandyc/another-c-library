#include "ac_conv.h"
#include "ac_json.h"
#include "ac_pool.h"

#include <stdio.h>
#include <string.h>

const char *json = "{ \"name\" : { \"first\": \"Andy\", \"last\": \"Curtis\"}, "
                   "\"region\": \"Greater Seattle Area\" }";

int main(int argc, char *argv[]) {
  ac_pool_t *pool = ac_pool_init(4096);
  char *json_copy = ac_pool_strdup(pool, json);

  ac_json_t *j = ac_json_parse(pool, json_copy, json_copy + strlen(json_copy));
  if (ac_json_is_error(j)) {
    ac_json_dump_error(stdout, j);
    ac_pool_destroy(pool);
    return -1;
  }

  ac_json_t *name = ac_jsono_scan(j, "name");
  const char *first_name =
      ac_str(ac_jsonv(ac_jsono_scan(name, "first")), "UnknownFirstName");
  const char *last_name =
      ac_str(ac_jsonv(ac_jsono_scan(name, "last")), "UnknownLastName");
  const char *region =
      ac_str(ac_jsonv(ac_jsono_scan(j, "region")), "UnknownRegion");
  const char *state =
      ac_str(ac_jsonv(ac_jsono_scan(j, "state")), "UnknownState");
  printf("%s %s from %s, %s\n", first_name, last_name, region, state);

  ac_pool_destroy(pool);
  return 0;
}
