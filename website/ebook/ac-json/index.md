---
path: "/ac-json"
posttype: "docs"
title: "ac_json"
---

```c
#include "ac_json.h"
```

ac\_json provides a json parser, methods for modifying a json DOM, and methods for dumping json to an ac\_buffer (or to a file).  Once a DOM is constructed, there are several ways to access fields.  The parser is efficient for several reasons.  One is that it assumes that keys are not encoded.  If keys are encoded, they will be remain encoded when a match is attempted.  The second is that the DOM is optimized as needed as it is accessed.  In many cases, the DOM will not need to be later optimized.  There is plans to add additional parsing methods to properly handle encoded keys and pre-optimize the DOM (this really isn't useful except if the DOM is to be accessed from multiple threads).

The json DOM consists of ac\_json\_t which each have a type (see ac\_json\_type).  If the type is not an object or array, the result will be a string (char *) and a length (size\_t).  

ac\_jsond will return a decoded string from the ac\_json\_t node.  ac\_jsonv will return the original encoded string.  ac\_jsonb will return the original encoded string and a length.  In order to further convert these strings, the ac\_conv package should be used.

If the ac\_json\_t is an array or object, there is an API for accessing and updating members as described below.

The first is the scan api which is the most efficient if only 1-3 fields are accessed within a particular subtree.  If the scan api is used and there isn't modification, the DOM would remain thread-safe.  

The scan api consists of ac\_jsona\_scan, ac\_jsona\_first, ac\_jsona\_last, ac\_jsona\_next, and ac\_jsona\_previous for arrays and ac\_jsono\_scan, ac\_jsono\_scanr, ac\_jsono\_first, ac\_jsono\_last, ac\_jsono\_next, and ac\_jsono\_previous for objects.  ac\_jsona\_scan and ac\_jsono\_scan internally just scan until the desired element is found or return NULL otherwise.  The scan api leaves nodes in the order in which they were parsed.  Calling ac\_jsono\_next on a node will always return the next node that was found in the source or that was added (and not necessarily the next alphabetical node).

For arrays, a direct access table can be setup by using ac\_jsona\_nth or ac\_jsona\_nth\_node.  This becomes more efficient if a large number of items is in the array or if frequent random accesses occur.  If an item is appended, inserted, or erased from the array, the direct access table is destroyed.  The ac\_jsona\_nth and ac\_jsona\_nth\_node should generally be used when when the array isn't changing and when random accesses are required.  

For objects, a read only sorted array of objects is constructed when ac\_jsono\_get or ac\_jsono\_get\_node.  This will sort the nodes by key using ac\_sort (which is O(n) time if keys are in order).  If items are inserted or erased, the sorted array will be discarded.

Finally, for objects, there is a ac\_jsono\_find/ac\_jsono\_insert api which constructs a ac\_map (rb tree).  This is useful when there is a need to find and insert nodes.

The most efficient method for constructing a DOM is to use the ac\_jsona\_append and ac\_jsono\_append methods.  The append methods will not do any lookup to see if the items already existed, so care must be taken.  However, it is common to know the list of fields that are needed and this can construct them very efficiently.  The append methods will not insert items into the rb tree if setup and will not be found using the get api (as the sorted set remains fixed).  If there aren't too many fields being added, consider using the scanr api (scan in reverse).  If the DOM is constructed such that the fields are alphabetical, then the get API will be more efficient when the json is later consumed.

The ac\_json code requires the files from the ac\_buffer, ac\_pool, ac\_allocator, ac\_map, ac\_search, and ac\_sort (or you can just use the whole library).

## ac\_json\_parse

```c
ac_json_t *ac_json_parse(ac_pool_t *pool, char *p, char *ep);
```
ac\_json\_parse is the core function for parsing json.  This parser is not fully compliant in that keys are expected to not include encodings (or if they do, then you must encode the keys in the same way to match).  ac\_json\_is\_error should be called after a parse to handle json errors.  If ac\_json\_is\_error returns true, then the json object was invalid and ac\_json\_dump\_error or ac\_json\_dump\_error\_to\_buffer will output the error.  The results of the json parse are cleaned up when the pool is cleared or destroyed.

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

## ac\_json\_is\_error

```c
bool ac_json_is_error(ac_json_t *j);
```
ac\_json\_is\_error will return true if the parse fails.  If this happens, you can dump it to the screen or to a buffer using ac\_json\_dump\_error or ac\_json\_dump\_error\_to\_buffer.  

See ac\_json\_dump\_error for an example.

## ac\_json\_dump\_error

```c
void ac_json_dump_error(FILE *out, ac_json_t *j);
```
ac\_json\_dump\_error dumps the error to the FILE handle.  This is typically used for debugging and out will be set to stderr or stdout.  

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

## ac\_json\_dump\_error\_to\_buffer

```c
void ac_json_dump_error_to_buffer(ac_buffer_t *bh, ac_json_t *j);
```
ac\_json\_dump\_error\_to\_buffer dumps the error to an ac\_buffer.

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
