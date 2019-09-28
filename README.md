# Standard Template Library in C Table of Contents
- Copyright 2019 Andy Curtis

This software and documentation contains code and ideas derived from works published by Robert Sedgewick relating to the Red-Black Tree and the Quicksort algorithm.  Many other works (both code and explanations) were studied.  I've spent years studying many open source implementations including the details of the linux kernel, so I'm sure you will see their influence in how the code is written.  I have tried to write the code without borrowing any code or explanations from other projects, but I'm sure that approaches, variable names, etc will look similar.  I've worked with brilliant engineers over the years and certainly borrowed approaches from their work.  I will try and call out where I have learned things when it is relevant (and I remember).  I'm sure that there are better ways to do things and welcome help!  

Goals of this project...
1. To provide an open source collection of algorithms necessary to build complex applications
2. To help engineers understand algorithms and C better, so that they can create their own
3. To show that it is possible to overcome many of the known challenges with C
4. To help people to learn what it takes to create something new
5. Build scalable applications using technology like Kubernetes, nginx, and docker.

My hope is that others will contribute!  

One of my favorite books is the Introduction to Algorithms by MIT Press.  I've often wondered if there was a follow up book (as the implication in the title is that it is for beginners).  I believe that the author wanted to convey that one should understand known algorithms to build better algorithms.  

```bash
git clone https://github.com/contactandyc/standard-template-library-alternative-in-c.git
cd standard-template-library-alternative-in-c/demo
make
```

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
3. [The Buffer Object](3_buffer.md)<br/>
   a. [How it compares to other languages](3_buffer.md#how-it-compares-to-other-languages)<br/>
   b. [A bit of history and setup](3_buffer.md#a-bit-of-history-and-setup)<br/>
   c. [The buffer interface](3_buffer.md#the-buffer-interface)<br/>
   d. [The implementation](3_buffer.md#the-implementation)<br/>
4. [Linked Lists](4_linked_lists.md)<br/>
   a. [A data structure interface](4_linked_lists.md#a-data-structure-interface)<br/>
   b. [The data structure interface test driver](4_linked_lists.md#the-data-structure-interface-test-driver)<br/>
   c. [The singly linked list](4_linked_lists.md#the-singly-linked-list)<br/>
   d. [The doubly linked list](4_linked_lists.md#the-doubly-linked-list)<br/>
5. [Threads](5_threads.md)<br/>
   a. [Introducing threads](5_threads.md#introducing-threads)<br/>
   b. [Creating threads](5_threads.md#creating-threads)<br/>
   c. [Threads and optimizing code](5_threads.md#threads-and-optimizing-code)<br/>
   d. [Avoid global variables when you can](5_threads.md#avoid-global-variables-when-you-can)<br/>
   e. [Mutexes](5_threads.md#mutexes)<br/>
   f. [Timing considerations](5_threads.md#timing-considerations)<br/>
6. [Macros](6_macros.md)
7. [The Global Allocator Object](7_allocator.md)
8. [The Global Allocator Implementation](8_allocator_impl.md)
9. [The Pool Object](9_pool.md)
10. [Binary Search Trees](10_binary_search_trees.md)<br/>
   a. [The basic structure](10_binary_search_trees.md#the-basic-structure)<br/>
   b. [Find](10_binary_search_trees.md#find)<br/>
   c. [Insert](10_binary_search_trees.md#insert)<br/>
   d. [First, Last, Next, Previous](10_binary_search_trees.md#first-last-next-previous)<br/>
   e. [Erase](10_binary_search_trees.md#erase)<br/>
   f. [Postorder iteration](10_binary_search_trees.md#postorder_iteration)<br/>
   g. [Printing a binary tree](10_binary_search_trees.md#printing-a-binary-tree)<br/>
11. [Balancing Binary Search Trees](11_balancing_binary_search_trees.md)<br/>
   a. [Why balancing is important](11_balancing_binary_search_trees.md#why-balancing-is-important)<br/>
   b. [Properties of a red black tree](11_balancing_binary_search_trees.md#properties-of-a-red-black-tree)<br/>
   c. [Coloring](11_balancing_binary_search_trees.md#coloring)<br/>
   d. [Rotations](11_balancing_binary_search_trees.md#rotations)<br/>
12. [The Red Black Tree](12_red_black_tree.md)<br/>
   a. [Testing the red black tree properties](12_red_black_tree.md#testing-the-red-black-tree-properties)<br/>
   b. [Insert](12_red_black_tree.md#insert)<br/>
   c. [Erase](12_red_black_tree.md#erase)<br/>
   d. [Packing color into the parent node](12_red_black_tree.md#packing-color-into-the-parent-node)<br/>
13. (Started 9/23/19) [Turning the Red Black Tree into a map](13_map.md)
14. [The set and multimap](14_set_and_multimap.md)
15. (Coming soon!) Building a generic least recently used cache
16. (Coming soon!) Binary search
17. (Coming soon!) Heaps and priority queues
18. (Coming soon!) Quicksort
19. (Coming soon!) Building web services in C

# [Things that have been helpful to me](tips.md)
1. [Line spacing in markdown](tips.md#line-spacing-in-markdown)
2. [Escape characters in markdown](tips.md#escape-characters-in-markdown)
3. [Create multiline macro in C](tips.md#create-multiline-macro-in-c)
4. [Static inline vs inline](tips.md#static-inline-vs-inline)
5. [Atom-beautify problems](tips.md#atom-beautify-problems)


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
