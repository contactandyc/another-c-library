[Table of Contents](README.md)  - Copyright 2019 Andy Curtis

## Introducing threads

Imagine if you were building a block building, and you had to paint the level on each block up to 100 levels.  To know which block to paint, you would look at the last block on the building, add one to it, and go and paint your block.  Once the block is dry, you can come back and put the block on the building.  At this point, you can repeat the process.  You are only allowed to paint one block at a time, and you must look at the highest block to determine what to paint on the next block.  

This would be fine if you were the only person working on the project.  You want to get the project done faster, so you enlist the help of a friend.  The friend isn't allowed to talk to you.  Your friend begins doing the same thing you are doing.  You come to the building and start on the first block. While your block is drying, your friend comes to the building and sees that the first block needs to be done. They also start on the first block.  You came back and put the first block on and then start on the second block.  A short time later, your friend comes back with the first block and realizes that you already built the first block and has to scrap their work.  Frustrated, they go ahead and start on the second block.  This time, the friend gets the second block done before you do and starts on the third block.  You come back and see that your work is wasted, so you start on the next block.  After a while, you have a lot of wasted blocks and work.  Adding your friend isn't speeding anything up.  You recognize that you must coordinate your work for it to be productive.

Coordinating your work can be done in several ways.  What if each person had a stop sign and could stop the other person from working for some time.  When you come to the building, you recognize that you need to build block 1.  You put a stop sign until you finish block 1.  Your friend comes to the building and sees the stop sign, so they wait until it no longer says stop.  When you came back with block one and put it on, you pull your stop sign, and your friend puts up theirs.  Your friend sees that block two needs built and goes ahead and works on the block.  You have to wait because your friend put up a stop sign.  In this scenario, work and blocks are no longer wasted.  However, the overall project time isn't improved because you and your friend cannot work simultaneously.  

Now consider if each person was also given the ability to write a note about the block that they were working on.  You come to the building, put up your stop sign, write a note to indicate that you are going to do block 1, take your stop sign down and go do block 1.  Your friend comes to the building and puts up their stop sign and sees your note.  They put a note that they are building block 2, and takes down their stop sign and begins working on block 2.  For whatever reason, your friend gets block 2 done before you get block 1 done.  When your friend comes back, they see that they cannot put their block on the building and just keeps repeatedly checking the height of the building every few minutes, waiting on you.  You finally come back, put up your stop sign, and put block 1 on the building.  You see your friend's note that they were working on block 2, so you write a note indicating that you will work on block 3, and take down your stop sign.  A couple minutes later, your friend checks and sees that the building has the first block, so they put up their stop sign and put block 2 on the building. They read the note that you are working on block 3, write a note that they are working on block 4, take down their stop sign and begin working on block 4.  This is much more efficient, assuming that it takes less time to write the note than it does to finish the block (which I'm assuming).

There is still more to optimize.  When a block is finished, the person finishing the block must keep checking the height of the building to see if they can put their block on the building before they can put up their stop sign and do the work.  One approach is to just check the building height more frequently, such as every second, but maybe there's a better approach.  What if we could introduce a pager and each person could broadcast to all of the other builders who were waiting on the building to grow that they should check the height of the building.  Additionally, if you put your stop sign up and someone else has a stop sign already up, yours will immediately take effect once the other stop sign is taken down, assuming you were first in line.  Instead of frequently checking the height of the building, you could go and take a nap and would be woken up by your pager, assuming you indicated that you were waiting on the building to grow.  When the pager goes off, you immediately get up and put up your stop sign.  As soon as the message broadcaster picks up their stop sign, the person who was paged can begin work.

We can think about each person's work as pseudocode:

worker
```
1. Put up stop sign
2. Get next block to build checking building height and notes
3. If the building is already the height desired, then you are done!
   You can quit as soon as you take down your stop sign.
4. Write note indicating block that you are going to build
5. Take down stop sign
6. Build block
7. Put up stop sign
8. If block you just built can be added
     a) add the block
     b) broadcast to everyone that is waiting on the building
        to grow that it grew
     c) go to step 2 (which will have you taking your stop sign down at step 5)
   Otherwise
     a) turn on your pager waiting on building to grow
     b) take down your stop sign
     b) go take a nap
     c) when pager goes off goto step 7
```

Computers allow for multiple threads (which are like workers or people working on the same problem).  You can put up stop signs and block everyone else until you are done.  You can also create conditions and wait on those conditions to become true (like the pager example).  Doing something as simple as incrementing a number needs to be protected with the stop signs.  

## Creating threads

In C, threads are created using pthread_create.  They can be created with the expectation that they will be collected once they are done.  They can also be created in what is known as a detached state.  All this means is that your program will not wait for the thread to finish.  In both cases, if your program terminates, the threads will be destroyed.  Threads are like workers.  They run in parallel often on different processors.  The example below shows 10 threads incrementing a number a million times and ultimately printing the final number.  The threads are collected using pthread_join.

The following code is found in <i>illustrations/4_threads/1_thread</i>

test_code.c
```c
#include <stdio.h>
#include <pthread.h>

int global_number;

void *worker(void *arg) {
  for( int i=0; i<1000000; i++ )
    global_number++;
  return NULL;
}

int main(int argc, char *argv[]) {
  global_number = 0;
  pthread_t threads[10];
  for( int i=0; i<10; i++ )
    pthread_create(&threads[i], NULL, worker, NULL);

  for( int i=0; i<10; i++ )
    pthread_join(threads[i], NULL);
  printf( "global_number (should be 10000000)= %d\n", global_number );
  return 0;
}
```

```bash
$ gcc test_code.c -o test_code -lpthread
$ ./test_code
global_number (should be 10000000)= 2010195
```

Notice that the number is not 10 million.  When global_number++ happens, each thread gets the value of global_number, adds one to it, and then replaces global_number with the new value.  Much like when we initially had a friend help build the building above, each thread is doing very similar work and then overwriting other thread's work.

## Threads and optimizing code

When writing software which has resources that need protecting, one should generally assume that you must protect them (using the equivalent of the stop signs mentioned above).  Sometimes, when you run software and tests, the result will look okay.  This doesn't necessarily mean that the code is thread safe.  On my computer, when I compiled the code with the -O3 option, I got the following result:

```bash
cd $stla/illustrations/4_threads/2_thread
gcc -O3 test_code.c -o test_code -lpthread
./test_code
```

Outputs
```bash
global_number (should be 10000000)= 10000000
```

which makes it look like everything is working.  A rule with writing good code is that you should assume if something can go wrong, it will go wrong.  Changing the compiler flag should not cause the code to perform differently.  Particularly, code that works when a flag is set and doesn't work when a flag is not set is not good.  It must work regardless of the flag.  Because the -O3 flag seems to make the program look like it is working, we will not use it.  Code may work in debug mode, but not in optimized mode.

## Avoid global variables when you can

I generally do not like to use global variables (unless they make sense).  In the above example, global_number is a global variable.  The pthread_create function allows you to pass an argument to the thread function for the thread that is being created.  For example:

The following code is found in <i>illustrations/4_threads/3_thread</i>

test_code.c
```c
#include <stdio.h>
#include <pthread.h>

void *worker(void *arg) {
  int *local_number = (int *)arg;
  for( int i=0; i<1000000; i++ )
    (*local_number)++;
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t threads[10];
  int local_number = 0;
  for( int i=0; i<10; i++ )
    pthread_create(&threads[i], NULL, worker, &local_number);

  for( int i=0; i<10; i++ )
    pthread_join(threads[i], NULL);
  printf( "local_number (should be 10000000)= %d\n", local_number );
  return 0;
}
```

```bash
$ gcc test_code.c -o test_code -lpthread
$ ./test_code
local_number (should be 10000000)= 2010195
```

## Mutexes

The pthread library defines a type called mutex which allows your code to lock around a resource much like a stop sign.  The mutex needs initialized and destroyed.  pthread_create only allows one argument to be passed to threads.  To pass the local number and the mutex to the worker function, a structure must be created.  For example:

The following code is found in <i>illustrations/4_threads/4_thread</i>

test_code.c
```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
  int local_number;
  pthread_mutex_t mutex;
} worker_t;

void *worker(void *arg) {
  worker_t *w = (worker_t *)arg;
  for( int i=0; i<1000000; i++ ) {
    pthread_mutex_lock(&(w->mutex));
    w->local_number++;
    pthread_mutex_unlock(&(w->mutex));
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t threads[10];
  worker_t w;
  w.local_number = 0;
  pthread_mutex_init(&w.mutex, NULL);
  for( int i=0; i<10; i++ )
    pthread_create(&threads[i], NULL, worker, &w);

  for( int i=0; i<10; i++ )
    pthread_join(threads[i], NULL);
  pthread_mutex_destroy(&w.mutex);
  printf( "local_number (should be 10000000)= %d\n", w.local_number );
  return 0;
}
```

When a mutex protects the addition, the local number ends up being what we expected.
```bash
$ gcc -O3 test_code.c -o test_code -lpthread
$ ./test_code
local_number (should be 10000000)= 10000000
```

pthread_mutex_init and pthread_mutex_destroy both take a pointer to a mutex that needs to be initialized and destroyed.  Note that the pointer to the mutex must not be NULL.
```c
pthread_mutex_init(&w.mutex, NULL);
...
pthread_mutex_destroy(&w.mutex);
```

pthread_mutex_lock is like putting up a stop sign and making all of the other threads block or wait.  pthread_mutex_unlock is like taking the stop sign away and allowing the next thread that had a stop sign ready to put its stop sign up.
```c
pthread_mutex_lock(&(w->mutex));
w->local_number++;
pthread_mutex_unlock(&(w->mutex));
```

Note that the worker_t structure (w) is shared amongst all of the threads.

## Timing considerations

Before ending this chapter, we should consider how long each task takes and the advantages and disadvantages of threads and coordination.

```bash
cd $stla/illustrations/4_threads/1_thread
make
time ./test_code
```

outputs
```bash
global_number (should be 10000000)= 2586841

real	0m0.045s
user	0m0.326s
sys	0m0.003s
```

```bash
cd $stla/illustrations/4_threads/4_thread
make
time ./test_code
```

outputs
```bash
local_number (should be 10000000)= 10000000

real	0m0.641s
user	0m0.666s
sys	0m4.836s
```

Coordination takes time.  We can test this code in optimized mode
```bash
cd $stla/illustrations/4_threads/5_thread
make
time ./test_code
```

outputs
```bash
local_number (should be 10000000)= 10000000

real	0m0.652s
user	0m0.686s
sys	0m4.940s
```

The optimized build doesn't improve performance.

Finally, if we look at the last example (6_single_thread), we can see that the performance is much faster.

```bash
cd $stla/illustrations/4_threads/6_single_thread
make
time ./test_code
```

outputs
```bash
local_number (should be 10000000)= 10000000

real	0m0.027s
user	0m0.024s
sys	0m0.002s
```

The code in 6_single_thread is so simple that turning on -O3 may not be a good test as the C compiler might just recognize that the final value should be 10000000.  

It takes 0.027 seconds to do the 10 million additions for a single thread (if the program isn't using threads).  Using threads and mutexes to protect the additions, the process takes almost 5 seconds.  Thread coordination takes time (much like people coordination).

Pthreads also support conditions that are like the pagers above.  They will be described later before needing them.  The next project will require a mutex.

# [Macros](6_macros.md)

[Table of Contents](README.md)  - Copyright 2019 Andy Curtis
