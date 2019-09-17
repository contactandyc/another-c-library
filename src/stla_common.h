#ifndef _stla_common_H
#define _stla_common_H

/* defines NULL, size_t, offsetof */
#include <stddef.h>
/* because I like to use true, false, and bool */
#include <stdbool.h>

/*
Defining _STLA_DEBUG_MEMORY_ will check that memory is properly
freed (and try some rudimentary double free checks).  If memory
doesn't seem to be previously allocated, there is a scan to find
the closest block.  _STLA_DEBUG_MEMORY_ can be defined as NULL or
a valid string.  If it is defined as a string, then a file will be
written with the given name every _STLA_DEBUG_MEMORY_SPEED_ seconds.
Snapshots are saved in increasing intervals.  
*/
// #define _STLA_DEBUG_MEMORY_ "memory.log"

/* How often should the memory be checked? It is always checked in the
   beginning and every _STLA_DEBUG_MEMORY_SPEED_ seconds assuming
   _STLA_DEBUG_MEMORY_ is defined as a string (and not NULL). */
#define _STLA_DEBUG_MEMORY_SPEED_ 60


#define stla_parent_object(addr, base_type, field) (base_type *)((char *)addr-offsetof(base_type,field))

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)
#define __FILE_LINE__ __FILE__ ":" STRINGIZE(__LINE__)
#define FILE_LINE_MACRO(a) __FILE_LINE__ " [" a "]"

#endif
