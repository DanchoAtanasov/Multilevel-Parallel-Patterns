
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include <iostream>

#define NUM_THREADS 10
#define MATRIX_SIZE 1005

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

struct thread_data{
   int thread_id;
   int from;
   int to;
};

struct thread_data thread_data_array[NUM_THREADS];

void printMatrix(){
  for(int i=0; i<MATRIX_SIZE; i++){
    for(int j=0; j<MATRIX_SIZE; j++){
      printf("%f ", matrix1[i][j]);
    }
    printf("\n");
  }
}


void *work(void *threadarg){
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;
	int tid = my_data->thread_id;
	int from = my_data->from;
	int to = my_data->to;
	printf("Working thread %d: %d-%d\n", tid, from, to);
  
	for(int i = from; i < to; i++) {
	  for(int j = 0; j < MATRIX_SIZE; j++) {
	    float c = 0.0f;
	    for(int k = 0; k < MATRIX_SIZE; k++) {
	      c+=matrix1[i][k]*matrix2[k][j];
	    }
	    matrix3[i][j]=c;
	  }
	}

	printf("Thread %ld done.\n",tid);
	pthread_exit(NULL);
}


int main() {

    for(int i = 0; i < MATRIX_SIZE; i++)
      for(int j = 0; j < MATRIX_SIZE; j++) {
          matrix1[i][j] = 2.0f;
          matrix2[i][j] = 3.0f;
        }

    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    int step = MATRIX_SIZE / NUM_THREADS;
    int remainder = MATRIX_SIZE % NUM_THREADS;
    int rc;
    long t;
    
    for(t=0; t<NUM_THREADS; t++){
       printf("In main: creating thread %ld\n", t);
       
       thread_data_array[t].thread_id = t;
       thread_data_array[t].from = t * step; 
       thread_data_array[t].to = (t+1) * step;
       if(t == NUM_THREADS - 1) thread_data_array[t].to += remainder;
       rc = pthread_create(&threads[t], &attr, work, (void *) &thread_data_array[t]);
       if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
       }
    }

    pthread_attr_destroy(&attr);
    for(t=0; t<NUM_THREADS; t++) {
      rc = pthread_join(threads[t], NULL);
      if (rc) {
        printf("ERROR; return code from pthread_join() is %d\n", rc);
        exit(-1);
      }
      printf("Main: completed join with thread %ld\n",t);
    }

    //printMatrix(); 
    printf("All values in matrix are: %.0f\n", matrix3[0][0]);
    
    /* Last thing that main() should do */
    pthread_exit(NULL);
  return EXIT_SUCCESS;
}

