# Standard Template Library in C Table of Contents
- Copyright 2019 Andy Curtis

1. [Getting Started](1_getting_started.md)
2. [Timing Your Code (the first project)](2_timing.md)<br/>
   a. [A brief introduction to C](2_timing.md#a-brief-introduction-to-c)<br/>
   b. [What happens during compilation](2_timing.md#what-happens-during-compilation)<br/>
   c. [How to time code](2_timing.md#how-to-time-code)<br/>
   d. [Reversing a string](2_timing.md#reversing-a-string)<br/>
   e. [The basic Makefile](2_timing.md#the-basic-makefile)<br/>
   f. [More accurately timing code](2_timing.md#more-accurately-timing-code)<br/>
   g. [Compiler optimizations](2_timing.md#compiler-optimizations)<br/>
   h. [Splitting up code into multiple files](2_timing.md#splitting-up-code-into-multiple-files)<br/>
   i. [Separating the implementation from the interface](2_timing.md#separating-the-implementation-from-the-interface)<br/>
   j. [Defining an object](2_timing.md#defining-an-object)<br/>
   k. [The timer interface](2_timing.md#the-timer-interface)<br/>
   l. [Making the timer object reusable](2_timing.md#making-the-timer-object-reusable)<br/>
   m. [Splitting up a project into multiple directories](2_timing.md#splitting-up-a-project-into-multiple-directories)<br/>
   n. [Splitting up the Makefile](2_timing.md#splitting-up-the-makefile)<br/>
3. [The Buffer Object](3_buffer.md)
4. [Linked Lists](4_linked_lists.md)
5. [Threads](5_threads.md)
6. [Macros](6_macros.md)
7. [The Global Allocator Object](7_allocator.md)
8. [The Global Allocator Implementation](8_allocator_impl.md)
9. [The Pool Object](9_pool.md)
10. [Binary Search Trees](10_binary_search_trees.md)
11. [Balancing Binary Search Trees](11_balancing_binary_search_trees.md)<br/>
  a. [Why balancing is important](11_balancing_binary_search_trees.md#why-balancing-is-important)<br/>
  b. [Properties of a red black tree](11_balancing_binary_search_trees.md#the-properties-of-a-red-black-tree)<br/>
  c. [Coloring](11_balancing_binary_search_trees.md#coloring)<br/>
  d. [Rotations](11_balancing_binary_search_trees.md#rotations)<br/>
12. [The Red Black Tree](12_red_black_tree.md)
13. (Coming soon!) Turning the Red Black Tree into a map, set
14. (Coming soon!) The multimap
14. (Coming soon!) Building a generic least recently used cache
15. (Coming soon!) Binary search
16. (Coming soon!) Heaps and priority queues
17. (Coming soon!) Quicksort

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
