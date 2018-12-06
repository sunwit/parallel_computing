#include<stdio.h>

__global__ void reverseArrayBlock(int *inArray, int *outArray) 
{
   outArray[blockDim.x-1-threadIdx.x] = inArray[threadIdx.x];
}

int main()
{
  int *h_a;
  int *d_a;
  int *d_b;
  int dimA=256;
  int nblocks = 1;
  int tpb = dimA;
  size_t memSize;
  memSize = nblocks*tpb*sizeof(int);
  h_a = (int*) malloc(memSize);
  cudaMalloc((void**)&d_a, memSize);
  cudaMalloc((void**)&d_b, memSize);
  
  for(int i=0;i<dimA;i++)
  {
     h_a[i] = i;
  }
  cudaMemcpy(d_a, h_a, memSize, cudaMemcpyHostToDevice);
  dim3 dimGrid(nblocks);
  dim3 dimBlock(tpb);
  reverseArrayBlock<<<dimGrid, dimBlock>>>(d_a, d_b);  

  cudaMemcpy(h_a, d_b, memSize, cudaMemcpyDeviceToHost);
  
  bool good_results = true;
  for(int i=0;(i<dimA)&& good_results;i++)
  {
     if(h_a[dimA-1-i]!=i)
     {
       printf("%s\n", "oops");
       good_results=false;
     }
     else
     {
       printf("yes \n");
     }
  }

}
