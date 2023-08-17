# anotherclibrary v1.0

`anotherclibrary` is a C library that provides various functionalities. This document provides a guide on how to build and use the library.

## Dependencies

- [libuv (version 1.35 or newer)](https://libuv.org)
- [zlib](https://www.zlib.net)
- a C compiler

Please ensure these dependencies are installed on your system before building or installing `anotherclibrary`.

## Building

To build `anotherclibrary`, follow these steps:

```bash
mkdir build
cd build
./configure
make 
make install
```


## Configure Options

* `--enable-debug`<br/>
  This will enable debug mode and should be used when specifying enable-address-sanitizer and the memory check options.
* `--enable-address-sanitizer`<br/>
  The Address Sanitizer is specific to Mac OSX and can be used to find memory errors.
* `--with-memory-check`<br/>
  AnotherCLibrary can check memory that has been allocated / freed with ac_alloc and ac_free functions.  It can find memory leaks, invalid free(s), and double free(s) without sacrificing performance.  This option will report errors to the terminal.
* `--with-memory-check-file=filename`<br/>
  Similar to the prior option, except that instead of reporting to the terminal, the memory information is stored in the file (based upon filename).  The with-memory-check option only reports on memory leaks at the end of the program.  This option will output memory usage every 2 minutes.  It saves previous versions by rotating the log.


## Development

4. **Generate the Configuration Script**: Run the following command to generate the configuration script:
   ```bash
   autoreconf --install
   ```


## Contact

For any inquiries, suggestions, or issues, please contact the maintainer at [contactandyc@gmail.com](mailto:contactandyc@gmail.com).

## License

Please see the `LICENSE` file for information on how this library is licensed.


# Another C Library Table of Contents
- Copyright 2019 Andy Curtis and Daniel Curtis

The book/code started in August 2019, so it's a work in progress.  This is not a usage book.  I'm working on a book that simply explains usage.  I'm following patterns used by https://nikhilm.github.io/uvbook/index.html.  I think you create velocity by slowing down and making sure that you always have understanding of what you are doing (or at a minimum make sure you have an understanding of what you don't understand).   Developers get better when they can explain what they know (and this is independent of language or technology).  Becoming a great developer takes practice.  This book may take several reads before you fully get it.  The examples intentionally build upon each other, but may build too quickly for some.  Feel free to send me an email at contactandyc@gmail.com if you have questions.

This software and documentation contains code and ideas derived from works published by Robert Sedgewick relating to the Red-Black Tree and the Quicksort algorithm.  Many other works (both code and explanations) were studied.  I've spent years studying many open source implementations including the details of the linux kernel, so I'm sure you will see their influence in how the code is written.  I have tried to write the code without borrowing any code or explanations from other projects, but I'm sure that approaches, variable names, etc will look similar.  I've worked with brilliant engineers over the years and certainly borrowed approaches from their work.  I will try and call out where I have learned things when it is relevant (and I remember).  I'm sure that there are better ways to do things and welcome help!  

Goals of this project...
1. To provide an open source collection of algorithms necessary to build complex applications
2. To help engineers understand algorithms and C better, so that they can create their own
3. To show that it is possible to overcome many of the known challenges in C
4. To help people to learn what it takes to create something new
5. Build scalable applications using technology like Kubernetes, nginx, and docker.

My hope is that others will contribute!  

One of my favorite books is the Introduction to Algorithms by MIT Press.  I've often wondered if there was a follow up book (as the implication in the title is that it is for beginners).  I believe that the author wanted to convey that one should understand known algorithms to build better algorithms.  To that end, I found an improvement to quicksort.  I have no idea if my improvement to quicksort is new.  I could not find it in any existing open source implementation.  Read about it on [Medium](https://medium.com/@contactandyc/improving-quicksorts-worst-case-ef48f756bd4e) or [LinkedIn](https://www.linkedin.com/pulse/improving-60-year-old-algorithm-andy-curtis/)

```bash
git clone https://github.com/contactandyc/another-c-library.git
cd another-c-library/demo
make
```

The ac_schedule is demonstrated in a second demo directory named scheduler_demo.  This demos how to chain input together through a variety of sorts and merges to produce a final output.  In addition, the sorts and merges will work within the confines of the environment described by the user.  This is something like the hadoop map/reduce framework only written in C (and at the moment, only capable of supporting one box).  This framework attempts to stay out of your way, but also provides a number of useful utilities to get common work done.  

```bash
cd ../scheduler_demo
make
./word_demo -h
./word_demo .. --cpus 4
```

The package depends on libuv in the uvdemo directory.  On a mac, use the following command to install libuv.
```bash
brew install libuv
cd another-c-library/uvdemo
make
```

1. [Getting Started](website/docs/1-getting-started/index.md)
2. [Timing Your Code (the first project)](website/docs/2-timing/index.md)<br/>
   a. [A brief introduction to C](website/docs/2-timing/index.md#a-brief-introduction-to-c)<br/>
   b. [What happens during compilation](website/docs/2-timing/index.md#what-happens-during-compilation)<br/>
   c. [How to time code](website/docs/2-timing/index.md#how-to-time-code)<br/>
   d. [Reversing a string](website/docs/2-timing/index.md#reversing-a-string)<br/>
   e. [The basic Makefile](website/docs/2-timing/index.md#the-basic-makefile)<br/>
   f. [More accurately timing code](website/docs/2-timing/index.md#more-accurately-timing-code)<br/>
   g. [Compiler optimizations](website/docs/2-timing/index.md#compiler-optimizations)<br/>
   h. [Splitting up code into multiple files](website/docs/2-timing/index.md#splitting-up-code-into-multiple-files)<br/>
   i. [Separating the implementation from the interface](website/docs/2-timing/index.md#separating-the-implementation-from-the-interface)<br/>
   j. [Defining an object](website/docs/2-timing/index.md#defining-an-object)<br/>
   k. [The timer interface](website/docs/2-timing/index.md#the-timer-interface)<br/>
   l. [Making the timer object reusable](website/docs/2-timing/index.md#making-the-timer-object-reusable)<br/>
   m. [Splitting up a project into multiple directories](website/docs/2-timing/index.md#splitting-up-a-project-into-multiple-directories)<br/>
   n. [Splitting up the Makefile](website/docs/2-timing/index.md#splitting-up-the-makefile)<br/>
3. [The Buffer Object](website/docs/3-buffer/index.md)<br/>
   a. [How it compares to other languages](website/docs/3-buffer/index.md#how-it-compares-to-other-languages)<br/>
   b. [A bit of history and setup](website/docs/3-buffer/index.md#a-bit-of-history-and-setup)<br/>
   c. [The buffer interface](website/docs/3-buffer/index.md#the-buffer-interface)<br/>
   d. [The implementation](website/docs/3-buffer/index.md#the-implementation)<br/>
4. [Linked Lists](website/docs/4-linked-lists/index.md)<br/>
   a. [A data structure interface](website/docs/4-linked-lists/index.md#a-data-structure-interface)<br/>
   b. [The data structure interface test driver](website/docs/4-linked-lists/index.md#the-data-structure-interface-test-driver)<br/>
   c. [The singly linked list](website/docs/4-linked-lists/index.md#the-singly-linked-list)<br/>
   d. [The doubly linked list](website/docs/4-linked-lists/index.md#the-doubly-linked-list)<br/>
5. [Threads](website/docs/5-threads/index.md)<br/>
   a. [Introducing threads](website/docs/5-threads/index.md#introducing-threads)<br/>
   b. [Creating threads](website/docs/5-threads/index.md#creating-threads)<br/>
   c. [Threads and optimizing code](website/docs/5-threads/index.md#threads-and-optimizing-code)<br/>
   d. [Avoid global variables when you can](website/docs/5-threads/index.md#avoid-global-variables-when-you-can)<br/>
   e. [Mutexes](website/docs/5-threads/index.md#mutexes)<br/>
   f. [Timing considerations](website/docs/5-threads/index.md#timing-considerations)<br/>
6. [Macros](website/docs/6-macros/index.md)
7. [The Global Allocator Object](website/docs/7-allocator/index.md)
8. [The Global Allocator Implementation](website/docs/8-allocator-implementation/index.md)
9. [The Pool Object](website/docs/9-pool/index.md)
10. [Binary Search Trees](website/docs/10-binary-search/index.md)<br/>
   a. [The basic structure](website/docs/10-binary-search/index.md#the-basic-structure)<br/>
   b. [Find](website/docs/10-binary-search/index.md#find)<br/>
   c. [Insert](website/docs/10-binary-search/index.md#insert)<br/>
   d. [First, Last, Next, Previous](website/docs/10-binary-search/index.md#first-last-next-previous)<br/>
   e. [Erase](website/docs/10-binary-search/index.md#erase)<br/>
   f. [Postorder iteration](website/docs/10-binary-search/index.md#postorder_iteration)<br/>
   g. [Printing a binary tree](website/docs/10-binary-search/index.md#printing-a-binary-tree)<br/>
11. [Balancing Binary Search Trees](website/docs/11-balancing-binary-search-trees/index.md)<br/>
   a. [Why balancing is important](website/docs/11-balancing-binary-search-trees/index.md#why-balancing-is-important)<br/>
   b. [Properties of a red black tree](website/docs/11-balancing-binary-search-trees/index.md#properties-of-a-red-black-tree)<br/>
   c. [Coloring](website/docs/11-balancing-binary-search-trees/index.md#coloring)<br/>
   d. [Rotations](website/docs/11-balancing-binary-search-trees/index.md#rotations)<br/>
12. [The Red Black Tree](website/docs/12-red-black-tree/index.md)<br/>
   a. [Testing the red black tree properties](website/docs/12-red-black-tree/index.md#testing-the-red-black-tree-properties)<br/>
   b. [Insert](website/docs/12-red-black-tree/index.md#insert)<br/>
   c. [Erase](website/docs/12-red-black-tree/index.md#erase)<br/>
   d. [Packing color into the parent node](website/docs/12-red-black-tree/index.md#packing-color-into-the-parent-node)<br/>
13. (Started 9/23/19) [Turning the Red Black Tree into a map](website/docs/13-map/index.md)
14. [The set and multimap](website/docs/14-set-and-multimap/index.md)
15. (Coming soon!) Building a generic least recently used cache
16. (Coming soon!) Binary search
17. (Coming soon!) Heaps and priority queues
18. (Coming soon!) Quicksort
19. (Coming soon!) Building web services in C

# [Things that have been helpful to me](website/docs/tips.md)
1. [Line spacing in markdown](website/docs/tips.md#line-spacing-in-markdown)
2. [Escape characters in markdown](website/docs/tips.md#escape-characters-in-markdown)
3. [Create multiline macro in C](website/docs/tips.md#create-multiline-macro-in-c)
4. [Static inline vs inline](website/docs/tips.md#static-inline-vs-inline)
5. [Regex find replace](website/docs/tips.md#regex-find-replace)
6. [Atom-beautify problems](website/docs/tips.md#atom-beautify-problems)


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

# Let's [get started](website/docs/1_getting_started.md)!
