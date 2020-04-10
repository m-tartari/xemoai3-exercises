# Exercise #6: Priority Inversion

This file reports solutions for the exercises that can be found at [www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-6.html](http://www.cs.ru.nl/J.Hooman/DES/XenomaiExercises/Exercise-6.html). Here you can find a copy of the exercises text.  The solutions have not been reviewed by the creator of the exercises.

## Introduction

Priority inversion occurs when a higher-priority task is forced to wait an indefinite period for the completion of a lower priority task. For example, suppose prioHigh, prioMedium, and prioLow are tasks of high, medium, and low priority, respectively. prioLow has acquired a resource by taking its associated binary semaphore. When prioHigh preempts prioLow and contends for the resource by taking the same semaphore, it becomes blocked. If prioHigh would be blocked no longer than the time it normally takes prioLow to finish with the resource, there would be no problem, because the resource cannot be preempted. However, the low priority task is vulnerable to preemption by the medium priority task, prioMedium, which could prevent prioLow from relinquishing the resource. This condition could persist, blocking prioHigh for an extensive period of time.

To address the problem of priority inversion, Xenomai provides a so-called mutex which always enforces a priority inheritance protocol in order to solve priority inversions. Thus when this mutex is used, a priority inheritance algorithm is used. This algorithm insures that the task that owns a resource protected by a mutex always executes at the priority of the highest priority task blocked on that mutex. When the resource is no longer needed, the task relinquishes the mutex and returns to its normal priority. Therefore, the inheriting task is protected from preemption by a task with an intermediate priority. For more details, see the Mutex services of the Alchemy API.

## Objectives

The following are the primary objectives of this exercise:
- To demonstrate Xenomai's priority inversion avoidance mechanisms.

## Exercises

### Exercise 6a.

Write a program in which priority inversion takes place. Use in the program three tasks  with the following behaviour:
- prioLow task locks a semaphore.
- prioLow task gets preempted by prioMedium task which runs for a long time.
- prioHigh task preempts prioMedium task and tries to lock the semaphore which is currently locked by prioLow.
- Since both prioLow and prioHigh are both blocked, prioMedium runs to completion (which takes a long time). 

The output of the program should look like:
```
Low priority task locks semaphore
Medium task running
Medium task running
High priority task tries to lock semaphore
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
------------------------------------------Medium priority task ends
Low priority task unlocks semaphore
High priority task locks semaphore
High priority task unlocks semaphore
High priority task tries to lock semaphore
High priority task locks semaphore
High priority task unlocks semaphore
High priority task tries to lock semaphore
High priority task locks semaphore
High priority task unlocks semaphore
..........................................High priority task ends
Low priority task locks semaphore
Low priority task unlocks semaphore
Low priority task locks semaphore
Low priority task unlocks semaphore
..........................................Low priority task ends
```

### Exercise 6b.

Modify the program of 6a such that the problem with priority inversion is eliminated and the output of the program looks like the following:
```
Low priority task locks mutex
Medium task running
Medium task running
High priority task tries to lock mutex
Low priority task unlocks mutex
High priority task locks mutex
High priority task unlocks mutex
High priority task tries to lock mutex
High priority task locks mutex
High priority task unlocks mutex
High priority task tries to lock mutex
High priority task locks mutex
High priority task unlocks mutex
..........................................High priority task ends
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
Medium task running
------------------------------------------Medium priority task ends
Low priority task locks mutex
Low priority task unlocks mutex
Low priority task locks mutex
Low priority task unlocks mutex
..........................................Low priority task ends
```

Describe and explain your solution.