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

template<typename R, typename... Args>
struct thread_data {
    int thread_id;
    //Args args;
    //T number; // void * args or templates
    R(*worker)(Args...);
};

int foo2(int x, int y) {
    printf("In foo2, %d, %d\n", x, y);
    return x + y;
}
int foo2(int x, int y, int z) {
    printf("In foo2 with z, %d, %d, %d\n", x, y, z);
    return x + y + z;
}
int foo2(int x, float y) {
    printf("Here instead\n");
    return x + y;
}

template<typename... Args>
int foo(Args... a) {
    //printf("YEAH %c", a...);
    //Args... n = a...;
	std::size_t s = sizeof...(Args);

    std::cout << s << std::endl;
    return foo2(a...);
}

template<typename R, typename... Args>
int wfoo(int NUM_THREADS, int* arr, int arr_len, R(*worker)(Args...), Args... a){
    printf("WFOO with num args: %d\n", sizeof...(Args));
    printf("NUM_THREADS: %d, arr_len: %d\n", NUM_THREADS, arr_len);
    int res = (*worker)(a...);
    printf("res is %d\n", res);
    return 1;
}

/*
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
template<typename R, typename... Args>
void* worker_wrapper(void* threadarg) {
    thread_data<Args...>* my_data;
    my_data = (thread_data<Args...>*) threadarg;
    int tid = my_data->thread_id;
    //Args args = my_data->args;
    //T number = my_data->number;
    R(*worker)(Args...) = my_data->worker;
    printf("Working thread: %d\n", tid);

    int res = (*worker)(Args...);

    printf("Thread %ld done with res: %d.\n", tid, res);
    pthread_exit(NULL);
}

// Farm function that calls 'worker' function on 'input_array' with length 'arr_len'
template<typename R, typename ...Args>
int farm(R(*worker)(Args... a), int arr_len, int* input_arr, int NUM_THREADS) {
    printf("In farm\n");
    //int * result = (int*)malloc(arr_len * sizeof(int));
    //free(result)

    // If there are more threads requested than the array_lenght reduce threads
    if (NUM_THREADS > arr_len) NUM_THREADS = arr_len;

    printf("NUM_THREADS: %d\n", NUM_THREADS);

    // Initialize array of threads 
    pthread_t threads[NUM_THREADS];

    // Initialize thread_data_array 
    thread_data<R, Args...> thread_data_array[NUM_THREADS];

    // Initialize and set thread detached attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data_array[t].thread_id = t;
        thread_data_array[t].args = a...;
        //thread_data_array[t].number = input_arr[t];
        thread_data_array[t].worker = worker;
        rc = pthread_create(&threads[t], &attr, worker_wrapper<R, Args...>, (void*)&thread_data_array[t]);
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

    // Last thing that main() should do
    //pthread_exit(NULL);  // TODO put this in main

    return 1;  // TODO Change the return type
}
*/

#endif
