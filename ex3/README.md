# Exercise #2: Semaphores

This file reports solutions for the exercises that can be found at [www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-3.html](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-3.html). Here you can find a copy of the exercises text.  The solutions have not been reviewed by the creator of the exercises.

## Introduction
Semaphores are useful to coordinate the activities of multitasking applications. The most obvious way for tasks to communicate is via various shared data structures. Because all tasks inside a single process exist in a single linear address space, sharing data structures between tasks is trivial. Global variables, linear buffers, ring buffers, link lists, and pointers can be referenced directly by code running in a different task context. However, while shared address space simplifies the exchange of data, interlocking access to memory is crucial to avoid data corruption. One of the methods to ensure exclusive access to shared resources is the semaphore mechanism. In addition, semaphores can be used for task synchronization.

## Objectives
The following are the primary objectives of this exercise:
- To get some experience with the use of semaphores in Xenomai.

## Description
The semaphores in Xenomai provide fast intertask communication. Semaphores are the primary means for addressing the requirements of both mutual exclusion and task synchronization. In general we can say: 

- The interface of a semaphore consists of two atomic operations, V and P, which affect an internal counter associated with the semaphore. V comes from the Dutch word "Verhogen" meaning  "increment"; P from the Dutch word "Proberen" meaning "try-to-decrease".
- The 'V' (signal, release, give) operation increments the counter and returns immediately.
- The 'P' operation (wait, acquire, take) decrements the counter and returns immediately, unless the counter is already zero and the operation blocks until another process releases the semaphore. 
- A semaphore in Xenomai is counting semaphore which can be used to guard multiple instances of a resource, this in contrast to a binary semaphore which only uses the values 0 and 1. A binary semaphore is called "mutex" in Xenomai and has a separate API which will be treated in exercise 6.

## Semaphore API

See the semaphore services of the [Alchemy API](https://xenomai.org/documentation/xenomai-3/html/xeno3prm/group__alchemy.html).

Create a counting semaphore:
```c
int rt_sem_create(RT_SEM *sem, const char *name, unsigned long icount, int mode)
```
where icount is the initial value of the counter and mode can, for instance, be `S_FIFO` or `S_PRIO` which makes tasks pend on the semaphore in FIFO order or in priority order, respectively. (See the Xenomai API for more details.)
Take a semaphore using an optional timeout:
```c
int rt_sem_p(RT_SEM *sem, RTIME timeout)
```
Give or signal a semaphore:
```c
int rt_sem_v(RT_SEM *sem)
```
Broadcast a semaphore, i.e., signaling a semaphore, unblocks all tasks waiting on it:
```c
int rt_sem_broadcast(RT_SEM *sem)
```
Delete a semaphore:
```c
int rt_sem_delete(RT_SEM *sem)
```

## Example: Changing a global variable by two tasks
In the example below, two tasks (taskOne and taskTwo), are competing to update the value of a global variable, called "global." The objective of the program is to toggle the value of the global variable (1s and 0s). TaskOne increments the value of "global" and taskTwo decrements the value.
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define ITER 10

static RT_TASK  t1;
static RT_TASK  t2;

int global = 0;

void taskOne(void *arg)
{
    int i;
    for (i=0; i < ITER; i++) {
        printf("I am taskOne and global = %d................\n", ++global);
    }
}

void taskTwo(void *arg)
{
    int i;
    for (i=0; i < ITER; i++) {
        printf("I am taskTwo and global = %d----------------\n", --global);
    }
}

int main(int argc, char* argv[]) {
    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);
    rt_task_start(&t1, &taskOne, 0);
    rt_task_start(&t2, &taskTwo, 0);
    return 0;
} 
```

## Exercise
### Exercise 3a.
Download [ex03a.c](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/ex03a.c) and run the example program above. Describe and explain the resulting output.

### Exercise 3b.
Adapt the program of 3a such that the variable "global" toggles between 0 and 1. Do this by using one or more semaphores (avoid the use of other global variables or time). Describe why your solution works.

Bonus: develop a scalable solution using at most two semaphores which works for multiple instances of `taskOne` and `taskTwo` which does not depend on the start-up sequence of tasks. Explain why your solution is scalable and correct.

### Exercise 3c.
Use semaphore(s) to adapt the program from exercise 2d such that, with the same priority assignment, the tasks are executed in the order of their priority (instead of the order in which they are started). Try two solutions, one which uses `rt_sem_v` and another with `rt_sem_broadcast`, where semaphores are used to signal the start of a task.