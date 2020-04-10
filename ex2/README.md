# Exercise #2: Multi-Tasking

This file reports solutions for the exercises that can be found at [www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-2.html](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-2.html). Here you can find a copy of the exercises text.  The solutions have not been reviewed by the creator of the exercises.

## Introduction

Modern real-time systems are based on the complementary concepts of multitasking and intertask communications. A multitasking environment allows real-time applications to be constructed as a set of independent tasks, each with a separate thread of execution and its own set of system resources. The intertask communication facilities allow these tasks to synchronize and coordinate their activities. The Xenomai multitasking scheduler uses interrupt-driven, priority-based preemptive task scheduling. It features fast context switch time and low interrupt latency.

## Objectives

The following are the primary objectives of this exercise:
- To learn how to initiate multiple tasks in one-shot or periodic mode with different priorities using Xenomai tasking routines.

## Description

Multitasking creates the appearance of many concurrently executing tasks wheras, in fact, the kernel interleaves their execution on the basis of a scheduling algorithm. Each task has its own context, which is the CPU environment and system resources that the task sees each time it is scheduled to run by the kernel. On a context switch, the context of a task is saved in the Task Control Block (TCB). The context of a task includes:

- a thread of execution, that is, the task's program counter
- the CPU registers and floating-point registers if necessary
- a stack of dynamic variables and return addresses of function calls
- I/O assignments for standard input, output, error
- a delay timer
- a timeslice timer
- kernel control structures
- signal handlers
- debugging and performance monitoring values

## Time in Xenomai Linux

In Xenomai, time is given by the system clock. 
The system clock can be read by the function `rt_timer_read()`, for instance:
```c
RTIME now; now = rt_timer_read();
```

## Scheduling the Task

A task can be scheduled in two ways.

### one-shot mode

One-shote mode is the default when a task is started by the `'rt_task_start'` function, as introduced in exercise 1.

### periodic mode

- A periodic task is also started by the `'rt_task_start'` function, but it made periodic executing the command:
  ```c
  int rt_task_set_periodic(RT_TASK *task,
                           RTIME start_time,
                           RTIME period);
  ```
  - `'task'` is the descriptor of the affected task. This task is immediately delayed until the first periodic release point is reached. If task is `NULL`, the current task is set periodic.
  - `'start_time'` is the absolute time, expressed in clock ticks ( nanoseconds or jiffies), when the task should begin execution. If `'start_time'` is equal to `TM_NOW`, the current system date is used, and no initial delay takes place.
  - `'period'` is the task's period, in clock ticks, which will be rounded to the nearest multiple of the period of the system timer in periodic mode.
- When a period task has perfomed its functionality, it releases the processor and starts waiting for the next period with the command:
```c
void rt_task_wait_period(NULL);
```
- It is important that timing parameters have time `RTIME` (unsigned long long).
- `rt_task_set_periodic` can be place in demo task (WITH NULL as parameter) or in main (before start of task) but then with task as parameter.

As an example, consider a task function which executes an infinite loop which typically reads some inputs, computes some outputs and waits for the next period. Typical code looks like this:
```c
void task_function(void *arg)
{
  rt_task_set_periodic(NULL, TM_NOW, period_ns);
  while (1) {
    /*
      Do your thing here
     */
    rt_task_wait_period(NULL);
  }
  return;
}
```

## Exercises 

### Exercise 2a.  

Write a program which runs five tasks, e.g. use
```c
#define NTASKS 5
RT_TASK task[NTASKS];
Give all tasks a different name and run them with the same priority in one-shot mode. Each task executes the following function  :
void demo(void *arg) {
    RT_TASK_INFO curtaskinfo;
    rt_task_inquire(NULL,&curtaskinfo);
    rt_printf("Task name: %s ", curtaskinfo.name);
}
```
Execute the program and describe the resulting output.

### Exercise 2b. 
Note that the demo task has an argument arg which is a pointer to type void. An integer argument can be passed as a reference to a task during the call to `rt_task_start()`. For instance, for a variable int index, use
```c
rt_task_start(&taskd.., &demo, &index)
```
Modify the program of exercise 2a such that a unique argument is provided to each of the five tasks. Let the tasks print this number by casting and derefencing the number using, for instance,
```c
int num = * (int *)arg;
```
Again, each task has the same priority. Execute the program and describe its output.

### Exercise 2c. 
Note that the correctness of solutions to 2b might depend on the fast evaluation of the reference by the task. If needed, modify your solution to 2b such that all tasks get a reference to the same variable (which changes value between the start of the tasks). Define
```c
RTIME sec = 1e9;
```
and insert in each task before `int num = * (int *)arg;` a real-time sleep statement
```c
rt_task_sleep(sec);
```
To avoid termination when the main loop stops, end the main loop with the statements:
```c
printf("End program by CTRL-C\n");
pause();
``` 
Execute the program and describe the resulting output.

### Exercise 2d. 
Modify the program of exercise 2b by assigning each task a unique priority such that the task which is started first gets the lowest priority and tasks started later get higher priority. Is there any difference in output? Has the order in which the five tasks print their message changed? If not, explain why.

### Exercise 2e. 
Rewrite the program of exercise 2b into one where three tasks are running periodically with periods of 1, 2 and 3 seconds, respectively. All tasks have the same priority; they should start with an `rt_task_sleep` of 1 second and next execute a loop in which they print a message and wait for the next period.

Similar to exercise 2c, add a `pause()` to avoid termination when main ends.

Explain the resulting output.