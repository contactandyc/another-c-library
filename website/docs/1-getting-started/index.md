---
path: "/1-getting-started"
posttype: "ebook"
title: "1. Getting Started"
---

## A quick word about licensing

Throughout this book, the following notice will appear at the top of most of the files.

Copyright 2019 Andy Curtis & Daniel Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

My goal is to produce a work that is unencumbered by licenses, and hope that others will find this code or parts of it useful.

## Getting Started

This first chapter covers setting up the computer for the projects in succeeding chapters. The following instructions execute in macOS Terminal, which holds consistent with Linux. However, Windows Command Line (now Terminal) may require some variation of the given commands.

Open the terminal (on a mac - `Command+Space` and type terminal)

Make sure you are in your home directory
```
cd
```

Optionally create a directory to place your code.
```
mkdir code
cd code
```

Clone the examples in this book into your newly created code directory
```
git clone https://github.com/contactandyc/another-c-library.git
```

Create an environment variable named ac (another c library) and create the variable available the next time you open the terminal
```
echo ac=`pwd`/another-c-library >> ~/.bashrc
```

Either close the terminal and reopen it or do the following to ensure the last changes take effect.
```
. ~/.bashrc
```

Change to the directory where the project exists
```
cd $ac
```

To test whether you have `make` and `gcc` installed (both requirements for this project).  Change the directory to the first example and then run `make`
```
cd $ac/illustrations/1_getting_started/1_test_setup
make
```

Output should be
```
Success!  You are now ready to continue with the rest of the examples in the book.
```

If an error occurs, use your favorite search engine and copy and paste the error and search for it. The answer will likely exist on sites like stackoverflow.com. These Internet resources did not exist when I began writing software. However, today, most errors are common. Just beware that the answers may be wrong, so you may need to look over a few answers. This project depends upon `gcc` and `make`. Post-installation, repeat the commands above (duplicated below) until you get the "Success!" message.

If you have changed directories, then you will need to rerun this.
```
cd $ac/illustrations/1_getting_started/1_test_setup
```

Run this command, and if you get the message afterward, you will be good to go.
```
make
```

```
Success!  You are now ready to continue with the rest of the examples in the book.
```

Throughout this book, I will often reference how to run a command with a dollar ($) preceding it, which indicates that you are at a command prompt. You should enter the text after the command, for example:

```
make
```

```
Success! You are now ready to continue with the rest of the examples in the book.
```

might be represented as
```
$ make
Success!  You are now ready to continue with the rest of the examples in the book.
```

Therefore, you should enter `make` at the command line and hit enter and expect the text "Success!..." if you are following along.

If you change to the directory
```
cd $ac/illustrations
```

and run

```
ls -l
```

you should see the following
```
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

For each chapter, there will be a number and name that corresponds to the chapter in this book.  For example, within <i>1_getting_started</i>, there is a folder named <i>1_test_setup</i>.
```
ls -l 1_getting_started
```

```
total 0
drwxr-xr-x  10 ac  staff  320 Sep 11 10:04 1_test_setup
```

The date and user will likely be different.  Each illustration is named based upon the general application and the order within the chapter.  I will often reference that code is in a given directory.  The expectation is that you would know how to change to that directory demonstrated below.

In the following, you would be expected to replace "<directory>" with the given directory
```
cd $ac/<directory>
```

For example, if the code was in <i>illustrations/1_getting_started/1_test_setup</i>, you would run the following command to change to the given directory.
```
cd $ac/illustrations/1_getting_started/1_test_setup
```

I would highly recommend going through tutorials if you are not familiar with the command line. Understanding the terminal is essential for any programmer. I do not plan on utilizing many terminal related commands but will try to detail the used commands.

[Table of Contents (if viewing on Github)](../../../README.md)
