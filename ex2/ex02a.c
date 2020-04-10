#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>

#define NTASKS 5
RT_TASK task[NTASKS];

// function to be executed by task
void demo(void *arg) {
    RT_TASK_INFO curtaskinfo;
    rt_task_inquire(NULL,&curtaskinfo);
    rt_printf("Task name: %s \n", curtaskinfo.name);
}

int main(int argc, char* argv[])
{
  char  str[10];

  printf("start task\n");
  sprintf(str,"task0");

  /* Create task
   * Arguments: &task,
   *            name,
   *            stack size (0=default),
   *            priority,
   *            mode (FPU, start suspended, ...)
   */
  rt_task_create(&task[0], str, 0, 50, 0);
  sprintf(str,"task1");
  rt_task_create(&task[1], str, 0, 50, 0);
  sprintf(str,"task2");
  rt_task_create(&task[2], str, 0, 50, 0);
  sprintf(str,"task3");
  rt_task_create(&task[3], str, 0, 50, 0);
  sprintf(str,"task4");
  rt_task_create(&task[4], str, 0, 50, 0);

  /*  Start task
   * Arguments: &task,
   *            task function,
   *            function argument
   */
  for(int i=0; i<NTASKS; i++)
    rt_task_start(&task[i], &demo, 0);
}