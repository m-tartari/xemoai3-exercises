#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define ITER 10
#define SEM_INIT 1       /* Initial semaphore count */
#define SEM_MODE S_FIFO  /* Wait by FIFO order */

static RT_TASK  t1;
static RT_TASK  t2;
RT_SEM sem1;
RT_SEM sem2;

int global = 0;
// Define semaphores

void taskOne(void *arg) {
  int i;
  
  for (i=0; i < ITER; i++) {
    printf("1----- wait    sem-1\n");
    rt_sem_p(&sem1, TM_INFINITE);
    printf("1----- found   sem-1\n");
    printf("I am taskOne and global = %d................\n", ++global);
    printf("1----- setting sem2-\n");
    rt_sem_v(&sem2);
    printf("1----- set     sem2-\n");
  }
}

void taskTwo(void *arg) {
  int i;
  
  for (i=0; i < ITER; i++) {
    printf("-2---- wait    sem2-\n");
    rt_sem_p(&sem2, TM_INFINITE);
    printf("-2---- found   sem2-\n");
    printf("I am taskTwo and global = %d----------------\n", --global);
    printf("-2---- setting sem-1\n");
    rt_sem_v(&sem1);
    printf("-2---- set     sem-1\n");
  }
}

int main(int argc, char* argv[]) {
  // int rt_sem_create(RT_SEM *sem, const char *name, unsigned long icount, int mode)
  rt_sem_create(&sem1, "semaphore1", 0, SEM_MODE);
  rt_sem_create(&sem2, "semaphore2", 0, SEM_MODE);
  
  rt_task_create(&t1, "task1", 0, 1, 0);
  rt_task_create(&t2, "task2", 0, 1, 0);
  rt_task_start(&t1, &taskOne, 0);
  rt_task_start(&t2, &taskTwo, 0);
  
  //start sem1
  rt_sem_v(&sem1);
  rt_sem_broadcast(&sem1);

  rt_sem_delete(&sem1);
  rt_sem_delete(&sem2);
  return 0;
} 