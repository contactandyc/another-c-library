#include "another-c-library/ac_number_range.h"

#include "another-c-library/ac_search.h"
#include <stdio.h>
#include <string.h>

bool ac_number_range_parse(const char *str, ac_number_range_t *nr, int min_max) {
    if(!str || str[0] == 0)
        return false;

    int s=-min_max, e=min_max;
    if(strstr(str, "..")) {
        if(sscanf(str, "%d..%d", &s, &e) != 2)
            return false;
    }
    else if(str[0] == '+') {
        if(str[strlen(str)-1] == '+') // all time, pointless
            return false;
        if(sscanf(str+1, "%d", &e) != 1)
            return false;
    }
    else if(str[strlen(str)-1] == '+') {
        if(sscanf(str, "%d+", &s) != 1)
            return false;
    }
    else if(str[0] == '-') {
        e = 0;
        if(sscanf(str, "%d", &s) != 1)
            return false;
    }
    else if(str[0] >= '0' && str[0] <= '9') {
        s = 0;
        if(sscanf(str, "%d", &e) != 1)
            return false;
    }
    else
        return false;

    if(s >= e)
        return false;
    nr->start = s;
    nr->end = e;
    return true;
}

static inline int compare_number_range(const int *k, const ac_number_range_t *v) {
    if(*k != v->end)
        return (*k < v->end) ? -1 : 1;
    return 0;
}

static inline ac_search_upper_bound_m(search_number_ranges, int, ac_number_range_t, compare_number_range);

static inline bool _match_number_ranges(int ts, ac_number_range_t *nr, size_t num_nr) {
    ac_number_range_t *t = search_number_ranges(&ts, nr, num_nr);
    if(!t) return false;
    if(t->start <= ts && t->end >= ts)
        return true;
    return false;
}
/*
bool _match_number_ranges2(int ts, ac_number_range_t *nr, size_t num_nr) {
    for( size_t i=0; i<num_nr; i++ ) {
        if(nr[i].start <= ts && nr[i].end >= ts)
            return true;
    }
    return false;
}
*/
bool ac_number_range_match(int ts, ac_number_range_t *nr, size_t num_nr) {
    return _match_number_ranges(ts, nr, num_nr);
    /*
    bool rv2 = _match_number_ranges2(ts, nr, num_nr);
    if(rv != rv2) {
        printf( "Match Failed\n");
        _match_number_ranges(ts, nr, num_nr);
    }
    return rv2;
    */
}
