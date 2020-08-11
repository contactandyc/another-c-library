---
title: ac_conv
description:
---

```c
#include "ac_conv.h"
```

ac_conv provides a series of string conversion functions. Each function will allow for a default value in case the string value cannot be converted or if the string value is NULL.

## ac_str

```c
const char *ac_str(const char *value, const char *default_value);
```
ac_str returns a string if value is not NULL, otherwise default_value

## ac_bool

```c
bool ac_bool(const char *value, bool default_value);
```
ac_bool returns a boolean if value is not NULL and valid, otherwise default_value

## ac_int

```c
int ac_int(const char *value, int default_value);
```
ac_int returns an int if value is not NULL and valid, otherwise default_value

## ac_long

```c
long ac_long(const char *value, long default_value);
```
ac_long returns a long if value is not NULL and valid, otherwise default_value

## ac_double

```c
double ac_double(const char *value, double default_value);
```
ac_double returns a double if value is not NULL and valid, otherwise default_value

## ac_int32_t

```c
int32_t ac_int32_t(const char *value, int32_t default_value);
```
ac_int32_t returns a int32_t if value is not NULL and valid, otherwise default_value

## ac_uint32_t

```c
uint32_t ac_uint32_t(const char *value, uint32_t default_value);
```
ac_uint32_t returns a uint32_t if value is not NULL and valid, otherwise default_value

## ac_int64_t

```c
int64_t ac_int64_t(const char *value, int64_t default_value);
```
ac_int64_t returns a int64_t if value is not NULL and valid, otherwise default_value

## ac_uint64_t

```c
uint64_t ac_uint64_t(const char *value, uint64_t default_value);
```
ac_uint64_t returns a uint64_t if value is not NULL and valid, otherwise default_value

<NextPrev prev="ac_common" prevUrl="/docs/ac-common" next="ac_in" nextUrl="/docs/ac-in" />
