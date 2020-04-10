# Exercise #5: Round-Robin Task Scheduling

This file reports solutions for the exercises that can be found at [www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-5.html](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-5.html). Here you can find a copy of the exercises text.  The solutions have not been reviewed by the creator of the exercises.

## Introduction
For task with equal priority, the Xenomai scheduler supports both FIFO scheduling and round-robin policies. The round-robin policy is similar to a time-sharing operating system runs each active process in turn for its share of time (its "timeslice").

## Objectives
The following are the primary objectives of this exercise:
- To demonstrate the use of  round-robin task scheduling facilities.

## Description 

### FIFO Scheduling
With FIFO scheduling, a task will only preempt a task with a lower priority; it never preempts a task with the same or lower priority. When multiple tasks of the same priority are ready to run, the one which was queued first in the waiting queue of the scheduler, is run first (following FIFO ordering). Only when the first task is ready, the scheduler may execute the second waiting task from the waiting queue.

### Round-Robin Scheduling
A round-robin scheduling algorithm tries to achieve fair scheduling among all ready tasks with the same priority. Without round-robin scheduling, a single task can usurp the processor by never blocking and, hence, never giving other equal priority tasks a chance to run.

Round-robin scheduling achieves fair allocation of the CPU to tasks of the same priority by an approach known as time slicing. Each task executes for a defined interval or time slice; then another task executes for an equal interval, in rotation. The allocation is fair in that no task of a priority group gets a second slice of time before the other tasks of a group are given a slice.

Round-robin scheduling can be enabled inside a task with a call to the following function
```c
rt_task_set_mode(0,XNRRB,NULL);
```
and a call to 
```c
rt_task_slice(NULL,quantum)
```
to set the round robin time slice interval value to quantum clock ticks (in ns). This interval is the amount of time each task is allowed to run before relinquishing the processor to another equal priority task. These functions have to be called only once.

Example: Round-robin Scheduling
In the example below, three tasks with the same priority print their task ids on the console. 
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>
#include <alchemy/timer.h>

#define NTASKS 3
RT_TASK demo_task[NTASKS];
RT_SEM mysync;

#define EXECTIME   2e8   // execution time in ns
#define SPINTIME   1e7   // spin time in ns
   
void demo(void *arg) {
    RTIME starttime, runtime;
    int num=*(int *)arg;
    printf("Task  : %d\n",num);
    rt_sem_p(&mysync,TM_INFINITE);

    // let the task run RUNTIME ns in steps of SPINTIME ns
    runtime = 0;
    while(runtime < EXECTIME) {
        rt_timer_spin(SPINTIME);  // spin cpu doing nothing
        runtime = runtime + SPINTIME;
        printf("Running Task  : %d  at time : %d\n",num,runtime);
        }
    printf("End Task  : %d\n",num);
}
   
//startup code
void startup() {
    int i;
    char  str[20] ;
    // semaphore to sync task startup on
    rt_sem_create(&mysync,"MySemaphore",0,S_FIFO);
    for(i=0; i < NTASKS; i++) {
        printf("start task  : %d\n",i);
        sprintf(str,"task%d",i);
        rt_task_create(&demo_task[i], str, 0, 50, 0);
        rt_task_start(&demo_task[i], &demo, &i);
    }
    printf("wake up all tasks\n");
    rt_sem_broadcast(&mysync);
}

int main(int argc, char* argv[])
{
    startup();
    printf("\nType CTRL-C to end this program\n\n" );
    pause();
}
```

## Exercises

### Exercise 5a.
Download the demo program [ex05a.c](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/ex05a.c) above. Run the program above and describe its behaviour.

### Exercise 5b.
Schedule all three tasks according to the round robin policy, using a time slice of 1e7 ns (10ms). Explain the output.

### Exercise 5c. 
Add a fourth task which is the same as the other three tasks, but has a priority of 80. Describe and explain the output from running the program. Does it make a difference whether task four uses the round-robin policy or not? Explain why.