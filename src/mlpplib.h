// mlpplib.h
#ifndef _MLPPLIB_H_
#define _MLPPLIB_H_

//#include <iostream>
#include <pthread.h>
#include <tuple>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

const int MAX_THREADS = 16;

// MPI variables
int numtasks, rank, sendcount, recvcount, source;

MPI_Request reqs[1];
MPI_Status stats[1];

// pthread code
template<typename R, typename... Args, typename... AArgs>
int farm(int NUM_THREADS, int input_len, R(*worker)(Args...), AArgs... args);

template<typename R, typename... Args>
struct thread_data {
    int thread_id;
    R(*worker)(Args...);
    std::tuple<Args...> args;

    // The following 3 functions convert from tuple to parameter pack
    // Code adapted from https://ideone.com/Nj6rGb
    template <std::size_t... Is>
    R func(std::tuple<Args...>& tup, std::index_sequence<Is...>)
    {
        return (*worker)(std::get<Is>(tup)...);
    }

    R func(std::tuple<Args...>& tup)
    {
        return func(tup, std::index_sequence_for<Args...>{});
    }

    R run_worker() {
        return func(args);
    }
};

// Implementations

// worker_wrapper function that is called on separate threads
// Calls the provided worker function with the processeed arguments 
template<typename R, typename... Args>
void* worker_wrapper(void* threadarg) {
    thread_data<R, Args...>* my_data = (thread_data<R, Args...>*) threadarg;
    int tid = my_data->thread_id;
    printf("rank %d -> Working thread: %d\n", rank, tid);

    R res = my_data->run_worker();

    printf("rank %d -> Thread %ld done with res: %d.\n", rank, tid, res);
    pthread_exit(NULL);
}

// Farm function that calls 'worker' function on 'input_array' with length 'input_len'
template<typename R, typename... Args, typename... AArgs>
int farm(int NUM_THREADS, int input_len, R(*worker)(Args...), AArgs... args) {
    printf("rank %d -> In farm with num args: %d\n", rank, sizeof...(AArgs));
    //printf("NUM_THREADS: %d, input_len: %d\n", NUM_THREADS, input_len);

    //int * result = (int*)malloc(input_len * sizeof(int));
    //free(result)

    // If there are more threads requested than the array_lenght reduce threads
    if (NUM_THREADS > input_len) NUM_THREADS = input_len;

    printf("rank %d -> NUM_THREADS: %d\n", rank, NUM_THREADS);

    // Initialize array of threads 
    pthread_t threads[NUM_THREADS];

    // Initialize thread_data_array 
    thread_data<R, Args...> thread_data_array[NUM_THREADS];

    // Initialize and set thread detached attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int rc;
    int batch_size = (input_len / numtasks) / NUM_THREADS;
    //int start = rank * batch_size;
    //int end = (rank + 1) * batch_size;
    //if (rank == numtasks - 1) end = input_len;
    //int _batch_size = batch_size / NUM_THREADS;
    for (int t = 0; t < NUM_THREADS; t++) {
        int start = t * batch_size;
        int end = (t + 1) * batch_size;
        //int start = t * batch_size;
        //int end = start + batch_size;
        if (t == NUM_THREADS - 1) end = input_len / NUM_THREADS + input_len % NUM_THREADS; // add remainder
        printf("rank %d -> new thread with t:%d, start:%d, end:%d\n", rank, t, start, end);
        thread_data_array[t].thread_id = t;
        thread_data_array[t].worker = worker;
        thread_data_array[t].args = std::tuple<Args...>(start, end, args...);

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
        printf("rank %d -> Completed join with thread %ld\n", rank, t);
    }

    // Last thing that main() should do
    //pthread_exit(NULL);  // TODO put this in main

    return 1;  // TODO Change the return type
}

// MPI code
void init() {
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
}

void load(void(*func)()) {
    if (rank == 0) {
        printf("rank %d -> In load\n", rank);
        (*func)();
    }
}

template <typename T, size_t size, size_t size2>
void scatter(T (&matrix)[size], int SIZE, T (&submatrix)[size2]) {
    printf("rank %d -> In scatter\n", rank);
    //const int SUBMATRIX_TOTAL_SIZE = SIZE / numtasks;

    // Scattering matrix1 to all nodes in chunks
    MPI_Scatter(matrix, size2, MPI_FLOAT, submatrix, size2,
        MPI_FLOAT, 0, MPI_COMM_WORLD);
    /*MPI_Scatter(matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, submatrix, SUBMATRIX_TOTAL_SIZE,
        MPI_FLOAT, 0, MPI_COMM_WORLD);*/
}

//void scatter(float matrix[][8], int SIZE, float submatrix[][8]) {
//    printf("rank %d -> In scatter\n", rank);
//    const int SUBMATRIX_TOTAL_SIZE = SIZE / numtasks;
//
//    // Scattering matrix1 to all nodes in chunks
//    MPI_Scatter(matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, submatrix, SUBMATRIX_TOTAL_SIZE,
//        MPI_FLOAT, 0, MPI_COMM_WORLD);
//}

template <typename T, size_t size>
void broadcast(T(&matrix)[size], int SIZE) {
    printf("rank %d -> In broadcast\n", rank);
    // Broadcasting the whole matrix2 to all nodes
    MPI_Bcast(matrix, SIZE, MPI_FLOAT,
        0, MPI_COMM_WORLD);
}

void gather(float result_matrix[][8], int SIZE, float matrix[][8]) {
    printf("rank %d -> In gather\n", rank);
    const int SUBMATRIX_TOTAL_SIZE = SIZE / numtasks;

    // Gather results from all nodes to main node
    MPI_Igather(result_matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, matrix, SUBMATRIX_TOTAL_SIZE,
        MPI_FLOAT, source, MPI_COMM_WORLD, &reqs[0]);

    if (rank == 0) {
        MPI_Wait(&reqs[0], &stats[0]);
        printf("rank %d -> Finished gathering\n", rank);
    }
}

void finish() {
    printf("rank %d -> Finished exectuion.\n", rank);
    MPI_Finalize();
    
    if (rank != 0) {
        printf("rank %d -> Exiting not-main process\n", rank);
        exit(0);
    }

}

#endif
