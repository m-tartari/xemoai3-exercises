# Exercise #8: Measuring Jitter and Latency

This file reports solutions for the exercises that can be found at [www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-8.html](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-8.html). Here you can find a copy of the exercises text.  The solutions have not been reviewed by the creator of the exercises.

**Note:** This excercise is intended for a real hardware platform, although the program for scheduling measurements could be tried first on VirtualBox

## Introduction

In real-time programming one usually has to guarantee that certain deadlines are always met. Hence, predictability of timing is crucial. In this exercise we investigate a few aspects of predictability by measuring scheduling jitter and interrupt latency.

## Objectives
The primary objectives of this exercise are:
- To get experience with timing measurements
- To investigate predictability and timing characteristics of Xenomai by measure scheduling jitter and interrupt latency.

## Description
Terminology
We start with a definition of a number of terms, using a figure from the presentation Real Time
in Embedded Linux Systems by Petazzoni & Opdenacker:

![task latency](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/tasklatency.jpg)

**Interrupt latency**: the time between the occurrence of the interrupt and the start of the interrupt handler.

**Scheduler latency**: the time elapsed after completion of the handler and before execution the scheduler.

**Scheduling latency, also called task latency:** the time between the occurrence of the interrupt that makes a sleeping task runnable again and the moment the task is resumed.

**Scheduling jitter:** the unwanted variation of the release times of a periodic task. It can be characterized in various ways such as an interval around the desired release time, a maximal deviation from the desired time point, or a standard deviation from the mean value.

## Exercises

### Exercise 8a.
Write a program to collect data about the periodic scheduling of a task on a Raspberry Pi and plot this data.

Approach:
- Write a periodic real-time task which is scheduled every 100 microseconds and which executes 10 000 times. Block the main linux program, e.g., by means of a semaphore.
- At the beginning of each run of the task, read the current time with rt_timer_read (which returns a value of type RTIME) and store this in a global array. 
- After having stored 10 000 values, let the main linux program continue to compute the difference between each pair of successive times (which should be close to 100 000 ns) and store this in another array. For instance, using a function
    ```c
    void calc_time_diffs(RTIME *time_diff) {  ...]}
    ```
    Write the array of time differences to a comma separated file (.csv) of the form:
    ```cvs
    0,value[0]
    1,value[1]
    2,value[2]
    ....
    ```
    For instance, by means of the function
    ```c
    void write_RTIMES(char * filename, unsigned int number_of_values, RTIME *time_values){
        unsigned int n=0;
        FILE *file;
        file = fopen(filename,"w");
        while (n<number_of_values) {
           fprintf(file,"%u,%llu\n",n,time_values[n]);
           n++;
        } 
       fclose(file);
    }
    ```
- Calculate the average of the time differences and print the result.
- Using these functions and `unsigned int nsamples=10000;` the main program has the form
    ```c
    ...
    RTIME *time_diff;
    time_diff = calloc(nsamples, sizeof(RTIME));
    ...
    calc_time_diffs(time_diff);
    write_RTIMES("time_diff.csv",nsamples,time_diff);
    average=calc_average_time(nsamples,time_diff);
    printf("average  %llu\n", average); 
    ...
    ```
- Open the .csv file in a spreadsheet (e.g., in Excel) and plot the data in a graphic (e.g. a scatter chart) showing the measured value on the y-axis and the number of the measurement on the x-axis. Note that the comma in the .csv file should lead to two columns; if this does not work, maybe because of language settings, try replacing the comma by ";".
Note: to avoid the influence of startup effects the results might be improved by omitting the first few values.

### Exercise 8b.

:warning: This exercise requires sone hardware other than the Rhaspberry Pi, which I didn't have at hand at the moment so I cannot provide any solutions :(

The breadbord of exercise 7 and the setup of exercise 7b are used to measure interrupt latency by means of three tasks:
A generator task which periodically toggles the value of GPIO pin 22 between 0 and 1, say 10 000 times with a period of 1 ms. Store the times of changing the pin value in an array, similar to 8a.
A worker task with which does some work with the lowest priority. For instance, using `RTIME USEC = 1000llu;`,
```c
void doWork(void *arg) {
  printf("Start working \n");
  while (1) {
    rt_task_sleep(200*USEC);
    printf(".");  
    rt_timer_spin(500*USEC); 
  }
}
```
A high-priority task which catches interrupts on GPIO pin 24. Store the times when interrupts are received in an array. Make sure interrupts can be catched before the generator task starts generating them.
Let your program calculate the difference between the arrays of generated and catched interrupts. Similar to 8a, caculate the avarage value and print it.
Are the results different when the worker task is omitted?