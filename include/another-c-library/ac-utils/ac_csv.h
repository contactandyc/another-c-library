#ifndef _ac_csv_H
#define _ac_csv_H

#include "another-c-library/ac_pool.h"

/* split a string into N pieces using delimiter.
 https://en.wikipedia.org/wiki/Comma-separated_values

 RFC 4180 and MIME standards
 The 2005 technical standard RFC 4180 formalizes the CSV file format
 and defines the MIME type "text/csv" for the handling of text-based
 fields. However, the interpretation of the text of each field is still
 application-specific. Files that follow the RFC 4180 standard can
 simplify CSV exchange and should be widely portable. Among its requirements:

 1. Any field may be quoted (with double quotes).
 2. Fields containing a line-break, double-quote or commas
    should be quoted. (If they are not, the file will likely be
    impossible to process correctly.)
 3. If double-quotes are used to enclose fields, then a
    double-quote in a field must be represented by two
    double-quote characters.
 */
char **ac_csv_split(ac_pool_t *h, size_t *num_splits, char delim,
                         const char *p);
char **ac_csv_splitf(ac_pool_t *h, size_t *num_splits, char delim,
                          const char *p, ...);

char **ac_csv_split2(ac_pool_t *h, size_t *num_splits, char delim,
                          const char *p);
char **ac_csv_split2f(ac_pool_t *h, size_t *num_splits, char delim,
                           const char *p, ...);

char *ac_csv_encode(ac_pool_t *h, char delim, const char *s);
char *ac_csv_encodef(ac_pool_t *h, char delim, const char *p, ...);

#endif
