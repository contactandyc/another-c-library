#ifndef _ac_string_map_h
#define _ac_string_map_h

#include "ac_pool.h"
#include "ac_buffer.h"

struct ac_string_map_s;
typedef struct ac_string_map_s ac_string_map_t;

/*
      // initialize object
      ac_string_map_t *sm = ac_string_map_init(pool);

      // add strings
      ac_string_map_add(sm, "def");
      ac_string_map_add(sm, "abc");
      ac_string_map_add(sm, "abc");
      ac_string_map_add(sm, "ghi");

      // order strings - writing to buffer
      // buffer will now contain actual strings and can be saved to disk
      ac_string_map_order(sm, bh);

      // find will return the offset into the buffer.  The string_map always puts an
      // empty string at byte 0, so abc will be in bytes 1-4 (extra byte is for zero terminator)
      uint32_t offs = ac_string_map_find(sm, "abc");

      // should return 5 (bytes 5-7 are def and byte 8 is the zero terminator)
      offs = ac_string_map_find("def");

      // should return 9 (bytes 9-11 are def and byte 12 is the zero terminator)
      offs = ac_string_map_find("ghi");

      because this object is created using a pool, it doesn't have a destroy method
*/
ac_string_map_t *ac_string_map_init(ac_pool_t *pool);
void ac_string_map_add(ac_string_map_t *h, const char *s);
void ac_string_map_add_with_payload(ac_string_map_t *h, const char *s, const void *payload, uint32_t len);
void ac_string_map_order(ac_string_map_t *h, ac_buffer_t *bh);
uint32_t ac_string_map_find(ac_string_map_t *h, const char *s);

// optional method to estimate output size
size_t ac_string_map_estimate(ac_string_map_t *h);


#endif
