# Change Log

## 06/07/2023


### Changes
* ac_cgi.c/h

supports cgi conversion to a json object to allow post simulation through CGI params
```ac_json_t *ac_cgi_to_json(ac_pool_t *pool, const char *q);```

* ac_conv.c
  * changed SCNd64 => SCNi64 for better portability

* ac_cursor.c/h
  * A generic cursor functionality (allows for advanced id selection)

* ac_file_sync.c/h
  * Provides a mechanism to synchronize files between a local and remote source

* ac_in.c/h, ac_in_base.c/h
  * Support ac_io_csv_delimiter for the csv format (internal changes)
  * ac_in_quick_init - a helper function to open a cursor on a file
```ac_in_t *ac_in_quick_init(const char *filename, ac_io_format_t format, size_t buffer_size)```
  * ac_in_init_from_cb - opens one cursor at a time as returned from callback.  The cursor is complete when the callback returns NULL
```ac_in_t *ac_in_init_from_cb(ac_in_init_cb cb, void *arg);```

* ac_io.c/h
  * Added ac_io_csv_delimiter 
```ac_io_format_t ac_io_csv_delimiter(int delim)```

* ac_json.c/h impl/ac_json.c/h
  * Fixed bug where quote is followed by a encoded double slash `\\`

* ac_out.c/h
    * Minor cleanup, fixed memory leak

* ac_pool.c/h
  * ac_pool_pool_init - create a pool within a pool
```ac_pool_t *ac_pool_pool_init(ac_pool_t *pool, size_t initial_size)```
  * ac_pool_split_csv/ac_pool_csv_encode functions to support csv encoding/decoding/splitting
  * ac_pool_strip_html - strips html from text

* ac_serve.c/h
  * improved service layer (better cors support, json integration)

* Other impl changes
  * Bug fixes to ac_map/ac_serve

### Additions
* ac_bit_set.c/h
  * A bitset which uses 1 bit per id
  * Supports boolean operations on the set
  * Supports counting
* ac_boolean_tree_node.c/h
  * Breaks a string based upon &&/||/--/[] into a tree for further parsing
* ac_number_range.c/h
  * Used to parse numeric ranges such as 1-3 or 1+, etc
* ac_odbc.c/h
  * An experimental object for connecting to odbc compliant databases
* ac_s.c/h
  * A live updatable search
* ac_search_builder.c/h
  * A static search
* ac_string_map.c/h
  * Used to compress strings to ids and a string buffer
* ac_string_table.c/h
  * An alternate approach to ac_string_map which makes ids sequential
* ac_token.c/h
  * Tokenization functions for search
* ac_top_n.h
  * Supports topN items often used in search