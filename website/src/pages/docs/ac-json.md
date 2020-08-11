---
title: ac_json
description:
---

```c
#include "ac_json.h"
```

ac_json provides a json parser, methods for modifying a json DOM, and methods for dumping json to an ac_buffer (or to a file). Once a DOM is constructed, there are several ways to access fields. The parser is efficient for several reasons. One is that it assumes that keys are not encoded. If keys are encoded, they will be remain encoded when a match is attempted. The second is that the DOM is optimized as needed as it is accessed. In many cases, the DOM will not need to be later optimized. There is plans to add additional parsing methods to properly handle encoded keys and pre-optimize the DOM (this really isn't useful except if the DOM is to be accessed from multiple threads).

The json DOM consists of ac_json_t which each have a type (see ac_json_type). If the type is not an object or array, the result will be a string (char *) and a length (size_t). 

ac_jsond will return a decoded string from the ac_json_t node. ac_jsonv will return the original encoded string. ac_jsonb will return the original encoded string and a length. In order to further convert these strings, the ac_conv package should be used.

If the ac_json_t is an array or object, there is an API for accessing and updating members as described below.

The first is the scan api which is the most efficient if only 1-3 fields are accessed within a particular subtree. If the scan api is used and there isn't modification, the DOM would remain thread-safe. 

The scan api consists of ac_jsona_scan, ac_jsona_first, ac_jsona_last, ac_jsona_next, and ac_jsona_previous for arrays and ac_jsono_scan, ac_jsono_scanr, ac_jsono_first, ac_jsono_last, ac_jsono_next, and ac_jsono_previous for objects. ac_jsona_scan and ac_jsono_scan internally just scan until the desired element is found or return NULL otherwise. The scan api leaves nodes in the order in which they were parsed. Calling ac_jsono_next on a node will always return the next node that was found in the source or that was added (and not necessarily the next alphabetical node).

For arrays, a direct access table can be setup by using ac_jsona_nth or ac_jsona_nth_node. This becomes more efficient if a large number of items is in the array or if frequent random accesses occur. If an item is appended, inserted, or erased from the array, the direct access table is destroyed. The ac_jsona_nth and ac_jsona_nth_node should generally be used when when the array isn't changing and when random accesses are required. 

For objects, a read only sorted array of objects is constructed when ac_jsono_get or ac_jsono_get_node. This will sort the nodes by key using ac_sort (which is O(n) time if keys are in order). If items are inserted or erased, the sorted array will be discarded.

Finally, for objects, there is a ac_jsono_find/ac_jsono_insert api which constructs a ac_map (rb tree). This is useful when there is a need to find and insert nodes.

The most efficient method for constructing a DOM is to use the ac_jsona_append and ac_jsono_append methods. The append methods will not do any lookup to see if the items already existed, so care must be taken. However, it is common to know the list of fields that are needed and this can construct them very efficiently. The append methods will not insert items into the rb tree if setup and will not be found using the get api (as the sorted set remains fixed). If there aren't too many fields being added, consider using the scanr api (scan in reverse). If the DOM is constructed such that the fields are alphabetical, then the get API will be more efficient when the json is later consumed.

The ac_json code requires the files from the ac_buffer, ac_pool, ac_allocator, ac_map, ac_search, and ac_sort (or you can just use the whole library).

## ac_json_parse

```c
ac_json_t *ac_json_parse(ac_pool_t *pool, char *p, char *ep);
```
ac_json_parse is the core function for parsing json. This parser is not fully compliant in that keys are expected to not include encodings (or if they do, then you must encode the keys in the same way to match). ac_json_is_error should be called after a parse to handle json errors. If ac_json_is_error returns true, then the json object was invalid and ac_json_dump_error or ac_json_dump_error_to_buffer will output the error. The results of the json parse are cleaned up when the pool is cleared or destroyed.

```c
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
```

## ac_json_is_error

```c
bool ac_json_is_error(ac_json_t *j);
```
ac_json_is_error will return true if the parse fails. If this happens, you can dump it to the screen or to a buffer using ac_json_dump_error or ac_json_dump_error_to_buffer. 

See ac_json_dump_error for an example.

## ac_json_dump_error

```c
void ac_json_dump_error(FILE *out, ac_json_t *j);
```
ac_json_dump_error dumps the error to the FILE handle. This is typically used for debugging and out will be set to stderr or stdout. 

```c
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
    ac_json_dump_error(stdout, j);
    ac_pool_destroy(pool);
    return -1;
  }
  printf( "JSON okay\n")
  ac_pool_destroy(pool);
  return 0;
}
```

## ac_json_dump_error_to_buffer

```c
void ac_json_dump_error_to_buffer(ac_buffer_t *bh, ac_json_t *j);
```
ac_json_dump_error_to_buffer dumps the error to an ac_buffer.

```c
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
    printf( "%s\n", ac_buffer_data(bh));
    ac_pool_destroy(pool);
    return -1;
  }
  printf( "JSON okay\n")
  ac_pool_destroy(pool);
  return 0;
}
```

<NextPrev prev="ac_in" prevUrl="/docs/ac-in" next="ac_map_reduce Part 1" nextUrl="/docs/ac-map-reduce" />
