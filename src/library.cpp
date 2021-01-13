// library.cpp

#include "library.h"
#include <iostream>
#include <pthread.h>

//#define NUM_THREADS 5
#define MAX_THREADS 16

typedef struct thread_data_ {
    int thread_id;
    int number; // void * args or templates
    int (*worker)(int);
} thread_data;

// Placeholder function for testing
int addOne(int number){
    printf("Adding one to %d\n", number);
    return number + 1;
}

// Placeholder function for testing
int addTwo(int number){
    printf("Adding two to %d\n", number);
    return number + 2;
}

// worker_wrapper function that is called on separate threads
// Calls the provided worker function with the processeed arguments 
void* worker_wrapper(void* threadarg) {
    thread_data* my_data;
    my_data = (thread_data*)threadarg;
    int tid = my_data->thread_id;
    int number = my_data->number;
    int (*worker)(int) = my_data->worker;
    printf("Working thread: %d, number: %d\n", tid, number);

    int res = (*worker)(number);

    printf("Thread %ld done with res: %d.\n", tid, res);
    pthread_exit(NULL);
}

// Farm function that calls 'worker' function on 'input_array' with length 'arr_len'
int farm(int (*worker)(int), int arr_len, int* input_arr, const int NUM_THREADS = MAX_THREADS) {
    printf("In farm\n");
    //int * result = (int*)malloc(arr_len * sizeof(int));
    //free(result)
    
    /* Initialize array of threads */
    pthread_t threads[NUM_THREADS];

    /* Initialize thread_data_array */
    thread_data thread_data_array[NUM_THREADS];

    /* Initialize and set thread detached attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data_array[t].thread_id = t;
        thread_data_array[t].number = input_arr[t];
        thread_data_array[t].worker = worker;
        rc = pthread_create(&threads[t], &attr, worker_wrapper, (void*)&thread_data_array[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);

    for (int t = 0; t < NUM_THREADS; t++) {
        rc = pthread_join(threads[t], NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld\n", t);
    }

    /* Last thing that main() should do */
    //pthread_exit(NULL);  // TODO put this in main

    return 1;  // TODO Change the return type
}

