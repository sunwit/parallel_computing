#include<mpi.h>
#include<stdio.h>

int main(int argc, char *argv[])
{

  int rank, msg[3];

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank==0)
  {
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
