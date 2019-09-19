[Table of Contents](README.md)  - Copyright 2019 Andy Curtis

# A quick word about licensing

Throughout this book you will see the following notice at the top of most of the files.

Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

It is my goal to produce a work that is unencumbered by licenses and my hope that others will find this code or parts of it useful.

# Getting Started

There is a little setup to get started.

Open the terminal (on a mac - Command+Space and type terminal)

Make sure you are in your home directory
```bash
cd
```

Optionally make a directory to place your code and change to it.
```bash
mkdir code
cd code
```

Clone the examples in this book into your newly created code directory
```bash
git clone https://github.com/contactandyc/standard-template-library-alternative-in-c.git
```

Create an environment variable named stla (standard template library alternative) and make the variable available the next time you open the terminal
```bash
echo stla=`pwd`/standard-template-library-alternative-in-c >> ~/.bashrc
```

Either close the terminal and reopen it or do the following to make the last changes take effect.
```bash
. ~/.bashrc
```

Change to the directory where the project exists
```bash
cd $stla
```

To test whether you have make and gcc installed (what's needed for this project).  Change the directory to the first example and then run make
```bash
cd $stla/illustrations/1_getting_started/1_test_setup
make
```

Output should be
```bash
Success!  You are now ready to continue with the rest of the examples in the book.
```

If an error occurs, use your favorite search engine and copy and paste the error and search for it.  This wasn't possible when I began writing software, but today, most errors are common.  It's likely that the answer will exist on sites like stackoverflow.com.  Just beware that the answers may be wrong, so you may need to look over a few answers.  This project depends upon gcc and make.  Once they are installed repeat the commands above (repeated below) until you get the Success! message.

If you've changed directories, then you will need to rerun this.
```bash
cd $stla/illustrations/1_getting_started/1_test_setup
```

Run this command and if you get the message afterwards, you will be good to go.
```bash
make
```

```bash
Success!  You are now ready to continue with the rest of the examples in the book.
```

Throughout this book, I will often reference how to run a command with a dollar ($) preceding it.  This is just to indicate that you are at a command prompt and that you should enter the text after the command.  For example,

```bash
make
```

```bash
Success!  You are now ready to continue with the rest of the examples in the book.
```

might be represented as
```bash
$ make
Success!  You are now ready to continue with the rest of the examples in the book.
```

This simply means that if you are following along, you should enter <b>make</b> at the command line and hit enter and expect the text Success...

If you change to the directory
```bash
cd $stla/illustrations
```

and run
```bash
ls -l
```

you should see the following
```bash
total 0
drwxr-xr-x  15 ac  staff  480 Sep 11 10:02 1_getting_started
drwxr-xr-x   4 ac  staff  128 Aug 30 21:15 2_timing
drwxr-xr-x   9 ac  staff  288 Sep  7 12:22 3_hello_buffer
drwxr-xr-x   9 ac  staff  288 Sep  7 12:22 4_linked_lists
drwxr-xr-x   9 ac  staff  288 Sep  7 12:22 5_pool
drwxr-xr-x   9 ac  staff  288 Sep  7 12:22 6_binary_search_tree
drwxr-xr-x   9 ac  staff  288 Sep  7 12:22 7_printing_the_binary_search_tree
drwxr-xr-x   9 ac  staff  288 Sep  7 12:22 8_red_black_tree
drwxr-xr-x   9 ac  staff  288 Aug 31 23:23 9_binary_search
drwxr-xr-x   9 ac  staff  288 Aug 31 23:23 10_quicksort
drwxr-xr-x   9 ac  staff  288 Aug 31 23:23 11_block_allocator
drwxr-xr-x   9 ac  staff  288 Aug 31 23:23 12_io
```

For each chapter, there will be a number and name that corresponds to the chapter in this book.  For example, within <i>1_getting_started</i>, there is a folder named 1_test_setup.
```bash
ls -l 1_getting_started
```

```bash
total 0
drwxr-xr-x  10 ac  staff  320 Sep 11 10:04 1_test_setup
```

The dates and user will likely be different.  Each illustration is named based upon the general application and the order within the chapter.  I will often reference that code is in a given directory.  The expectation is that you would know how to change to that directory.  That can be done by using the following command.

In the following, you would be expected to replace directory with the given directory
```bash
cd $stla/<directory>
```

For example, if the code was in <i>illustrations/1_getting_started/1_test_setup</i>, you would run the following command to change to the given directory.
```bash
cd $stla/illustrations/1_getting_started/1_test_setup
```

If you don't understand the terminal, I'd highly recommend spending some time going through tutorials to do so.  I don't plan to use many terminal related commands and I will try to detail exactly which commands that you need to run, but if there are problems, you won't have help.  Understanding the terminal is essential for any programmer.

# Continue to [Timing](2_timing.md)!
