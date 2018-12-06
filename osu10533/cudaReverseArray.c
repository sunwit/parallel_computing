#include<stdio.h>
#include<cuda.h>
#include<stdlib.h>

int main()
{
  int *h_a;
  int *h_v;
  int *d_a;
  int *d_b;
  int dimA=256;
  
  int nblocks = 1;
  int tpb = dima;
  size_t memSize;
  memSize = nblocks*dimA*sizeof(int);
  h_a = (int*) malloc(memSize);
  h_v = (int*) malloc(memSize);
  cudaMalloc((void**)&d_a, memSize);
  cudaMalloc((void**)&d_b, memSize);
  
  for(int i=0;i<dimA;i++)
  {
     h_a[i] = i;
     h_v[i] = i;
  }
  cudaMemcpy(d_a, h_a, memSize, cudaMemcpyHostToDevice);
  dim3 dimGrid(nblocks);
  dim3 dimBlock(tpb);
   
  cudaMemcpy(h_a, d_b, memSize, cudaMemcpyDeviceToHost);
  
  bool good_results = true;
  for(int i=0;i<dimA && good_results;i++)
  {
     if(h_a[dimA-1-i]!=i)
     {
       cerr<<"oops"<<endl;
       good_results=false;
     }
  }

}
