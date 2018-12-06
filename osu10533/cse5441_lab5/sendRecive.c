#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<inttypes.h>
#include<time.h>
#include<mpi.h>
#include<omp.h>

#define BUF_SIZE 5  //queue size is 5

typedef struct work_entry
{
  char cmd;
  uint16_t key;
}work;




uint16_t transformA(uint16_t x); //transformA 
uint16_t transformB(uint16_t x); //transformB
uint16_t transformC(uint16_t x); //transformC
uint16_t transformD(uint16_t x); //transformD



int main(int argc, char *argv[])
{
 int rank;
 int tag = 1;

 MPI_Request Srequest, Rrequest;
 MPI_Status status;
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);


 const int nitems = 2;
 int blocklengths[2] = {1, 1};
 MPI_Datatype types[2] = {MPI_CHAR, MPI_UINT16_T};
 MPI_Datatype work_entry_type;
 MPI_Aint offsets[2];
 offsets[0] = offsetof(work, cmd);
 offsets[1] = offsetof(work, key);
 MPI_Type_create_struct(nitems, blocklengths, offsets, types, &work_entry_type);
 MPI_Type_commit(&work_entry_type);

 struct work_entry *outDataA = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
 struct work_entry *outDataB = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
 struct work_entry *outDataC = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
 struct work_entry *outDataD = (struct work_entry*) malloc(10000*sizeof(struct work_entry));

 struct work_entry *inDataA = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
 struct work_entry *inDataB = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
 struct work_entry *inDataC = (struct work_entry*) malloc(10000*sizeof(struct work_entry));
 struct work_entry *inDataD = (struct work_entry*) malloc(10000*sizeof(struct work_entry));



  int msg[3];


  if(rank==0)
  {
    int numA = 0, numB = 0, numC = 0, numD = 0;
    char lines[10];
    for(int i=0;fgets(lines, sizeof(lines), stdin);i++)
    {
      char *token = strtok(lines, " \t");
      char cm = *token;
      if(*token=='A' || *token=='B' || *token=='C' || *token=='D')
      {
         token = strtok(NULL, " \t");
         if(atoi(token)>=0 && atoi(token)<=1000)
         {
            if(cm=='A') { outDataA[numA].cmd = 'A'; outDataA[numA].key = atoi(token); numA = numA + 1;}
            if(cm=='B') { outDataB[numB].cmd = 'B'; outDataB[numB].key = atoi(token); numB = numB + 1;}
            if(cm=='C') { outDataC[numC].cmd = 'C'; outDataC[numC].key = atoi(token); numC = numC + 1;}
            if(cm=='D') { outDataD[numD].cmd = 'D'; outDataD[numD].key = atoi(token); numD = numD + 1;}
         }
       }
     }

   msg[0] = 42;
   msg[1] = 33;
   msg[2] = 44;
   MPI_Send(msg, 3, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }
  else if(rank==1)
  {
   MPI_Recv(msg, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
   printf("Recevied %d %d %d\n", msg[0], msg[1], msg[2]);
  }
  MPI_Finalize();
  return(0);

}
