#ifndef _ac_in_H
#define _ac_in_H

#include "ac_in_base.h"

#include "ac_common.h"
#include "ac_io.h"
#include "ac_lz4.h"

#include <sys/types.h>

#include "impl/ac_in.h"

/* ac_in_options_t is declared in impl/ac_in.h.  r is expected to point to a
   structure of this type (and not NULL). */
void ac_in_options_init(ac_in_options_t *r);
void ac_in_options_fd(ac_in_options_t *r, int fd, bool owner);
void ac_in_options_buffer(ac_in_options_t *r, void *d, size_t len, bool owner);
void ac_in_options_buffer_size(ac_in_options_t *r, size_t buffer_size);
void ac_in_options_format(ac_in_options_t *r, ac_io_format_t format);
void ac_in_options_abort_on_error(ac_in_options_t *r);
void ac_in_options_tag(ac_in_options_t *r, int tag);
void ac_in_options_gz(ac_in_options_t *r);
void ac_in_options_lz4(ac_in_options_t *r, size_t lz4_buffer_size);

ac_in_t *ac_in_init(const char *filename, ac_in_options_t *options);

ac_io_record_t *ac_in_advance(ac_in_t *h);
ac_io_record_t *ac_in_advance_prefix(ac_in_t *h);
ac_io_record_t *ac_in_advance_fixed(ac_in_t *h, uint32_t length);
ac_io_record_t *ac_in_advance_delimited(ac_in_t *h, char delimiter,
                                        bool full_record_required);
void *ac_in_read(ac_in_t *h, int32_t *rlen, uint32_t length);

void ac_in_destroy(ac_in_t *h);

#endif
