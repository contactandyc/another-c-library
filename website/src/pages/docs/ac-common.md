---
title: ac_common
description:
---

```c
#include "ac_common.h"
```

`ac_common` is meant to help normalize platforms by including platform specific header files. It also includes commonly used types like stddef (for `size_t`), stdbool (for bool), and inttypes (for `intXX_t` like types).

### `__AC_FILE_LINE__`

Macro provides the current filename and line number as a constant string that will persist for the life of the program.

### `AC_FILE_LINE_MACRO`

```c
AC_FILE_LINE_MACRO(a)
```
Macro provides the current filename, line number, and the contents of [ **a** ] in square braces.

### `ac_parent_object`

```c
ac_parent_object(addr, base_type, field)
```

Given an address of a member of a structure, the base object type, and the field name, return the address of the base structure.

### `_AC_DEBUG_MEMORY_`

Defining _AC_DEBUG_MEMORY_ will check that memory is properly freed (and try some rudimentary double free checks). If memory doesn't seem to be previously allocated, there is a scan to find the closest block. `_AC_DEBUG_MEMORY_` can be defined as NULL or
a valid string. If it is defined as a string, then a file will be written with the given name every `_AC_DEBUG_MEMORY_SPEED_` seconds. Snapshots are saved in increasing intervals.

### `_AC_DEBUG_MEMORY_SPEED_`

How often should the memory be checked? It is always checked in the beginning and every `_AC_DEBUG_MEMORY_SPEED_` seconds assuming `_AC_DEBUG_MEMORY_` is defined as a string (and not NULL). This defaults to 60 seconds if not defined through compiler options or elsewhere.

<NextPrev prev="ac_buffer" prevUrl="/docs/ac-buffer" next="ac_conv" nextUrl="/docs/ac-conv" />
