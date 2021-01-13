// library.h
#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include <iostream>
#include <pthread.h>

const int MAX_THREADS = 16;

// Declarations
int addOne(int number);
int addTwo(int number);
char addChar(char character);
int add2Args(int x, int y);

template<typename T> int farm(T (*worker)(T), int arr_len, int* input_arr, int NUM_THREADS = MAX_THREADS);

template<typename T>
struct thread_data {
    int thread_id;
    T number; // void * args or templates
    T(*worker)(T);
};

template<typename... Args>
int foo(Args... a) {
    return foo2(a);
}

int foo2(int x, int y) {
    return x + y;
}

// Definitions
// Placeholder function for testing
int addOne(int number) {
    printf("Adding one to %d\n", number);
    return number + 1;
}

// Placeholder function for testing
int addTwo(int number) {
    printf("Adding two to %d\n", number);
    return number + 2;
}

// Placeholder function for testing
char addChar(char character) {
    printf("Adding char to %c\n", character);
    return character;
}

int add2Args(int x, int y) {
    printf("Adding with 2 args %d + %d: \n", x, y);
    return x + y;
}

// worker_wrapper function that is called on separate threads
// Calls the provided worker function with the processeed arguments 
template<typename T>
void* worker_wrapper(void* threadarg) {
    thread_data<T>* my_data;
    my_data = (thread_data<T>*) threadarg;
    int tid = my_data->thread_id;
    T number = my_data->number;
    T(*worker)(T) = my_data->worker;
    printf("Working thread: %d, number: %d\n", tid, number);

    int res = (*worker)(number);

    printf("Thread %ld done with res: %d.\n", tid, res);
    pthread_exit(NULL);
}

// Farm function that calls 'worker' function on 'input_array' with length 'arr_len'
template<typename T>
int farm(T(*worker)(T), int arr_len, int* input_arr, int NUM_THREADS) {
    printf("In farm\n");
    //int * result = (int*)malloc(arr_len * sizeof(int));
    //free(result)

    // If there are more threads requested than the array_lenght reduce threads
    if (NUM_THREADS > arr_len) NUM_THREADS = arr_len;

    printf("NUM_THREADS: %d\n", NUM_THREADS);

    /* Initialize array of threads */
    pthread_t threads[NUM_THREADS];

    /* Initialize thread_data_array */
    thread_data<T> thread_data_array[NUM_THREADS];

    /* Initialize and set thread detached attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data_array[t].thread_id = t;
        thread_data_array[t].number = input_arr[t];
        thread_data_array[t].worker = worker;
        rc = pthread_create(&threads[t], &attr, worker_wrapper<T>, (void*)&thread_data_array[t]);
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

#endif
