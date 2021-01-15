// library.h
#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include <iostream>
#include <pthread.h>
#include <tuple>

const int MAX_THREADS = 16;

template<typename R, typename... Args> int farm(int NUM_THREADS, int* arr, int arr_len, R(*worker)(Args...), Args... a);

template<typename R, typename... Args>
struct thread_data {
    int thread_id;
    R(*worker)(Args...);
    std::tuple<Args...> args;

    template <std::size_t... Is>
    R func(std::tuple<Args...>& tup, std::index_sequence<Is...>)
    {
        return (*worker)(std::get<Is>(tup)...);
    }

    R func(std::tuple<Args...>& tup)
    {
        return func(tup, std::index_sequence_for<Args...>{});
    }

    R run_worker(){
        return func(args);
    }
};

template<typename R, typename... Args>
int wfoo(int NUM_THREADS, int* arr, int arr_len, R(*worker)(Args...), Args... a){
    printf("WFOO with num args: %d\n", sizeof...(Args));
    //printf("NUM_THREADS: %d, arr_len: %d\n", NUM_THREADS, arr_len);

    thread_data<R, Args...> my_data;
    my_data.thread_id = 1;
    my_data.worker = worker;
    my_data.args = std::tuple<Args...>(a...);

    void* tmp = (void*)&my_data;
    thread_data<R, Args...> * ptr_my_data= (thread_data<R, Args...>*) tmp;

    int res = ptr_my_data->run_worker();

    printf("res is %d\n", res);
    return 1;
}

// Definitions

// worker_wrapper function that is called on separate threads
// Calls the provided worker function with the processeed arguments 
template<typename R, typename... Args>
void* worker_wrapper(void* threadarg) {
    thread_data<R, Args...>* my_data = (thread_data<R, Args...>*) threadarg;
    int tid = my_data->thread_id;
    printf("Working thread: %d\n", tid);

    R res = my_data->run_worker();

    printf("Thread %ld done with res: %d.\n", tid, res);
    pthread_exit(NULL);
}

// Farm function that calls 'worker' function on 'input_array' with length 'arr_len'
template<typename R, typename... Args>
int farm(int NUM_THREADS, int* arr, int arr_len, R(*worker)(Args...), Args... a) {
    printf("In farm with num args: %d\n", sizeof...(Args));
    //printf("NUM_THREADS: %d, arr_len: %d\n", NUM_THREADS, arr_len);

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
        thread_data_array[t].worker = worker;
        thread_data_array[t].args = std::tuple<Args...>(a...);
        
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


#endif
