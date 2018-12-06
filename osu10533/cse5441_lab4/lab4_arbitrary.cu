/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 4
   Summary: Producer Consumer problem with GPU programming
   Submission: AU18

   Main Idea: this is the arbitrary version of producer-consumer with GPU programming. 
   the most time consuming part of producer and consumer is transforms. 
   then, we let GPU to do this most time consuming part. but, as you can see 
   we do these transforms in an arbitary order. so, version 1 is less efficient
   than version 2 which can reorder the transform part for GPU. 
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<inttypes.h>
#include<time.h>

#define BUF_SIZE 5  //queue size is 5

struct work_entry //struct of queue
{
   char cmd;  // save letters
   uint16_t pkey; //save the key before producer transforms it
   uint16_t akey; //save the key after consumer transforms it

};


__device__ uint16_t transformA(uint16_t x); //transformA 
__device__ uint16_t transformB(uint16_t x); //transformB
__device__ uint16_t transformC(uint16_t x); //transformC
__device__ uint16_t transformD(uint16_t x); //transformD


__global__ void transformerPro(struct work_entry *d_a, int Asize) //producer gets started  in gpu kernel
{
      for(int step=0; step<Asize; step+=blockDim.x*gridDim.x) //the thread will transform whatever they get in an arbitrary order
      {
         int idx = step+(blockIdx.x*blockDim.x)+threadIdx.x;
         if(d_a[idx].cmd=='A') d_a[idx].akey = transformA(d_a[idx].pkey);
         else if(d_a[idx].cmd=='B') d_a[idx].akey = transformB(d_a[idx].pkey);
         else if(d_a[idx].cmd=='C') d_a[idx].akey = transformC(d_a[idx].pkey);
         else if(d_a[idx].cmd=='D') d_a[idx].akey = transformD(d_a[idx].pkey);
      }

} 

__global__ void transformerCon(struct work_entry *d_a, int Asize) //consumer gets started in gpu kernel
{
      for(int step=0; step<Asize;step+=blockDim.x*gridDim.x) //the thread will transform whatever they get in an arbitrary order
      {
        int idx = step+(blockIdx.x*blockDim.x)+threadIdx.x;
        uint16_t  ke;
        if(d_a[idx].cmd=='A') ke = transformA(d_a[idx].akey);
        else if(d_a[idx].cmd=='B') ke = transformB(d_a[idx].akey);
        else if(d_a[idx].cmd=='C') ke = transformC(d_a[idx].akey);
        else if(d_a[idx].cmd=='D') ke = transformD(d_a[idx].akey);
        d_a[idx].pkey = ke;
      }
}

int main()
{
  double producerTime = 0; //producer's time
  double consumerTime = 0; //consumer's time
  clock_t producerClock = 0; //producer's clock
  clock_t consumerClock = 0; //consumer's clock

  
  struct work_entry *h_a; //host memory 
  struct work_entry *d_a; //device memory 


  struct work_entry *data = (struct work_entry*) malloc(10000*sizeof(struct work_entry));  // read data from the file
  char lines[10];
  int num = 0;
  for(int i=0; fgets(lines, sizeof(lines), stdin); i++) 
  {
     char *token=strtok(lines, " \t");
     char cm = *token;
     if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
     {
        token = strtok(NULL, " \t");
        if(atoi(token)>=0 && atoi(token)<=1000)
        {
          data[num].cmd = cm;
          data[num].pkey = atoi(token);
          num = num + 1;
        }
     }
  }
 

  //threads hierachy 
  int nblocks = 1;
  int tpb = 1024; 
  size_t memSize;

  memSize = num*sizeof(struct work_entry);
  h_a = (struct work_entry*) malloc(memSize); //allocate memory for host

  cudaMalloc((void**) &d_a, memSize); //alocate memory for device


  for(int i=0;i<num;i++)  //input data to the host memory 
  {
    
    h_a[i].cmd = data[i].cmd;
    h_a[i].pkey = data[i].pkey;
  }
  cudaMemcpy(d_a, h_a, memSize, cudaMemcpyHostToDevice); //copy input data to device

  dim3 dimGrid(nblocks);
  dim3 dimBlock(tpb);

  // time and clock calculation for producer
  clock_t  pT;
  time_t  pStart, pEnd;
  time(&pStart);
  pT = clock();
  //launch the gpu kerenel to process data for producer                            
  transformerPro<<<dimGrid, dimBlock>>>(d_a, num); // the most time conumming part in producer side
  cudaMemcpy(h_a, d_a, memSize, cudaMemcpyDeviceToHost); //copy back the data from gpu kernel to host memory


  time(&pEnd);
  pT = clock()-pT;
  producerTime = producerTime + difftime(pEnd, pStart);
  producerClock = producerClock + pT;

  // time and clock calculation for consumer
  clock_t  cT;
  time_t  cStart, cEnd;
  time(&cStart);
  cT = clock();
  //lauch the gpu kernel to process data for consumer
  transformerCon<<<dimGrid, dimBlock>>>(d_a, num); //the most time consumming part in consumer side 
               
  cudaMemcpy(h_a, d_a, memSize, cudaMemcpyDeviceToHost); //copy back the data from the device to host 
  for(int i=0;i<num;i++) //consumer prints the result 
  {
    int idx = i % BUF_SIZE;
    printf("%d %c %d %d\n", idx, h_a[i].cmd, h_a[i].pkey, h_a[i].akey);
  }
  time(&cEnd);
  cT = clock()-cT;
  consumerTime = consumerTime + difftime(cEnd, cStart);
  consumerClock = consumerClock + cT;
 //output clock the producer thread and consumer thread take
 printf("total time of producer and consumer measured by clock(2):\n");
 printf("producer takes %d clicks %f seconds.\n", producerClock, (float)(producerClock)/CLOCKS_PER_SEC);
 printf("consumer takes %d clicks %f seconds.\n", consumerClock, (float)(consumerClock)/CLOCKS_PER_SEC);
 //output time the producer thread and consumer thread take
 printf("total time of producer and consumer measured by time(2):\n");
 printf("producer takes %f seconds.\n", producerTime);
 printf("consumer takes %f seconds.\n", consumerTime);
 }
