#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>

#define NTASKS 3
RT_TASK task[NTASKS];
RTIME sec = 1e9;

// function to be executed by task
void demo(void *arg) {
  // read arg as integer
  int num = * (int *)arg;
 
  //get task info
  RT_TASK_INFO curtaskinfo;
  rt_task_inquire(NULL,&curtaskinfo);

  /* int rt_task_set_periodic(RT_TASK *task,
   *                     RTIME start_time,
   *                     RTIME period);
   */
  rt_task_set_periodic(NULL, TM_NOW, sec*(1+num));
  while (1) {

    //display info
    rt_printf("%d - Task name: %s\n", num, curtaskinfo.name);

    // Wait next loop
    rt_task_wait_period(NULL);
  }
  return;
}

int main(int argc, char* argv[])
{
  char  str[10];

  printf("start task\n");

  /* Create task
   * Arguments: &task,
   *            name,
   *            stack size (0=default),
   *            priority,
   *            mode (FPU, start suspended, ...)
   */
  for(int i=0; i<NTASKS; i++){
    sprintf(str,"task %d", i);
    rt_task_create(&task[i], str, 0, 50, 0);
  }
  
  /*  Start task
   * Arguments: &task,
   *            task function,
   *            function argument
   */
  for(int i=0; i<NTASKS; i++)
    rt_task_start(&task[i], &demo, &i);
  
  printf("End program by CTRL-C\n");
  pause();
}