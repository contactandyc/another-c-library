---
title: ac_common
description:
---

```c
#include "ac_common.h"
```

ac\_common is meant to help normalize platforms by including platform specific header files.  It also includes commonly used types like stddef (for size\_t), stdbool (for bool), and inttypes (for intXX\_t like types).

## \_\_AC\_FILE\_LINE\_\_

Macro provides the current filename and line number as a constant string that will persist for the life of the program.

## AC\_FILE\_LINE\_MACRO

```c
AC\_FILE\_LINE\_MACRO(a)
```
Macro provides the current filename, line number, and the contents of [ **a** ] in square braces.

## ac_parent_object

```c
ac_parent_object(addr, base_type, field)
```

Given an address of a member of a structure, the base object type, and the field name, return the address of the base structure.

## \_AC\_DEBUG\_MEMORY\_

Defining \_AC\_DEBUG\_MEMORY\_ will check that memory is properly freed (and try some rudimentary double free checks).  If memory doesn't seem to be previously allocated, there is a scan to find the closest block.  \_AC\_DEBUG\_MEMORY\_ can be defined as NULL or
a valid string.  If it is defined as a string, then a file will be written with the given name every \_AC\_DEBUG\_MEMORY\_SPEED\_ seconds.  Snapshots are saved in increasing intervals.

## \_AC\_DEBUG\_MEMORY\_SPEED\_

How often should the memory be checked? It is always checked in the beginning and every \_AC\_DEBUG\_MEMORY\_SPEED\_ seconds assuming \_AC\_DEBUG\_MEMORY\_ is defined as a string (and not NULL).  This defaults to 60 seconds if not defined through compiler options or elsewhere.
