#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

void *say_hello(void *i)
{

  printf("hello from %d\n", *((int*)i));
  free(i);
  pthread_exit((void*)NULL);

}

int main()
{

 int i;
 int* param;
 void* thread_status;
 pthread_t *my_threads;


 for(i=0;i<10;i++)
 {
  param = malloc(sizeof(int));
  *param = i;
  pthread_create(&my_threads[i], NULL, say_hello, (void*)param);
 }


 for(i=0;i<10;i++)
 {

  pthread_join(my_threads[i], &thread_status);

 }

 printf("all forked threads have said hello \n");
 printf("hello from master thread\n");
}
