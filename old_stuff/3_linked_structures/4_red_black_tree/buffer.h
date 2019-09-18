#ifndef _buffer_H
#define _buffer_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

struct buffer_s;
typedef struct buffer_s buffer_t;

/* initial size of the buffer, buffer will grow as needed */
buffer_t *buffer_init(size_t initial_size);

/* clear the buffer */
void buffer_clear(buffer_t *h);

/* get the contents of the buffer */
char *buffer_data(buffer_t *h);
/* get the length of the buffer */
size_t buffer_length(buffer_t *h);

/* append bytes to the current buffer */
void buffer_append(buffer_t *h, const void *data, size_t length);
/* append a string to the current buffer */
void buffer_appends(buffer_t *h, const char *s);
/* append a character to the current buffer */
void buffer_appendc(buffer_t *h, char ch);
/* append a character n times to the current buffer */
void buffer_appendn(buffer_t *h, char ch, ssize_t n);
/* append bytes in current buffer using va_args and formatted string */
void buffer_appendvf(buffer_t *h, const char *fmt, va_list args);
/* append bytes in current buffer using a formatted string -similar to printf */
void buffer_appendf(buffer_t *h, const char *fmt, ...);

/* set bytes to the current buffer */
void buffer_set(buffer_t *h, const void *data, size_t length);
/* set a string to the current buffer */
void buffer_sets(buffer_t *h, const char *s);
/* set a character to the current buffer */
void buffer_setc(buffer_t *h, char ch);
/* set a character n times to the current buffer */
void buffer_setn(buffer_t *h, char ch, ssize_t n);
/* set bytes in current buffer using va_args and formatted string */
void buffer_setvf(buffer_t *h, const char *fmt, va_list args);
/* set bytes in current buffer using a formatted string -similar to printf */
void buffer_setf(buffer_t *h, const char *fmt, ...);

/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will retain the original data in the buffer for up to length bytes. */
void *buffer_resize(buffer_t *h, size_t length);
/* grow the buffer by length bytes and return pointer to the new memory.  This
   will retain the original data in the buffer for up to length bytes. */
void *buffer_append_alloc(buffer_t *h, size_t length);
/* resize the buffer and return a pointer to the beginning of the buffer.  This
   will NOT retain the original data in the buffer for up to length bytes. */
void *buffer_alloc(buffer_t *h, size_t length);

/* destroy the buffer */
void buffer_destroy(buffer_t *h);

#endif
