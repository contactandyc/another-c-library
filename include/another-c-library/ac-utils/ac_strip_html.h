#ifndef _ac_strip_html_H
#define _ac_strip_html_H

#include "another-c-library/ac_pool.h"

/* strips <tag href="..." />, captures most html tags */
char *ac_strip_html(ac_pool_t *h, const char *s);

#endif
