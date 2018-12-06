/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 4
   Summary: Producer Consumer problem with GPU programming
   Submission: AU18
   
   Main Idea: this is version 2 of producer consumer problem with gpu programming.
   comparing with the version 1, in the version 2, we let the gpu kernel to perform 
   the transforms without if-else statements.  In the gpu kernel, using while statement
   we do transformA, transformB, transformC and transformD sequentially.  So, we can 
   improve the performance in comparision to the version 1 because of thread 
   divergence.  
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
      int step = 0;
      int idx = step+(blockIdx.x*blockDim.x)+threadIdx.x;
      while(step<Asize && d_a[idx].cmd=='A')  //doing transformA
      {
        d_a[idx].akey = transformA(d_a[idx].pkey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

      while(step<Asize && d_a[idx].cmd=='B') //doing transformB
      {
        d_a[idx].akey = transformB(d_a[idx].pkey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

      while(step<Asize && d_a[idx].cmd=='C') //doing transformC
      {
        d_a[idx].akey = transformC(d_a[idx].pkey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

      while(step<Asize && d_a[idx].cmd=='D') //doing transformD
      {
        d_a[idx].akey = transformD(d_a[idx].pkey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

} 





__global__ void transformerCon(struct work_entry *d_a, int Asize) //consumer gets started in gpu kernel
{
      int step = 0;
      int idx = step+(blockIdx.x*blockDim.x)+threadIdx.x;
      while(step<Asize && d_a[idx].cmd=='A') //doing transformA
      {
        d_a[idx].pkey = transformA(d_a[idx].akey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

      while(step<Asize && d_a[idx].cmd=='B') //doing transformB
      {
        d_a[idx].pkey = transformB(d_a[idx].akey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

      while(step<Asize && d_a[idx].cmd=='C') //doing transformC
      {
        d_a[idx].pkey = transformC(d_a[idx].akey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
      }

      while(step<Asize && d_a[idx].cmd=='D') //doing transformD
      {
        d_a[idx].pkey = transformD(d_a[idx].akey);
        step = step + (blockDim.x*gridDim.x);
        idx = step + (blockIdx.x*blockDim.x) + threadIdx.x;
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
  int num_a = 0;
  int num_b = 0;
  int num_c = 0;
  int num_d = 0;
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
          if(cm=='A') num_a = num_a + 1;
          if(cm=='B') num_b = num_b + 1;
          if(cm=='C') num_c = num_c + 1;
          if(cm=='D') num_d = num_d + 1;
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
  h_a = (struct work_entry*) malloc(memSize);
  cudaMalloc((void**) &d_a, memSize);


  int ia=0, ib=num_a, ic=ib+num_b, id=ic+num_c;
  for(int i=0;i<num;i++) //pre-processing data into four groups such that gpu kernel can process them conveniently.
  {
     if(data[i].cmd=='A')  //the order of data is going from A, B, C to D.
     {
       h_a[ia].cmd  = 'A';
       h_a[ia].pkey = data[i].pkey; 
       ia = ia + 1;
     }
     else if(data[i].cmd=='B')
     {
       h_a[ib].cmd = 'B';
       h_a[ib].pkey = data[i].pkey; 
       ib = ib + 1;
     }
     else if(data[i].cmd=='C')
     {
       h_a[ic].cmd = 'C';
       h_a[ic].pkey = data[i].pkey; 
       ic = ic + 1;
     }
     else if(data[i].cmd=='D')
     {
       h_a[id].cmd = 'D';
       h_a[id].pkey = data[i].pkey; 
       id = id + 1;
     }
  }

  
  cudaMemcpy(d_a, h_a, memSize, cudaMemcpyHostToDevice); //copy input data to device

  dim3 dimGrid(nblocks);
  dim3 dimBlock(tpb);

  // time and clock calculation for producer
  clock_t  pT;
  time_t  pStart, pEnd;
  time(&pStart);
  pT = clock();
  //launch the gpu kernel for producer to transforming                             
  transformerPro<<<dimGrid, dimBlock>>>(d_a, num); // the most time conumming part in producer side
  cudaMemcpy(h_a, d_a, memSize, cudaMemcpyDeviceToHost);



  time(&pEnd);
  pT = clock()-pT;
  producerTime = producerTime + difftime(pEnd, pStart);
  producerClock = producerClock + pT;

  // time and clock calculation for consumer
  clock_t  cT;
  time_t  cStart, cEnd;
  time(&cStart);
  cT = clock();
  //launch the gpu kernel for consumer to do transforming
  transformerCon<<<dimGrid, dimBlock>>>(d_a, num); //the most time consumming part in consumer side 
  cudaMemcpy(h_a, d_a, memSize, cudaMemcpyDeviceToHost); //copy data from gpu kernel to host



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
