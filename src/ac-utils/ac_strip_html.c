#include "another-c-library/ac-utils/ac_strip_html.h"

char *ac_strip_html(ac_pool_t *h, const char *s) {
    char *r = ac_pool_strdup(h, s);
    char *wp = r;
    char *sp = r;
    while(*sp == ' ')
        sp++;
    while(*sp) {
        if(*sp == '<' && strchr(sp+1, '>')) {
            sp++;
            while(*sp && *sp != '>') {
                if(*sp == '\"') {
                    sp++;
                    while(*sp && *sp != '\"') {
                        if(*sp == '\\')
                            sp++;
                        sp++;
                    }
                    if(*sp == '\"')
                        sp++;
                }
                else if(*sp == '\'') {
                    sp++;
                    while(*sp && *sp != '\'') {
                        if(*sp == '\\')
                            sp++;
                        sp++;
                    }
                    if(*sp == '\'')
                        sp++;
                }
                else
                    sp++;
             }
             if(*sp == '>')
                sp++;
        }
        else {
            if(*sp == ' ' && wp > r && wp[-1] == ' ')
                sp++;
            else
                *wp++ = *sp++;
        }
    }
    while(wp > r && wp[-1] == ' ')
        wp--;
    *wp = 0;
    return r;
}
