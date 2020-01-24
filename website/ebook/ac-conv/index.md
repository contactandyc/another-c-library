---
path: "/ac-conv"
posttype: "docs"
title: "ac_conv"
---

```c
#include "ac_conv.h"
```

ac_conv provides a series of string conversion functions.  Each function will allow for a default value in case the string value cannot be converted or if the string value is NULL.

## ac\_str

```c
const char *ac_str(const char *value, const char *default_value);
```
ac\_str returns a string if value is not NULL, otherwise default\_value

## ac\_bool

```c
bool ac_bool(const char *value, bool default_value);
```
ac\_bool returns a boolean if value is not NULL and valid, otherwise default\_value

## ac\_int

```c
int ac_int(const char *value, int default_value);
```
ac\_int returns an int if value is not NULL and valid, otherwise default\_value

## ac\_long

```c
long ac_long(const char *value, long default_value);
```
ac\_long returns a long if value is not NULL and valid, otherwise default\_value

## ac\_double

```c
double ac_double(const char *value, double default_value);
```
ac\_double returns a double if value is not NULL and valid, otherwise default\_value

## ac\_int32\_t

```c
int32_t ac_int32_t(const char *value, int32_t default_value);
```
ac\_int32\_t returns a int32\_t if value is not NULL and valid, otherwise default\_value

## ac\_uint32\_t

```c
uint32_t ac_uint32_t(const char *value, uint32_t default_value);
```
ac\_uint32\_t returns a uint32\_t if value is not NULL and valid, otherwise default\_value

## ac\_int64\_t

```c
int64_t ac_int64_t(const char *value, int64_t default_value);
```
ac\_int64\_t returns a int64\_t if value is not NULL and valid, otherwise default\_value

## ac\_uint64\_t

```c
uint64_t ac_uint64_t(const char *value, uint64_t default_value);
```
ac\_uint64\_t returns a uint64\_t if value is not NULL and valid, otherwise default\_value
