#include <pthread.h>
#include <unistd.h>
#include <iostream>

#define NUM_THREADS  2
#define TCOUNT  1
#define BALANCE_LIMIT 10000

struct thread_data{
  int thread_id;
  int amount;
};

struct thread_data thread_data_array[NUM_THREADS];

int balance = 0;
pthread_mutex_t balance_mutex;
pthread_cond_t balance_threashold_cv;

void *deposit(void *threadarg){
  //long my_id = (long)t;
  std::cout << "In deposit" << std::endl;
  struct thread_data * my_data;
  my_data = (struct thread_data *) threadarg;
  int amount = my_data->amount;
  std::cout << amount << std::endl;

  pthread_mutex_lock(&balance_mutex);
  balance += my_data->amount;
  pthread_mutex_unlock(&balance_mutex);

  pthread_exit(NULL);

}

int main(int argc, char *argv[]){
 
  std::cout << "In main" << std::endl;	
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;

  pthread_mutex_init(&balance_mutex, NULL);
  pthread_cond_init(&balance_threashold_cv, NULL);
    
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  thread_data_array[0].thread_id = 0;
  thread_data_array[0].amount = 100;
  pthread_create(&threads[0], &attr, deposit, (void *)&thread_data_array[0]);
  
  thread_data_array[1].thread_id = 1;
  thread_data_array[1].amount = 150;
  pthread_create(&threads[1], &attr, deposit, (void *)&thread_data_array[1]);

  std::cout << "Waiting for threads to join" << std::endl;
  for (int i = 0; i < NUM_THREADS; i++){
	  pthread_join(threads[i], NULL);
  }
  std::cout << "Threads have joined" << std::endl;
  std::cout << "Balance is " << balance << std::endl;

  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&balance_mutex);
  pthread_cond_destroy(&balance_threashold_cv);
}
