#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>

#define NTASKS 5
RT_TASK task[NTASKS];

// function to be executed by task
void demo(void *arg) {
  // read arg as integer
  int num = * (int *)arg;

  //get task info
  RT_TASK_INFO curtaskinfo;
  rt_task_inquire(NULL,&curtaskinfo);

  //display info
  rt_printf("%d - Task name: %s\n", num, curtaskinfo.name);
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
}