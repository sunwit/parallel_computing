/*
   Name: Wei Sun (sun.1868@osu.edu)
   Class Section: CSE 5441 AU2018(27372) W F 12:45pm - 2:00pm
   Assignment Number: Programming Assignment 5
   Summary: Producer Consumer problem with message passing interface (MPI)
   Submission: AU18
   
   Main Idea: one master process. there are four threads in the master process. 
   Each thread will be corresponding to one remote process. Four remote processes
   will do produce and consume computation. And, the master process will read the 
   original data and print the final result. we use the OpenMP to create the four
   threads. 
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<inttypes.h>
#include<time.h>
#include<mpi.h>
#include<omp.h>

#define BUF_SIZE 5  //queue size is 5

typedef struct work_entry //buffer structure 
{
  char cmd;    
  uint16_t key;
  int nums;
  uint16_t alpha;
  double pWallTime; // used to measure the wall time of producer
  double cWallTime; // used to measure the wall time of consumer
}work;


uint16_t transformA(uint16_t x); //transformA 
uint16_t transformB(uint16_t x); //transformB
uint16_t transformC(uint16_t x); //transformC
uint16_t transformD(uint16_t x); //transformD

int main(int argc, char *argv[])
{
 int rank;
 int tag = 1;
 int total;

 MPI_Request Srequest, Rrequest;
 MPI_Status status;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);

 //we create our own MPI structure 
 const int nitems = 6;
 int blocklengths[6] = {1, 1, 1, 1, 1, 1};
 MPI_Datatype types[6] = {MPI_CHAR, MPI_UINT16_T, MPI_INT, MPI_UINT16_T, MPI_DOUBLE, MPI_DOUBLE};
 MPI_Datatype work_entry_type;
 MPI_Aint offsets[6];
 offsets[0] = offsetof(work, cmd);
 offsets[1] = offsetof(work, key);
 offsets[2] = offsetof(work, nums);
 offsets[3] = offsetof(work, alpha);
 offsets[4] = offsetof(work, pWallTime);
 offsets[5] = offsetof(work, cWallTime);


 MPI_Type_create_struct(nitems, blocklengths, offsets, types, &work_entry_type);
 MPI_Type_commit(&work_entry_type);

struct work_entry *data = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
struct work_entry dataA, dataB, dataC, dataD;
struct work_entry inDataA, inDataB, inDataC, inDataD;

int num = 0;
if(rank==0)  //master process
{
  char lines[10];
  for(int i=0;fgets(lines, sizeof(lines), stdin);i++) // reading the data from file 
  {
    char *token = strtok(lines, " \t");
    char cm = *token;
    if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
    {
       token = strtok(NULL, " \t");
       if(atoi(token)>=0 && atoi(token)<=1000)
       {
           if(cm=='A') { data[num].cmd = 'A'; data[num].key = atoi(token);}
           if(cm=='B') { data[num].cmd = 'B'; data[num].key = atoi(token);}
           if(cm=='C') { data[num].cmd = 'C'; data[num].key = atoi(token);}
           if(cm=='D') { data[num].cmd = 'D'; data[num].key = atoi(token);}
           num = num + 1;
       }
    }
  }
// used to measure the wall time of producer and consumer
num = num - num%4;
double sumPtime[4]={0, 0, 0, 0}, sumCtime[4]={0, 0, 0, 0}; 
double sumP=0, sumC=0;

// we use OpenMP to do threading, there are four threads. each thread is corresponding to one remote process. 
#pragma omp parallel num_threads(4)
{
    int t_id = omp_get_thread_num();
    for(int i=t_id;i<num;i+=omp_get_num_threads())
    {
      int idx = i%4;
      if(idx==0) // theread 0 will send and receive data, and printing the result from the remote process
      {
        dataA.cmd = data[i].cmd;
        dataA.key = data[i].key;
        dataA.nums = num/4;
        MPI_Send(&dataA, 1, work_entry_type, 1, tag, MPI_COMM_WORLD);  
        MPI_Recv(&inDataA, 1, work_entry_type, 1, tag, MPI_COMM_WORLD, &status);
        sumPtime[idx] = sumPtime[idx] + inDataA.pWallTime;
        sumCtime[idx] = sumCtime[idx] + inDataA.cWallTime;
        printf("%c %d %d %d\n", inDataA.cmd, dataA.key, inDataA.alpha, inDataA.key);
      }
      else if(idx==1) // thread 1 will send and receive data, and printing the result from the remote process
      {
        dataB.cmd = data[i].cmd; 
        dataB.key = data[i].key;
        dataB.nums = num/4;
        MPI_Send(&dataB, 1, work_entry_type, 2, tag, MPI_COMM_WORLD);  
        MPI_Recv(&inDataB, 1, work_entry_type, 2, tag, MPI_COMM_WORLD, &status);
        sumPtime[idx] = sumPtime[idx] + inDataB.pWallTime;
        sumCtime[idx] = sumCtime[idx] + inDataB.cWallTime;
        printf("%c %d %d %d\n",inDataB.cmd, dataB.key, inDataB.alpha, inDataB.key);
      }
      else if(idx==2) // thread 2 will send and receive data, and printing the result from the remote process
      {
        dataC.cmd = data[i].cmd; 
        dataC.key = data[i].key;
        dataC.nums = num/4;
        MPI_Send(&dataC, 1, work_entry_type, 3, tag, MPI_COMM_WORLD);  
        MPI_Recv(&inDataC, 1, work_entry_type, 3, tag, MPI_COMM_WORLD, &status);
        sumPtime[idx] = sumPtime[idx] + inDataC.pWallTime;
        sumCtime[idx] = sumCtime[idx] + inDataC.cWallTime;
        printf("%c %d %d %d\n",inDataC.cmd, dataC.key, inDataC.alpha, inDataC.key);
      }
      else if(idx==3) // thread 3 will send and receive data, and printing the result from the remote process.
      {
        dataD.cmd = data[i].cmd; 
        dataD.key = data[i].key; 
        dataD.nums = num/4; 
        MPI_Send(&dataD, 1, work_entry_type, 4, tag, MPI_COMM_WORLD);  
        MPI_Recv(&inDataD, 1, work_entry_type, 4, tag, MPI_COMM_WORLD, &status);
        sumPtime[idx] = sumPtime[idx] + inDataD.pWallTime;
        sumCtime[idx] = sumCtime[idx] + inDataD.cWallTime;
        printf("%c %d %d %d\n", inDataD.cmd, dataD.key, inDataD.alpha, inDataD.key);
      }
    }
 }
    for(int k=0;k<4;k++)
    {
      sumP = sumP + sumPtime[k];
      sumC = sumC + sumCtime[k];
    }
    printf("the wall time of producer is: %f\nthe wall time of consumer is: %f\n", sumP, sumC); // printing the wall time  of producer and consumer.

}



if(rank==1) //remote process 1 
{
  int total=0;
  do 
   {
    double pstartTime, pendTime, cstartTime, cendTime;

    MPI_Recv(&dataA, 1, work_entry_type, 0, tag, MPI_COMM_WORLD, &status); // receiving data from the master process 

    pstartTime = MPI_Wtime();
    inDataA.alpha = transformA(dataA.key);
    pendTime = MPI_Wtime();

    cstartTime = MPI_Wtime();
    uint16_t beta = (inDataA.alpha + 1) % 1000;
    inDataA.key = transformA(beta); 
    cendTime = MPI_Wtime();

    inDataA.pWallTime = pendTime - pstartTime; // measuring wall time of producer
    inDataA.cWallTime = cendTime - cstartTime; // measuring wall time of consumer

    inDataA.cmd = dataA.cmd; 
    inDataA.nums = dataA.nums;
    MPI_Send(&inDataA, 1, work_entry_type, 0, tag, MPI_COMM_WORLD); // sending data to the master process
    total = total + 1;
  }while(inDataA.nums>total);
}
if(rank==2) //remote process 2
{
  int total = 0;
  do
   {
    double pstartTime, pendTime, cstartTime, cendTime;

    MPI_Recv(&dataB, 1, work_entry_type, 0, tag, MPI_COMM_WORLD, &status);  // receiving data from the master process

    pstartTime = MPI_Wtime();
    inDataB.alpha = transformB(dataB.key);
    pendTime = MPI_Wtime();

    cstartTime = MPI_Wtime();
    uint16_t beta = (inDataB.alpha + 1) % 1000;
    inDataB.key = transformB(beta);
    cendTime = MPI_Wtime();

    inDataB.pWallTime = pendTime - pstartTime; // measuring the wall time of producer and consumer
    inDataB.cWallTime = cendTime - cstartTime;

    inDataB.cmd = dataB.cmd; 
    inDataB.nums = dataB.nums;
    MPI_Send(&inDataB, 1, work_entry_type, 0, tag, MPI_COMM_WORLD);// sending data to the master process
    total = total + 1;
  }while(inDataB.nums>total);
}
if(rank==3) // remote process 3
{
  int total = 0;
  do 
   {
    double pstartTime, pendTime, cstartTime, cendTime;

    MPI_Recv(&dataC, 1, work_entry_type, 0, tag, MPI_COMM_WORLD, &status); //receiving data from the master process 

    pstartTime = MPI_Wtime();
    inDataC.alpha = transformC(dataC.key);
    pendTime = MPI_Wtime();

    cstartTime = MPI_Wtime();
    uint16_t beta = (inDataC.alpha + 1) % 1000;
    inDataC.key = transformC(beta); 
    cendTime = MPI_Wtime();

    inDataC.pWallTime = pendTime - pstartTime; // measuring the wall time of producer and consumer
    inDataC.cWallTime = cendTime - cstartTime;


    inDataC.cmd = dataC.cmd; 
    inDataC.nums = dataC.nums;
    MPI_Send(&inDataC, 1, work_entry_type, 0, tag, MPI_COMM_WORLD);//sending data  to the master process
    total = total + 1;
  }while(inDataC.nums>total);
}
if(rank==4)// remote process 4
{
  int total = 0;
  do
   {

    double pstartTime, pendTime, cstartTime, cendTime;

    MPI_Recv(&dataD, 1, work_entry_type, 0, tag, MPI_COMM_WORLD, &status); // receiving data from the master process

    pstartTime = MPI_Wtime();
    inDataD.alpha = transformD(dataD.key);
    pendTime = MPI_Wtime();

    cstartTime = MPI_Wtime();
    uint16_t beta = (inDataD.alpha + 1) % 1000;
    inDataD.key = transformD(beta); 
    cendTime = MPI_Wtime();
    
    inDataD.pWallTime = pendTime - pstartTime; // measuring the wall time of producer and consumer
    inDataD.cWallTime = cendTime - cstartTime; 

    inDataD.cmd = dataD.cmd; 
    inDataD.nums = dataD.nums;
    MPI_Send(&inDataD, 1, work_entry_type, 0, tag, MPI_COMM_WORLD);// sending data to the master process
    total = total + 1;
  }while(inDataD.nums>total);
}
MPI_Finalize();
return(0); 
}
