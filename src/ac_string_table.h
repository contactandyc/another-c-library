#ifndef _ac_string_table_h
#define _ac_string_table_h

#include <inttypes.h>

/*
    String table which can be reopened to be updated.  Ideally, this would support
    referring to a string by offset or by the id of the string.  Because strings
    would be kept in order, there would also need to be a mapping array to allow
    old ids to be changed.  Maybe because of this, it makes sense to always compact
    the space to a set of ids and then allow the actual offset management to happen
    through a layer.
*/

struct ac_string_table_s;
typedef struct ac_string_table_s ac_string_table_t;

/* opens a read only string table which will allow id => string and string => id */
ac_string_table_t *ac_string_table_init(const char *filename);

/* return the id of a string (or zero if not found) */
uint32_t ac_string_table_find(ac_string_table_t *h, const char *s);

/* returns the number of strings */
uint32_t ac_string_table_size(ac_string_table_t *h);

/* returns a string given an id, NULL if id is out of bounds */
const char *ac_string_table_lookup(ac_string_table_t *h, uint32_t id);

/* opens string table so that it can be expanded */
void ac_string_table_expand(ac_string_table_t * h);

/* adds strings if they didn't already exist */
void ac_string_table_add(ac_string_table_t *h, const char *s);

/* this returns a id mapping which maps the old id to the new id.  Any
   id which was referencing the string table should go through this
   conversion so that the id points to the proper string.  The
   return value must be freed using ac_free(...)
 */
uint32_t *ac_string_table_save(ac_string_table_t *h, const char *filename);

void ac_string_table_destroy(ac_string_table_t *h);

#endif
