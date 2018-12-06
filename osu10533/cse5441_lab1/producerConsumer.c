/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 1
   Summary: Emulating producer and consumer problem serially
   Compilation: icc programConsumer.c transform.o
   Submission: AU18
*/

#include<stdio.h>
#include<string.h>
#include<inttypes.h>
#include<stdlib.h>
#include<time.h>

struct work_entry
{
   char cmd;
   uint16_t key;
};
struct work_entry work_queue[5];
int lq = 0;


uint16_t transformA(uint16_t x);
uint16_t transformB(uint16_t x);
uint16_t transformC(uint16_t x);
uint16_t transformD(uint16_t x);


// producer to produce the data based on transform function
int producer(uint16_t ke,  char cm, int lqen)
{
  work_queue[lqen].cmd = cm;       
  if(cm=='A') work_queue[lqen].key=transformA(ke);
  else if(cm=='B') work_queue[lqen].key=transformB(ke);
  else if(cm=='C') work_queue[lqen].key=transformC(ke);
  else if(cm=='D') work_queue[lqen].key=transformD(ke);
  return lqen + 1;
}

// consumer to consume the data in queue and print out all the data
int consumer(int lqen)
{
   for(int i=0;i<lqen;i++)
   {
      printf("%d ", i);
      printf("%c ", work_queue[i].cmd);
      printf("%d ", work_queue[i].key);
      char x = work_queue[i].cmd;
      uint16_t kee;
      if(x=='A') kee=transformA(work_queue[i].key);
      else if(x=='B') kee=transformB(work_queue[i].key);
      else if(x=='C') kee=transformC(work_queue[i].key);
      else if(x=='D') kee=transformD(work_queue[i].key);
      printf("%d\n", kee);
   }
   return 0;
}
 


//read from the file line by line and perform the producer and consumer function
void main(int argc, char *argv[])
{
   FILE *file=fopen(argv[1], "r");
   char line[256];

   //clock(2)
   clock_t consumerClockTime = 0;
   clock_t producerClockTime = 0;
   clock_t cT, pT;
   
   //time(2)
   double consumerTime = 0;
   double producerTime = 0;
   time_t cStart, cEnd, pStart, pEnd;
   

   while(fgets(line, sizeof(line), file))
   { 
    char *token=strtok(line, " \t");
    char cm = *token;
    // consumer starts to consume
    if(*token=='X' || lq==5)
    { 
      time(&cStart);      //time(2) start
      cT = clock();       //clock(2) start
      lq = consumer(lq);  //calling consumer to consume the data in queue
      time(&cEnd);        //time(2) end
      cT = clock() - cT;  //time measured by clock(2)
      consumerClockTime = consumerClockTime + cT; //total time measured by clock(2)
      consumerTime = consumerTime + (double)(difftime(cEnd, cStart)); //total time measured by time(2)
    }
    if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
    {
       token = strtok(NULL, " \t");
       uint16_t ke = atoi(token); 
       // producer starts to produce
       if(ke>=0 && ke<=1000)
       {
         time(&pStart); //time(2) start
         pT=clock();    //clock(2) start
         lq = producer(ke, cm, lq); //calling producer to produce the dtat to the queue
         time(&pEnd);  //time(2) end
         pT=clock()-pT; //time measured by clock(2)
         producerClockTime = producerClockTime + pT; //total time measured by clock(2)
         producerTime = producerTime + (double)(difftime(pEnd, pStart)); //total time measured by time(2)
       }
    }
   }
   fclose(file);

   //printing time measured by clock(2)
   printf("total time of producer and consumer measured by clock(2):\n");
   printf("Consumer takes %d clicks (%f seconds).\n", consumerClockTime, ((float)consumerClockTime)/CLOCKS_PER_SEC);
   printf("Producer takes %d clicks (%f seconds).\n", producerClockTime, ((float)producerClockTime)/CLOCKS_PER_SEC);

 
   //printing time measured by time(2) 
   printf("\ntotal time of producer and consumer measured by time(2):\n");
   printf("Consumer takes %f seconds.\n", consumerTime);
   printf("Producer takes %f seconds.\n", producerTime);
}
