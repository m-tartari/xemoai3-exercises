#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>
#include <alchemy/timer.h>

#define NTASKS 3
#define HIGH 52 /* high   priority */
#define MID 51  /* medium priority */
#define LOW 50  /* low    priority */

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
  rt_task_set_priority(&demo_task[1],HIGH);
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