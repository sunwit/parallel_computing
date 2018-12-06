/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 4
   Summary: Producer Consumer problem with GPU programming
   Submission: AU18
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
   char cmd;
   uint16_t pkey;
   uint16_t akey;
};


__device__ uint16_t transformA(uint16_t x); //transformA 
__device__ uint16_t transformB(uint16_t x); //transformB
__device__ uint16_t transformC(uint16_t x); //transformC
__device__ uint16_t transformD(uint16_t x); //transformD


__global__ void transformerPro(struct work_entry *d_a) 
{
     
      int idx = threadIdx.x;
      char cm = d_a[idx].cmd;
      if(cm=='A') d_a[idx].akey=transformA(d_a[idx].pkey);
      else if(cm=='B') d_a[idx].akey=transformB(d_a[idx].pkey);
      else if(cm=='C') d_a[idx].akey=transformC(d_a[idx].pkey);
      else if(cm=='D') d_a[idx].akey=transformD(d_a[idx].pkey);
}

__global__ void transformerCon(struct work_entry* d_a)
{
      int idx = threadIdx.x;
      uint16_t  ke;
      if(d_a[idx].cmd=='A') ke = transformA(d_a[idx].akey);
      else if(d_a[idx].cmd=='B') ke = transformB(d_a[idx].akey);
      else if(d_a[idx].cmd=='C') ke = transformC(d_a[idx].akey);
      else if(d_a[idx].cmd=='D') ke = transformD(d_a[idx].akey);
      d_a[idx].akey = ke;
}

int main()
{
  struct work_entry *data = (struct work_entry*) malloc(10000);
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
          data[num].akey = 0;
          num = num + 1;
        }
     }
  }
  
  struct work_entry *h_a;
  struct work_entry *d_a;

  int dimA = num; 
  int nblocks = 1;
  int tpb = dimA; 
  size_t memSize;

  memSize = nblocks*dimA*sizeof(struct work_entry);
  h_a = (struct work_entry*) malloc(memSize);
  cudaMalloc((void**) &d_a, memSize);

  for(int i=0;i<num;i++)
  {
    h_a[i].cmd = data[i].cmd;
    h_a[i].pkey = data[i].pkey;
    h_a[i].akey = data[i].akey;
  }

  cudaMemcpy(d_a, h_a, memSize, cudaMemcpyHostToDevice);

  dim3 dimGrid(nblocks);
  dim3 dimBlock(tpb);
  
  transformerPro<<<dimGrid, dimBlock>>>(d_a); 

  cudaMemcpy(h_a, d_a, memSize, cudaMemcpyDeviceToHost);


  transformerCon<<<dimGrid, dimBlock>>>(d_a); 


  for(int i=0; i<dimA; i++)
  {
   int idx = i%BUF_SIZE;
   printf("%d %c %d %d\n", idx, h_a[idx].cmd, h_a[idx].pkey, h_a[idx].akey);
  }
 
 }
