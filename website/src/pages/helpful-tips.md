---
title: Helpful Tips
description: Helpful tips for using Another C Library
---

A list of tips that have been helpful to me:

## Create multiline macro in C

On a mac, Command-F to get find and replace to show up on the bottom.  Next, select the [<b>\.\*</b>] button on the right side of the find and replace pane.  Replace dollar (<b>$</b>) with a space followed by a backslash (<b> \\</b>).  If you have atom-beautify installed, you can beautify (I have beautify on save enabled for the C and C++ languages and clang-format as beautifier).  If you have trouble with Atom-beautify, see next tip.

## Static inline vs inline

At least on my mac, when I compile code into binaries in debug mode (-g vs -O3), I get a linking error for functions which were declared inline vs static inline.

## Regex find replace

```
Find: less\(([^,]*),([^),]*)\)
Replace: less($1,$2, arg)
```

To find and replace all instances of less(a, b) with less(a, b, arg)
