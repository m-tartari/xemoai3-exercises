#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define ITER 10
#define SEM_INIT 0       /* Initial semaphore count */
#define SEM_MODE S_PRIO  /* Wait by FIFO order */

#define NTASKS 5
//RT_TASK task[NTASKS];
static RT_TASK  t1[NTASKS];
static RT_TASK  t2[NTASKS];
RT_SEM sem1;
RT_SEM sem2;

int global = 0;
// Define semaphores

void taskOne(void *arg) {
  int instance = * (int *)arg;
  
  for (int i=0; i < ITER; i++) {
    printf("task 1.%d.%d   wait    semaphore 1\n", instance, i);
    rt_sem_p(&sem1, TM_INFINITE);
    //printf("1--%d-- found   sem-1\n", instance);
    printf("................................ task 1.%d.%d and global = %d\n", instance, i, ++global);
    printf("task 1.%d.%d   setting semaphore 2\n", instance, i);
    rt_sem_v(&sem2);                              // validate sem2 but does not run it till it finds a pause
  }
  printf("task 1.%d     ended\n", instance);
}

void taskTwo(void *arg) {
  int instance = * (int *)arg;
  
  for (int i=0; i < ITER; i++) {
    printf("task 2.%d.%d   wait    semaphore 2\n", instance, i);
    rt_sem_p(&sem2, TM_INFINITE);
    
    printf("................................ task 2.%d.%d and global = %d\n", instance, i, --global);
    printf("task 2.%d.%d   setting semaphore 1\n", instance, i);
    rt_sem_v(&sem1);
  }
  printf("task 2.%d     ended\n", instance);
}

int main(int argc, char* argv[]) {
  char  str[10];
  printf("start tasks\n")
  ;
  // int rt_sem_create(RT_SEM *sem, const char *name, unsigned long icount, int mode)
  rt_sem_create(&sem1, "semaphore1", SEM_INIT, SEM_MODE);
  rt_sem_create(&sem2, "semaphore2", SEM_INIT, SEM_MODE);
  
  // create tasks
  for(int i=0; i<NTASKS; i++){
    sprintf(str,"task 1%d", i);
    rt_task_create(&t1[i], str, 0, 1+i, 0);
    sprintf(str,"task 2%d", i);
    rt_task_create(&t2[i], str, 0, 1+i, 0);
  }

  // start tasks
  for(int i=0; i<NTASKS; i++){
    rt_task_start(&t1[i], &taskOne, &i);
    rt_task_start(&t2[i], &taskTwo, &i);
  }
  
  // validate sem1 and start taskOne right away
  rt_sem_broadcast(&sem1);

  // delete semaphors
  rt_sem_delete(&sem1);
  rt_sem_delete(&sem2);
  return 0;
} 