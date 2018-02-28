/* RYAN EDELSTEIN, HYUNGHO SEO
   I pledge my honor that I have abided by the Stevens Honor System.
   Task 2
*/

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h> 
#include <errno.h>

#define NUMP 5
#define MAXATT 4 // max number of philsophers allowed to attempt to eat at the same time

int attempts; // number of ongoing attempts

pthread_mutex_t fork_mutex[NUMP];
pthread_mutex_t attempt_mutex; // mutex for modifying the number of attempts 
pthread_cond_t eat_cond; // condition variable for blocking the thread in case MAXATT is exceeded

int main()  
{
  int i;
  pthread_t diner_thread[NUMP]; 
  int dn[NUMP];
  void *diner();

  attempts = 0; // initialize the number of attempts

  pthread_mutex_init(&attempt_mutex, NULL); // initialize the mutex
  pthread_cond_init(&eat_cond, NULL); // initialize the condition variable

  for (i=0;i<NUMP;i++)
    pthread_mutex_init(&fork_mutex[i], NULL);

  for (i=0;i<NUMP;i++){
    dn[i] = i;
    pthread_create(&diner_thread[i],NULL,diner,&dn[i]);
  }
  for (i=0;i<NUMP;i++)
    pthread_join(diner_thread[i],NULL);

  for (i=0;i<NUMP;i++)
    pthread_mutex_destroy(&fork_mutex[i]);
  
  pthread_mutex_destroy(&attempt_mutex); // destroy the mutex
  pthread_cond_destroy(&eat_cond); // initialize the mutex

  pthread_exit(0);

}

void *diner(int *i)
{
  int v;
  int eating = 0;
  printf("I'm diner %d\n",*i);
  v = *i;
  while (eating < 5) {
    printf("%d is thinking\n", v);
    sleep( v/2);
    printf("%d is hungry\n", v);

    /* lock the mutex and wait on &eat_cond when the number of ongoing attempts
       is greater than the max allowed */
    pthread_mutex_lock(&attempt_mutex);
    attempts++; // increment the number of attempts
    printf("#ATTEMPTS: %d\n", attempts);
    while (attempts > MAXATT) {
      printf("WAITING: %d\n",v);
      pthread_cond_wait(&eat_cond, &attempt_mutex);
    }
    pthread_mutex_unlock(&attempt_mutex); // unlock the mutex

    pthread_mutex_lock(&fork_mutex[v]);
    pthread_mutex_lock(&fork_mutex[(v+1)%NUMP]);
    
    /* lock the mutex and decrement the number of attempts */
    pthread_mutex_lock(&attempt_mutex);
    attempts--;
    pthread_mutex_unlock(&attempt_mutex);

    pthread_cond_signal(&eat_cond); // signal the condition variable

    printf("%d is eating\n", v);
    eating++;
    sleep(1);
    printf("%d is done eating\n", v);

    pthread_mutex_unlock(&fork_mutex[v]);
    pthread_mutex_unlock(&fork_mutex[(v+1)%NUMP]);
  }
  pthread_exit(NULL);
}
