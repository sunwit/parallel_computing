/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 3
   Summary: Emulating producer and consumer problem with OpenMP
   Submission: AU18
*/


#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<inttypes.h>
#include<time.h>

#define BUF_SIZE 5  //queue size is 5

struct work_entry //struct of queue
{
   char cmd;
   uint16_t key;
};


typedef struct{    //definition of struct including working  queue
  struct work_entry  work_queue[BUF_SIZE]; //work entry
  size_t flag[BUF_SIZE]; //flag variable
  size_t j;
  double producerTime; //producer's time
  double consumerTime; //consumer's time
  clock_t producerClock; //producer's clock
  clock_t consumerClock; //consumer's clock
}buffer_t;


uint16_t transformA(uint16_t x); //transformA 
uint16_t transformB(uint16_t x); //transformB
uint16_t transformC(uint16_t x); //transformC
uint16_t transformD(uint16_t x); //transformD


void main(int argc, char* argv[])
{
  int nt = atoi(argv[1]);
  buffer_t buffer={ //struct initialization 
    .flag={0},
    .j=0,
    .producerTime = 0,
    .consumerTime = 0,
    .producerClock = 0,
    .consumerClock = 0
 };
  #pragma omp parallel num_threads(nt)
  {
      int t_id = omp_get_thread_num();
      int idx;
      for(int i = t_id=0;!feof(stdin);i+=omp_get_num_threads())
      {
           idx = (buffer.j)%BUF_SIZE;
           if(buffer.flag[idx]==0)
           {
               char line[10];
               if(fgets(line, sizeof(line), stdin)!=NULL) 
               {
                   char *token=strtok(line, " \t");
                   char cm = *token;
                   if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
                   {
                       token = strtok(NULL, " \t");
                       if(atoi(token)>=0 && atoi(token)<=1000)
                       {
                               clock_t  pT;
                               time_t  pStart, pEnd;
                               time(&pStart);
                               pT = clock();
                               buffer.work_queue[idx].cmd = cm;       
                               if(cm=='A') buffer.work_queue[idx].key=transformA(atoi(token));
                               else if(cm=='B') buffer.work_queue[idx].key=transformB(atoi(token));
                               else if(cm=='C') buffer.work_queue[idx].key=transformC(atoi(token));
                               else if(cm=='D') buffer.work_queue[idx].key=transformD(atoi(token));
                               buffer.flag[idx]=1;
                               time(&pEnd);
                               pT = clock()-pT;
                               buffer.producerTime = buffer.producerTime + difftime(pEnd, pStart);
                               buffer.producerClock = buffer.producerClock + pT;
                         }
                     }
                }
             }
             else 
             {
                 uint16_t ke;
                 if(buffer.work_queue[idx].cmd=='A') ke = transformA(buffer.work_queue[idx].key);
                 else if(buffer.work_queue[idx].cmd=='B') ke = transformB(buffer.work_queue[idx].key);
                 else if(buffer.work_queue[idx].cmd=='C') ke = transformC(buffer.work_queue[idx].key);
                 else if(buffer.work_queue[idx].cmd=='D') ke = transformD(buffer.work_queue[idx].key);
                 clock_t  cT;
                 time_t  cStart, cEnd;
                 time(&cStart);
                 cT = clock();
                 printf("%d %c %d %d\n", idx, buffer.work_queue[idx].cmd, buffer.work_queue[idx].key, ke);
                 buffer.flag[idx]=0;
                 time(&cEnd);
                 cT = clock()-cT;
                 buffer.consumerTime = buffer.consumerTime + difftime(cEnd, cStart);
                 buffer.consumerClock = buffer.consumerClock + cT;
             }
         #pragma omp critical
         {
            buffer.j = buffer.j + 1;
         }
         }
    }
 //output clock the producer thread and consumer thread take
 printf("total time of producer and consumer measured by clock(2):\n");
 printf("producer takes %d clicks %f seconds.\n", buffer.producerClock, (float)(buffer.producerClock)/CLOCKS_PER_SEC);
 printf("consumer takes %d clicks %f seconds.\n", buffer.consumerClock, (float)(buffer.consumerClock)/CLOCKS_PER_SEC);
 //output time the producer thread and consumer thread take
 printf("total time of producer and consumer measured by time(2):\n");
 printf("producer takes %f seconds.\n", buffer.producerTime);
 printf("consumer takes %f seconds.\n", buffer.consumerTime);
}
