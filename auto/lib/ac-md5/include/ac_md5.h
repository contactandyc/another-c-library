#ifndef _ac_md5_H
#define _ac_md5_H

#include <inttypes.h>

uint64_t ac_md5(const void *s, size_t len);
uint64_t ac_md5_str(const char *s);

#endif /* _ac_md5_H */