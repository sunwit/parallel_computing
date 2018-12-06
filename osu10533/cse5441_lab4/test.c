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
   uint16_t key;
};


/*
__device__ uint16_t transformA(uint16_t x); //transformA 
__device__ uint16_t transformB(uint16_t x); //transformB
__device__ uint16_t transformC(uint16_t x); //transformC
__device__ uint16_t transformD(uint16_t x); //transformD



__global__ void transformerPro(uint16_t* keyWord, char* d_a, int Asize) 
{
    for(int step=0; step<Asize; step+=blockDim.x*gridDim.x)      
    {
      int idx = step+(blockIdx.x*blockDim.x)+threadIdx.x;
      char cm = d_a[idx];
      if(cm=='A') keyWord[idx]=transformA(cm);
      else if(cm=='B') keyWord[idx]=transformB(cm);
      else if(cm=='C') keyWord[idx].key=transformC(cm);
      else if(cm=='D') keyWord[idx].key=transformD(cm);
    }
}

__global__ void transformerCon(uint16_t* h_keyWord, char* d_a, int Asize)
{
    for(int step=0; step<Asize; step+=blockDim.x*gridDim.x) 
    {
      int idx = step+(blockIdx.x*blockDim.x)+threadIdx.x;
      uint16_t  ke;
      if(d_a[idx]=='A') ke = transformA(h_keyWord[idx]);
      else if(d_a[idx]=='B') ke = transformB(h_keyWord[idx]);
      else if(d_a[idx]=='C') ke = transformC(h_keyWord[idx]);
      else if(d_a[idx]=='D') ke = transformD(h_keyWord[idx]);
      h_keyWord[idx] = ke;
    }
}
*/

int main()
{
  struct work_entry *h_a;
  struct work_entry *d_a;

  int dimA = 8*8; 
  int nblocks = 1;
  int tpb = 8;
  size_t memSize;
  /*
  memSize = dimA*sizeof(char);
  h_a = (char*) malloc(memSize);
  cudaMalloc((void**) &d_a, memSize);
  */

  char line[10];
  for(int i=0; fgets(line, sizeof(line), stdin); i++) 
  {
     char *token=strtok(line, " \t");
     char cm = *token;
     if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
     {
        token = strtok(NULL, " \t");
        if(atoi(token)>=0 && atoi(token)<=1000)
        {
          h_a[i].cmd=cm;
          h_a[i].key=atoi(token);
          printf("%c %d \n", h_a[i].cmd, h_a[i].key);
        }
     }
  }
  /*  
  cudaMemcpy(d_a, h_a, memSize, cudaMemcpyHostToDevice);

  dim3 dimGrid(nblocks);
  dim3 dimBlock(tpb);
  
  transformerPro<<<dimGrid, dimBlock>>>(d_keyWord, d_a, dimA); 

  cudaMemcpy(h_keyWord, d_keyWord, memSize_key, cudaMemcpyDeviceToHost);
 
  transformerCon<<<dimGrid, dimBlock>>>(h_keyWord, d_a, dimA);
  for(int i=0; i<dimA; i++)
  {
   int idx = i%BUF_SIZE;
   printf("%d %c %d %d\n", idx, h_a[idx], d_a[idx], h_keyWord[idx]);
  }
  */
  
 }
