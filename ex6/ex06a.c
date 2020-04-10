#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>
#include <alchemy/timer.h>

#define HIGH 52 /* high   priority */
#define MED 51  /* medium priority */
#define LOW 50  /* low    priority */

RT_TASK prioLow, prioMedium, prioHigh;
RT_SEM mysync, mysync2;

#define EXECTIME   2e8   // execution time in ns
#define SPINTIME   2e7   // spin time in ns

void low(void *arg)
{
  RTIME starttime, runtime;
  RT_TASK_INFO curtaskinfo;
  int num=*(int *)arg;
  runtime = 0;
  rt_task_inquire(NULL,&curtaskinfo);
  printf("Task  : %d\n",num);
  rt_sem_p(&mysync,TM_INFINITE);
  for(int i=0; i<3; i++){
      rt_task_inquire(NULL,&curtaskinfo);
      printf("Low priority task tries to lock semaphore\n");
      rt_sem_p(&mysync2,TM_INFINITE);
      rt_task_inquire(NULL,&curtaskinfo);
      printf("Low priority task locks semaphore\n");
      if(i==0){
        //printf("Medium task prehempts low task\n");
        rt_task_set_priority(&prioMedium,MED);
      }
      rt_task_inquire(NULL,&curtaskinfo);
      printf("Low priority task unlocks semaphore\n");
      rt_sem_v(&mysync2);
  }
  printf("Task  : %d ended\n",num);
}

void med(void *arg)
{
  RTIME starttime, runtime;
  RT_TASK_INFO taskinfo;
  int num=*(int *)arg;
  runtime = 0;
  printf("Task  : %d\n", num);
  rt_sem_p(&mysync,TM_INFINITE);
  for(int i=0; i<10; i++){
    if(i==2){
      rt_task_set_priority(&prioHigh, HIGH);
    }
    
    rt_timer_spin(SPINTIME);  // spin cpu doing nothing
    runtime = runtime + SPINTIME;
    printf("Medium task running\n");
  }
  printf("------------------------------------------Medium priority task ends\n");
}

void high(void *arg)
{
  RTIME starttime, runtime;
  RT_TASK_INFO curtaskinfo;
  int num=*(int *)arg;
  runtime = 0;
  printf("Task  : %d\n",num);
  rt_sem_p(&mysync,TM_INFINITE);

  for(int i=0; i<3; i++){
    printf("High priority task tries to lock semaphore\n");
    rt_sem_p(&mysync2,TM_INFINITE);
    printf("High priority locks semaphore\n");
    printf("High priority unlocks semaphore\n");
    rt_sem_v(&mysync2);
  }
  printf("..........................................High priority task ends\n",num);
}

//startup code
void startup()
{
  char  str[20];
  // semaphore to sync task startup on
  rt_sem_create(&mysync,"MySemaphore",0,S_FIFO);
  rt_sem_create(&mysync2,"MySemaphore2",1,S_PRIO);

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
  rt_sem_delete(&mysync2);
}