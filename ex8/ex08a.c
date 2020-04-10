#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <alchemy/task.h>
#include <alchemy/sem.h>
#include <alchemy/timer.h>

#define NLOOPS 10000      // Number of loops priority
#define EXECTIME 1e5    // execution time in ns

RT_TASK loopTime;
RT_SEM mysync;
RTIME runtime[NLOOPS];

//-------------------------------------Tasks------------------------------------
void loop(void *arg)
{
    printf("Task Started\n");
    rt_task_set_periodic(NULL, TM_NOW, EXECTIME);
    for(int i=0; i<NLOOPS; i++){
        //register time
        runtime[i]= rt_timer_read();
        // Wait next loop
        rt_task_wait_period(NULL);
    }
    printf("Task Ended\n");
    rt_sem_v(&mysync);
}

//-------------------------------------Main-------------------------------------
int main(int argc, char* argv[])
{
    // semaphore to sync task startup on
    rt_sem_create(&mysync,"MySemaphore",0,S_FIFO);
    rt_task_create(&loopTime, "loopTask", 0, 50, 0);
    rt_task_start(&loopTime, &loop, NULL);

    // wait semaphore
    rt_sem_p(&mysync,TM_INFINITE);
    RTIME time_diff[NLOOPS];
    
    // calc_time_diffs(&time_diff);
    printf("Elaborating Data\n");
    unsigned int i=1;
    time_diff[0]=0;
    while(i<NLOOPS){
        time_diff[i]=runtime[i]-runtime[i-1];
        i++;
    }

    //write_RTIMES("time_diff.csv",NLOOPS,&time_diff);
    printf("Saving Data\n");
    unsigned int n=0;
    FILE *file;
    file = fopen("time_diff.csv","w");
    while (n<NLOOPS) {
       fprintf(file,"%u, %llu\n",n,time_diff[n]);
       n++;
    } 
    fclose(file);

    // average=calc_average_time(NLOOPS,&time_diff);
    RTIME average=0;
    for(unsigned int i=0; i<NLOOPS; i++)
        average+=time_diff[i];
    average=average/NLOOPS;

    printf("average:  %llu\n", average);
}