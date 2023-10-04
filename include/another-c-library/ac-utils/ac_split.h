#ifndef _ac_split_H
#define _ac_split_H

#include "another-c-library/ac_pool.h"

/* split a string into N pieces using any character in delim array.  The array
   that is returned will always be valid with a NULL string at the end if p is
   NULL. num_splits can be NULL if the number of returning pieces is not
   desired. */
char **ac_split_by_chars(ac_pool_t *h, size_t *num_splits, const char *delim,
                         const char *p);

char **ac_split_by_charsf(ac_pool_t *h, size_t *num_splits, const char *delim,
                           const char *p, ...);

char **ac_split_by_str(ac_pool_t *h, size_t *num_splits, const char *delim,
                         const char *p);

char **ac_split_by_strf(ac_pool_t *h, size_t *num_splits, const char *delim,
                      const char *p, ...);

char **ac_split_by_str2(ac_pool_t *h, size_t *num_splits, const char *delim,
                      const char *p);

char **ac_split_by_str2f(ac_pool_t *h, size_t *num_splits, const char *delim,
                       const char *p, ...);


#endif

