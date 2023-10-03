#include "another-c-library/ac_md5.h"
#include "md5/md5.h"


uint64_t ac_md5(const void *s, size_t len) {
    return md5_hash(s, len);
}

uint64_t ac_md5_str(const char *s) {
    return md5_hash_str(s);
}
