/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _ac_common_H
#define _ac_common_H

/* defines NULL, size_t, offsetof */
#include <stddef.h>
/* because I like to use true, false, and bool */
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Defining _AC_DEBUG_MEMORY_ will check that memory is properly
freed (and try some rudimentary double free checks).  If memory
doesn't seem to be previously allocated, there is a scan to find
the closest block.  _AC_DEBUG_MEMORY_ can be defined as NULL or
a valid string.  If it is defined as a string, then a file will be
written with the given name every _AC_DEBUG_MEMORY_SPEED_ seconds.
Snapshots are saved in increasing intervals.
*/
// #define _AC_DEBUG_MEMORY_ "memory.log"

/* How often should the memory be checked? It is always checked in the
   beginning and every _AC_DEBUG_MEMORY_SPEED_ seconds assuming
   _AC_DEBUG_MEMORY_ is defined as a string (and not NULL). */
#ifndef _AC_DEBUG_MEMORY_SPEED_
#define _AC_DEBUG_MEMORY_SPEED_ 60
#endif
/*
  Given an address of a member of a structure, the base object type, and the
  field name, return the address of the base structure.
*/
#define ac_parent_object(addr, base_type, field)                               \
  (base_type *)((char *)addr - offsetof(base_type, field))

#define AC_STRINGIZE2(x) #x
#define AC_STRINGIZE(x) AC_STRINGIZE2(x)
#define __AC_FILE_LINE__ __FILE__ ":" AC_STRINGIZE(__LINE__)
#define AC_FILE_LINE_MACRO(a) __AC_FILE_LINE__ " [" a "]"

#ifdef __cplusplus
}
#endif

#endif
