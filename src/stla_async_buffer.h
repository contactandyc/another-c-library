#ifndef _stla_async_buffer_H
#define _stla_async_buffer_H

#include <stddef.h>

struct stla_async_buffer_s;
typedef struct stla_async_buffer_s stla_async_buffer_t;

typedef void (*stla_async_buffer_f)(stla_async_buffer_t *);

/*  Init/clear/destroy functionality  */
#ifdef _STLA_DEBUG_MEMORY_
#define stla_async_buffer_init()                                               \
  _stla_async_buffer_init(STLA_FILE_LINE_MACRO("stla_async_buffer"))
stla_async_buffer_t *_stla_async_buffer_init(const char *caller);
#else
#define stla_async_buffer_init() _stla_async_buffer_init()
stla_async_buffer_t *_stla_async_buffer_init();
#endif

void stla_async_buffer_clear(stla_async_buffer_t *);
void stla_async_buffer_destroy(stla_async_buffer_t *);

void stla_async_buffer_set_arg(stla_async_buffer_t *, void *);
void *stla_async_buffer_get_arg(stla_async_buffer_t *);

/*  The advance functions return 1 for success and 0 for failure  */
int stla_async_buffer_advance_to_char(stla_async_buffer_t *, char delimiter,
                                      stla_async_buffer_f);
int stla_async_buffer_advance_to_string(stla_async_buffer_t *,
                                        char const *delimiter,
                                        stla_async_buffer_f);
int stla_async_buffer_advance_to_mem(stla_async_buffer_t *, void *delimiter,
                                     size_t delimiter_length,
                                     stla_async_buffer_f);
int stla_async_buffer_advance_bytes(stla_async_buffer_t *, size_t bytes,
                                    stla_async_buffer_f);

/*  Get the data for the current advance action  */
char *stla_async_buffer_data(stla_async_buffer_t *);
/*  Get the length of the current advance action.
    This may be 0 if the delimiter was at the 0th position.  */
size_t stla_async_buffer_data_length(stla_async_buffer_t *);

void stla_async_buffer_parse(stla_async_buffer_t *, void const *data,
                             size_t data_length);
#endif
