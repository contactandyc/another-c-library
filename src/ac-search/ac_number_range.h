#ifndef _ac_number_range_H
#define _ac_number_range_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int32_t start;
    int32_t end;
} ac_number_range_t;

bool ac_number_range_parse(const char *str, ac_number_range_t *nr, int min_max);
bool ac_number_range_match(int ts, ac_number_range_t *nr, size_t num_nr);

#endif