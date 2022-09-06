#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *functionC();
sem_t mutex; // semaphore : data type is sem_t
int  counter = 0;

int main(void)
{
   int rc1, rc2;
   pthread_t thread1, thread2;

   /* initialize semaphore */
   sem_init(&mutex, 0, 1); // 0 means semaphore is shared between threads
                           // and 1 is semaphore value (for one instance of resource)

   /* Create independent threads each of which will execute functionC */

   if( (rc1=pthread_create( &thread1, NULL, &functionC, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }

   if( (rc2=pthread_create( &thread2, NULL, &functionC, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   /* Wait till threads are complete before main continues. Unless we  */
   /* wait we run the risk of executing an exit which will terminate   */
   /* the process and all threads before the threads have completed.   */

   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL); 

   /* semaphore destroy */
   sem_destroy(&mutex);

   return 0;
}

void *functionC()
{
   sem_wait(&mutex);

   counter++;
   printf("Counter value: %d\n",counter);

   sem_post(&mutex);
}


