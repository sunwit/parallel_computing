/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 2
   Summary: Emulating producer and consumer problem with pthreads
   Submission: AU18
*/


#include<pthread.h>
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
  pthread_mutex_t mutex; //mutex of critical section
  double producerTime; //producer's time
  double consumerTime; //consumer's time
  clock_t producerClock; //producer's clock
  clock_t consumerClock; //consumer's clock
}buffer_t;


uint16_t transformA(uint16_t x); //transformA 
uint16_t transformB(uint16_t x); //transformB
uint16_t transformC(uint16_t x); //transformC
uint16_t transformD(uint16_t x); //transformD


void* producer(void* arg) //producer function
{ 
  buffer_t *buffer = (buffer_t*)arg; 
  while(!feof(stdin))
  {
    //clock and time
    clock_t  pT;
    time_t  pStart, pEnd;
    time(&pStart);
    pT = clock();
    
    char line[10];
    if(fgets(line, sizeof(line), stdin)!=NULL) //reading line by line from the file
    {
    char *token=strtok(line, " \t");
    char cm = *token;
    if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
    {
      token = strtok(NULL, " \t");
      uint16_t ke = atoi(token); 
      if(ke>=0 && ke<=1000)
      {
         int i=0;
         while(1)
         {
            pthread_mutex_lock(&buffer->mutex); //grabing the mutex to enter critical section
            if(buffer->flag[i]==0)
            {
              pthread_mutex_unlock(&buffer->mutex); //releaseing the mutex, this can gurantee that the producer and consumer can work simultaneously
              clock_t  pT;
              time_t  pStart, pEnd;
              time(&pStart);
              pT = clock();
              buffer->work_queue[i].cmd = cm;       
              if(cm=='A') buffer->work_queue[i].key=transformA(ke);
              else if(cm=='B') buffer->work_queue[i].key=transformB(ke);
              else if(cm=='C') buffer->work_queue[i].key=transformC(ke);
              else if(cm=='D') buffer->work_queue[i].key=transformD(ke);
              buffer->flag[i]=1;
              time(&pEnd);
              pT = clock()-pT;
              buffer->producerTime = buffer->producerTime + difftime(pEnd, pStart);
              buffer->producerClock = buffer->producerClock + pT;
              break;
            }
        else 
        {
        pthread_mutex_unlock(&buffer->mutex); //releasing the mutex, this can gurantee that the producer and consumer can work simultaneously
        int exit = 0;
        for(int j=0;j<BUF_SIZE;j++)
        {
             if(buffer->flag[j]==1)
                exit=1; 
        }
        if(exit==0) break;
        }
         i = (i+1)%BUF_SIZE;
         }
      }
     }
     }
  }
  pthread_exit((void*) NULL); //producer thread dies
}

void* consumer(void* arg) //consumer function
{
  buffer_t *buffer = (buffer_t*) arg;
  while(!feof(stdin))
  {
    int i=0;
    while(1)
    {
    pthread_mutex_lock(&buffer->mutex); //mutex of critical section for consumer 
    if(buffer->flag[i]==1)
    {
      pthread_mutex_unlock(&buffer->mutex); //releasing the mutex, this can gurantee that the producer and consumer can work simltaneously
      clock_t  cT;
      time_t  cStart, cEnd;
      time(&cStart);
      cT = clock();
      printf("%d ", i);
      printf("%c ", buffer->work_queue[i].cmd);
      printf("%d ", buffer->work_queue[i].key);
      char x = buffer->work_queue[i].cmd;
      uint16_t kee;
      if(x=='A') kee=transformA(buffer->work_queue[i].key);
      else if(x=='B') kee=transformB(buffer->work_queue[i].key);
      else if(x=='C') kee=transformC(buffer->work_queue[i].key);
      else if(x=='D') kee=transformD(buffer->work_queue[i].key);
      printf("%d\n", kee);
      buffer->flag[i]=0;
      time(&cEnd);
      cT = clock()-cT;
      buffer->consumerTime = buffer->consumerTime + difftime(cEnd, cStart);
      buffer->consumerClock = buffer->consumerClock + cT;
      pthread_mutex_unlock(&buffer->mutex); //releasing the mutex, this can gurantee that the producer and consumer can work simultanteously
     }
     else
     {
        pthread_mutex_unlock(&buffer->mutex); //releasing the mutex
        int exit = 0;
        for(int j=0;j<BUF_SIZE;j++)
        {
             if(buffer->flag[j]==1)
                exit=1; 
        }
        if(exit==0) break;
     }
     i = (i+1)%BUF_SIZE;
     }
    }
  pthread_exit((void*) NULL); //consumer thread dies
}

void main()
{
  buffer_t buffer={ //struct initialization 
    .flag={0},
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .producerTime = 0,
    .consumerTime = 0,
    .producerClock = 0,
    .consumerClock = 0
 };

 pthread_t prod, cons; //producer thread and consumer thread
 void* thread_status;
 pthread_create(&prod, NULL, producer, (void*) &buffer);//producer thread creation
 pthread_create(&cons, NULL, consumer, (void*) &buffer);//consumer thread creation
 
 pthread_join(prod, &thread_status);//producer thread joins
 pthread_join(cons, &thread_status);//consumer thread joins

 //output clock the producer thread and consumer thread take
 printf("total time of producer and consumer measured by clock(2):\n");
 printf("producer takes %d clicks %f seconds.\n", buffer.producerClock, (float)(buffer.producerClock)/CLOCKS_PER_SEC);
 printf("consumer takes %d clicks %f seconds.\n", buffer.consumerClock, (float)(buffer.consumerClock)/CLOCKS_PER_SEC);
 //output time the producer thread and consumer thread take
 printf("total time of producer and consumer measured by time(2):\n");
 printf("producer takes %f seconds.\n", buffer.producerTime);
 printf("consumer takes %f seconds.\n", buffer.consumerTime);
}

