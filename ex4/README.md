# Exercise #4: Preemptive Priority-Based Scheduling

This file reports solutions forthe exercises that can be found at [www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-4.html](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-4.html). Here you can find a copy of the exercises text.  The solutions have not been reviewed by the creator of the exercises.

## Introduction
The default scheduling policy in Xenomai is preemptive priority-based FIFO scheduling. Hence, execution of a low-priority task is interrupted when a high-priority is ready to execute. When multiple processes of the same priority are ready to run, the default is that the task which was first queued in the waiting queue of the scheduler, is run first (FIFO ordering), and run to its end. In the next exercise we show how this can be changed to round-robin scheduling of tasks with equal priority.

## Objectives
The following are the primary objectives of this exercise:
- To demonstrate the use of Xenomai preemptive priority based task scheduling facilities.

## Description 

### Preemptive Priority-Based Scheduling 

With a preemptive priority-based scheduler, each task has a priority and the kernel insures that the CPU is allocated to the highest priority task that is ready to run. This scheduling method is preemptive in that if a task that has a higher priority than the current task becomes ready to run, the kernel saves the context of the current tasks and switches to the context of the higher priority task. The Xenomai kernel has priority levels between the highest priority 99 and the lowest priority 1.

When creating a task with  `rt_task_create(..)` one of the arguments to is the priority at which the task is to execute.  The priority of a task can be changed by means of the following function:
```c
int rt_task_set_priority(RT_TASK *task, int prio)
```

## Example: Priority-Based Scheduling
The next example program contains three tasks with different priorities. Each task has an execution of  `EXECTIME` ns in which after each SPINTIME ns a message is printed. The CPU is kept busy by the function
```c
void rt_timer_spin(RTIME ns)
```
which is a busy wait burning CPU cycles.
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>
#include <alchemy/timer.h>
   
#define NTASKS 3
#define HIGH 52 /* high priority */
#define MID 51 /* medium priority */
#define LOW 50  /* low priority */

RT_TASK demo_task[NTASKS];
RT_SEM mysync;

#define EXECTIME   2e8   // execution time in ns
#define SPINTIME   1e7   // spin time in ns
   
void demo(void *arg)
{
  RTIME starttime, runtime;
  RT_TASK_INFO taskinfo;
  int num=*(int *)arg;
  printf("Task  : %d\n",num);
  rt_sem_p(&mysync,TM_INFINITE);
  runtime = 0;
  while(runtime < EXECTIME) {
    rt_timer_spin(SPINTIME);  // spin cpu doing nothing
    runtime = runtime + SPINTIME;
    printf("Running Task  : %d at ms : %d\n",num, runtime/1000000);
  }
  printf("End Task  : %d\n",num);
}
   
//startup code
void startup()
{
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
  // assign priorities to tasks
  rt_task_set_priority(&demo_task[0],LOW);
  rt_task_set_priority(&demo_task[1],MID);
  rt_task_set_priority(&demo_task[2],HIGH);
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

### Exercise 4a.

Download example program [ex04a.c](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/ex04a.c) above. Execute it, describe and explain its output.

### Experiment 4b.

Modify the program of 4a such that first task (i.e., task 0) still has the lowest priority, but the other two tasks are running at the same high priority. Compare the output with that of 4a.

### Experiment 4c.
Modify the program of 4a such that the highest-priority task half-way its execution:

first raises the priority of the middle-priority task with 10 and
next immediately also raises the priority of the low-priority task with 10.
Explain the output.

### Experiment 4d.
Modify the program of 4a such that each task in the middle of its execution changes its priority. The first task (i.e., task 0) changes it priority halfway to 40, the second task to 39 and the third to 38. Explain the resulting output.