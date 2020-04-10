#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>
#include <alchemy/mutex.h>
#include <alchemy/timer.h>

#define HIGH 52 /* high   priority */
#define MED 51  /* medium priority */
#define LOW 50  /* low    priority */

RT_TASK prioLow, prioMedium, prioHigh;
RT_SEM mysync;
RT_MUTEX mymutex;

#define EXECTIME   2e8   // execution time in ns
#define SPINTIME   2e7   // spin time in ns

void low(void *arg)
{
  int num=*(int *)arg;
  printf("Task  : %d\n",num);
  rt_sem_p(&mysync,TM_INFINITE);

  for(int i=0; i<3; i++){
      printf("Low priority task tries to lock mutex\n");
      rt_mutex_acquire(&mymutex, TM_INFINITE);

      printf("Low priority task locks mutex\n");
      if(i==0){
        //printf("Medium task prehempts low task\n");
        rt_task_set_priority(&prioMedium,MED);
      }

      printf("Low priority task unlocks mutex\n");
      rt_mutex_release(&mymutex);
  }
  printf("Task  : %d ended\n",num);
}

void med(void *arg)
{
  int num=*(int *)arg;
  printf("Task  : %d\n", num);
  rt_sem_p(&mysync,TM_INFINITE);

  for(int i=0; i<10; i++){
    if(i==2){
      //printf("High task prehempts low task\n");
      rt_task_set_priority(&prioHigh, HIGH);
    }
    
    rt_timer_spin(SPINTIME);  // spin cpu doing nothing
    printf("Medium task running\n");
  }
  printf("------------------------------------------Medium priority task ends\n");
}

void high(void *arg)
{
  int num=*(int *)arg;
  printf("Task  : %d\n",num);
  rt_sem_p(&mysync,TM_INFINITE);

  for(int i=0; i<3; i++){
    printf("High priority task tries to lock mutex\n");
    rt_mutex_acquire(&mymutex, TM_INFINITE);
    printf("High priority locks mutex\n");

    printf("High priority unlocks mutex\n");
    rt_mutex_release(&mymutex);
  }
  printf("..........................................High priority task ends\n",num);
}

//startup code
void startup()
{
  char  str[20];
  // semaphore to sync task startup on
  rt_sem_create(&mysync,"MySemaphore",0,S_FIFO);
  rt_mutex_create(&mymutex,"MyMutex");

  int i=0;
  rt_task_create(&prioLow, "prioLow", 0, LOW, 0);
  rt_task_start(&prioLow, &low, &i);
  i++;
  rt_task_create(&prioMedium, "prioMedium", 0, LOW, 0);
  rt_task_start(&prioMedium, &med, &i);
  i++;
  rt_task_create(&prioHigh, "prioHigh", 0, LOW, 0);
  rt_task_start(&prioHigh, &high, &i);

  printf("wake up all tasks\n");
  rt_sem_broadcast(&mysync);
}

int main(int argc, char* argv[])
{
  startup();
  printf("\nType CTRL-C to end this program\n\n" );
  pause();
  
  rt_sem_delete(&mysync);
  rt_mutex_delete(&mymutex);
}