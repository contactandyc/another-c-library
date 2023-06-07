#ifndef _ac_in_base_H
#define _ac_in_base_H

#include "ac_common.h"

#include <inttypes.h>
#include <sys/types.h>

/*
   This is generally meant to be an internal object, but can be used
   externally if a need arises.  ac_in should be used instead.

   This should not be documented for the website usage.
*/

#include "impl/ac_in_base.h"

ac_in_base_t *ac_in_base_init_gz(const char *filename, int fd, bool can_close,
                                 size_t buffer_size);
ac_in_base_t *ac_in_base_init(const char *filename, int fd, bool can_close,
                              size_t buffer_size);
ac_in_base_t *ac_in_base_init_from_buffer(char *buffer, size_t buffer_size,
                                          bool can_free);
ac_in_base_t *ac_in_base_reinit(ac_in_base_t *base, size_t buffer_size);

const char *ac_in_base_filename(ac_in_base_t *h);

char *ac_in_base_read_delimited(ac_in_base_t *h, int32_t *rlen, int delim,
                                bool required);

/*
  returns NULL if len bytes not available
*/
char *ac_in_base_read(ac_in_base_t *h, int32_t len);

/*
  places a zero after *rlen bytes
*/
char *ac_in_base_readz(ac_in_base_t *h, int32_t *rlen, int32_t len);

void ac_in_base_destroy(ac_in_base_t *h);

#endif
