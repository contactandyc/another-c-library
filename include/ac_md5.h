#ifndef _ac_md5_H
#define _ac_md5_H

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t ac_md5(const void *s, size_t len);
uint64_t ac_md5_str(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* _ac_md5_H */