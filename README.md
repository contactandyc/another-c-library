# Standard Template Library in C Table of Contents
- Copyright 2019 Andy Curtis

1. [Getting Started](1_getting_started.md)
2. [Timing Your Code (the first project)](2_timing.md)
3. [The Buffer Object](3_buffer.md)
4. [Linked Lists](4_linked_lists.md)
5. [Threads](5_threads.md)
6. [Macros](6_macros.md)
7. [The Global Allocator Object](7_allocator.md)
8. [The Global Allocator Implementation](8_allocator_impl.md)
9. [The Pool Object](9_pool.md)
10. [Binary Search Trees](10_binary_search_trees.md)
11. [Balancing Binary Search Trees](11_balancing_binary_search_trees.md)
12. [The Red Black Tree](12_red_black_tree.md)

# Some useful reading...

This book assumes you have a basic understanding of C.  I hope to show some tricks along the way, but you should have a basic understanding of the language before continuing.  You should also know machine architecture, multithreaded programming, the bash shell, and probably read over kubernetes tutorials.  I like to avoid thread contention, context switching, and optimize cpu caching in multithreaded programming (which this will be).  To do this, I believe that you need to avoid locking as much as possible and carve out different spaces in RAM for each thread to operate in.  It's good to understand machine architecture to really understand optimizations.

## Markdown (how to write this)
[Markdown Cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)<br/>

## C Tutorials
https://www.programiz.com/c-programming<br/>
https://www.tutorialspoint.com/cprogramming/<br/>
https://www.learn-c.org<br/>
https://www.guru99.com/c-programming-tutorial.html<br/>
https://www.javatpoint.com/c-programming-language-tutorial<br/>

<a href="http://www.youtube.com/watch?feature=player_embedded&v=KJgsSFOSQv0
" target="_blank"><img src="http://img.youtube.com/vi/KJgsSFOSQv0/0.jpg"
alt="C Programming for Beginners" width="480" height="360" border="10" /></a>

## Understanding the machine
[Why software developers should care about CPU caches](https://medium.com/software-design/why-software-developers-should-care-about-cpu-caches-8da04355bb8a)<br/>

# Let's [get started](1_getting_started.md)!
