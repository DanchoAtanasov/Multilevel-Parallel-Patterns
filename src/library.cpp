// library.cpp

#include "library.h"
#include <iostream>
#include <pthread.h>

#define NUM_THREADS 5

typedef struct thread_data_ {
    int thread_id;
    int number;
    int (*worker)(int);
} thread_data;

thread_data thread_data_array[NUM_THREADS];

int addOne(int number){
    printf("Adding one to %d\n", number);
    return number + 1;
}

int addTwo(int number){
    printf("Adding two to %d\n", number);
    return number + 2;
}

void* work(void* threadarg) {
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

int farm(int (*worker)(int), int arr_len, int* input_arr) {
    printf("In farm\n");
    //int * result = (int*)malloc(arr_len * sizeof(int));
    
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;

    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data_array[t].thread_id = t;
        thread_data_array[t].number = input_arr[t];
        thread_data_array[t].worker = worker;
        rc = pthread_create(&threads[t], &attr, work, (void*)&thread_data_array[t]);
        //rc = pthread_create(&threads[t], &attr, (void* (*)(void*))worker, NULL);
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
    pthread_exit(NULL);

    return 1;
    //return EXIT_SUCCESS;
    
    /*int result[arr_len];
    for (int i = 0; i < arr_len; i++) {
        result[i] = (*worker)(input_arr[i]);
    }
    for (int i = 0; i < arr_len; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");
    //free(result);

    return 1;*/
}

