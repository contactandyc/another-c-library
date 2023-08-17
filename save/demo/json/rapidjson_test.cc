#include "ac_allocator.h"
#include "ac_io.h"
#include "ac_pool.h"
#include "ac_timer.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

bool do_test(size_t repeat, const char *filename);

size_t parse_total_bytes = 0;
size_t dump_total_bytes = 0;
double parse_total_seconds = 0.0;
double dump_total_seconds = 0.0;

int main(int argc, char *argv[]) {
  setlocale(LC_NUMERIC, "");
  size_t repeat = 1;
  if (argc < 2 || sscanf(argv[1], "%lu", &repeat) != 1)
    repeat = 1;
  for (int i = 2; i < argc; i++)
    do_test(repeat, argv[i]);

  double speed =
      ((parse_total_bytes * 1.0) / parse_total_seconds) / (1024.0 * 1024.0);
  double speed2 = ((dump_total_bytes * 1.0) / dump_total_seconds) / (1024.0 * 1024.0);
  printf("\taParse Speed (MB/sec)\tzoverall\trapidjson\t%'0.0f\n", speed);
  printf("\tbDump Speed (MB/sec)\tzoverall\trapidjson\t%'0.0f\n", speed2);
  // printf("overall\t%0.3fMB/sec\t%0.3fMB/sec\n", speed, speed2);
  return 0;
}


bool do_test(size_t repeat, const char *filename) {
  size_t length = 0;
  char *json = ac_io_read_file(&length, filename);
  if (!json)
    return false;

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
  for (size_t i = 0; i < repeat; i++) {
    ac_pool_clear(pool);
    char *s = (char *)ac_pool_dup(pool, json, length);
    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    if (document.ParseInsitu(s).HasParseError())
      abort();
  }
  ac_timer_stop(t2);
  double seconds = ac_timer_sec(t2);
  double bytes_per_second = bytes;
  bytes_per_second = bytes_per_second / seconds;

  parse_total_seconds = parse_total_seconds + seconds;
  parse_total_bytes = parse_total_bytes + bytes;

  if (repeat > 1) {
    printf("\taParse Speed (MB/sec)\ta%s\trapidjson\t%'0.0f\n", filename, bytes_per_second / (1024.0*1024.0));
    printf("-\tcms per parse\ta%s\trapidjson\t%'0.3f\n", filename, ac_timer_ms(t2));
  }

  ac_timer_destroy(t1);
  ac_timer_destroy(t2);

  ac_pool_clear(pool);
  char *s = (char *)ac_pool_dup(pool, json, length);
  Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
  if (document.ParseInsitu(s).HasParseError())
    abort();

  t1 = ac_timer_init(repeat);
  ac_timer_start(t1);
  for (size_t i = 0; i < repeat; i++) {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
  }
  ac_timer_stop(t1);
  seconds = ac_timer_sec(t1);
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  document.Accept(writer);
  bytes_per_second = buffer.GetSize();
  bytes_per_second = bytes_per_second / seconds;

  dump_total_seconds = dump_total_seconds + seconds;
  dump_total_bytes = dump_total_bytes + buffer.GetSize();

  if (repeat > 1) {
    printf("\tbDump Speed (MB/sec)\ta%s\trapidjson\t%'0.0f\n", filename, bytes_per_second / (1024.0*1024.0));
    printf("-\tdms per dump\ta%s\trapidjson\t%'0.3f\n", filename, ac_timer_ms(t1));
  }
  ac_timer_destroy(t1);

  if (repeat == 1) {
    printf("%s\n", buffer.GetString());
  }

  /*
  size_t bytes = strlen(json);
  if (repeat != 1)
    printf("bytes: %lu\n", repeat * bytes);

  for (size_t i = 0; i < repeat; i++) {
    ac_pool_clear(pool);
    char *s = (char *)ac_pool_dup(pool, json, length);
    Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
    if (document.ParseInsitu(s).HasParseError())
      return 1;
  }
  */
  ac_pool_destroy(pool);
  ac_free(json);
  return 0;
}
