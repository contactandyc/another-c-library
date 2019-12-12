/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "ac_buffer.h"
#include "ac_conv.h"
#include "ac_io.h"
#include "ac_json.h"
#include "ac_pool.h"
#include "ac_timer.h"

int main(int argc, char *argv[]) {
  size_t repeat = 1;
  if (argc <= 2 || sscanf(argv[2], "%lu", &repeat) != 1)
    repeat = 1;

  size_t length = 0;
  char *json = ac_io_read_file(&length, argv[1]);
  if (!json)
    return -1;

  ac_pool_t *pool = ac_pool_init(20000000);

  size_t bytes = strlen(json);
  // if (repeat != 1)
  //  printf("bytes: %lu\n", repeat * bytes);
  ac_timer_t *t1 = ac_timer_init(repeat);
  ac_timer_start(t1);
  for (size_t i = 0; i < repeat; i++) {
    ac_pool_clear(pool);
    ac_pool_dup(pool, json, length);
  }
  ac_timer_stop(t1);
  ac_timer_t *t2 = ac_timer_init(repeat);
  ac_timer_subtract(t2, t1);
  ac_timer_start(t2);
  ac_json_t *j;
  for (size_t i = 0; i < repeat; i++) {
    ac_pool_clear(pool);
    char *s = ac_pool_dup(pool, json, length);
    j = ac_json_parse(pool, s, s + bytes);
    if (ac_json_is_error(j)) {
      ac_json_dump_error(stdout, j);
      abort();
    }
  }
  ac_timer_stop(t2);
  double seconds = ac_timer_sec(t2);
  double bytes_per_second = bytes;
  bytes_per_second = bytes_per_second / seconds;
  if (repeat > 1)
    printf("json_parse: %0.0f MB/sec, %0.3fms\n",
           bytes_per_second / (1024.0 * 1024.0), ac_timer_ms(t2));

  ac_timer_destroy(t1);
  ac_timer_destroy(t2);

  t1 = ac_timer_init(repeat);
  ac_timer_start(t1);
  ac_buffer_t *bh = ac_buffer_init(3000000);
  for (size_t i = 0; i < repeat; i++) {
    ac_buffer_clear(bh);
    ac_json_dump_to_buffer(bh, j);
  }
  ac_timer_stop(t1);
  seconds = ac_timer_sec(t1);
  bytes_per_second = ac_buffer_length(bh);
  bytes_per_second = bytes_per_second / seconds;
  if (repeat > 1)
    printf("json_dump: %0.0f MB/sec, %0.3fms\n",
           bytes_per_second / (1024.0 * 1024.0), ac_timer_ms(t1));
  ac_timer_destroy(t1);

  if (repeat == 1) {
    printf("%s\n", ac_buffer_data(bh));
  }
  ac_buffer_destroy(bh);
  ac_pool_destroy(pool);
  ac_free(json);
  return 0;
}
