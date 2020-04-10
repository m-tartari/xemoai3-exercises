#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define ITER 10

static RT_TASK  t1;
static RT_TASK  t2;

int global = 0;

void taskOne(void *arg) {
    int i;
    for (i=0; i < ITER; i++) {
        printf("I am taskOne and global = %d................\n", ++global);
    }
}

void taskTwo(void *arg) {
    int i;
    for (i=0; i < ITER; i++) {
        printf("I am taskTwo and global = %d----------------\n", --global);
    }
}

int main(int argc, char* argv[]) {
    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);
    rt_task_start(&t1, &taskOne, 0);
    rt_task_start(&t2, &taskTwo, 0);
    return 0;
} 