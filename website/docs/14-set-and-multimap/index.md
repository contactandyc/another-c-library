---
path: "/14-set-and-multimap"
posttype: "ebook"
title: "14. The Set and Multimap"
---

# The set and multimap

## The set

C++ and other languages define sets such that the entire value is the key.  The ac_map.h/c doesn't break out the key and value.  The find and insert methods can use some or all of the structure.  

## The multimap

This is much like the set.  The insert method needs to make sure that comparisons never return 0.  This can be done by comparing the node's address after all other comparisons are complete.  The find method must use the find_first (or find_last) approach that was talked about in the last chapter.

This is by far my favorite chapter to write :-)  *NOTE TO SELF* update this with some examples.

[Table of Contents (only if viewing on Github)](../../../README.md)
