// mlpplib.h
#ifndef _MLPPLIB_H_
#define _MLPPLIB_H_

//#include <iostream>
#include <pthread.h>
#include <tuple>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <vector>

const int kMaxThreads = 16;

// MPI variables
int numTasks;  // Number of MPI processes - nodes running the MPI task
int rank;
const int kSource = 0;  // Task 0 is the main task

// Vectors used for creating custom MPI datatypes
std::vector<MPI_Datatype> _type;
std::vector<int> _blocklen;
std::vector<MPI_Aint> _disp;
MPI_Datatype MPI_Custom;

// MPI variables for the non-blocking message used in the Gather routine
MPI_Request reqs[1];
MPI_Status stats[1];

// Pipeline stuff
std::vector<int (*)(int)> stages;
MPI_Request pipeline_reqs[2];
MPI_Status pipeline_stats[2];

// pthread prototypes
template<typename R, typename... Args, typename... AArgs>
int Farm(int num_threads, int input_len, R(*worker)(Args...), AArgs... args);

// MPI prototypes
void Init();
template<typename R, typename... Args>
void Load(R(*func)(Args...), Args... args);
template <typename T>
void Scatter(T* send_buffer, int count, T* receive_buffer);
template <typename T>
void Broadcast(T* send_buffer, int count);
template <typename T>
void Gather(T* send_buffer, int count, T* receive_buffer);
void Finish();
void Abort();

// pthread implementations
template<typename R, typename... Args>
struct ThreadData {
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

// WorkerWrapper function that is called on separate threads,
// calls the provided worker function with the processeed arguments
template<typename R, typename... Args>
void* WorkerWrapper(void* threadarg) {
    ThreadData<R, Args...>* my_data = (ThreadData<R, Args...>*) threadarg;
    int tid = my_data->thread_id;
    printf("rank %d -> Working thread: %d\n", rank, tid);

    R res = my_data->run_worker();

    printf("rank %d -> Thread %ld done with res: %d.\n", rank, tid, res);
    pthread_exit(NULL);
}

// Farm function that calls 'worker' function on 'input_array' with length 'input_len'
template<typename R, typename... Args, typename... AArgs>
int Farm(int num_threads, int input_len, R(*worker)(Args...), AArgs... args) {
    printf("rank %d -> In Farm with num args: %d\n", rank, sizeof...(AArgs));
    //printf("num_threads: %d, input_len: %d\n", num_threads, input_len);

    //int * result = (int*)malloc(input_len * sizeof(int));
    //free(result)

    if (num_threads > kMaxThreads) num_threads = kMaxThreads;

    // If there are more threads requested than the array_lenght reduce threads
    if (num_threads > input_len) num_threads = input_len; //TODO if input_len % numTasks != 0
    //printf("rank %d -> num_threads: %d\n", rank, num_threads);

    // Initialize array of threads 
    pthread_t threads[num_threads];

    // Initialize thread_data_array 
    ThreadData<R, Args...> thread_data_array[num_threads];

    // Initialize and set thread detached attribute
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int return_code;
    int batch_size = (input_len / numTasks) / num_threads;
    for (int t = 0; t < num_threads; t++) {
        int start = t * batch_size;
        int end = (t + 1) * batch_size;
        if (t == num_threads - 1) end = input_len / numTasks + input_len % numTasks; // add remainder
        printf("rank %d -> new thread with t:%d, start:%d, end:%d\n", rank, t, start, end);
        
        // Set values in thread_data_array to be passed to the corresponding threads
        thread_data_array[t].thread_id = t;
        thread_data_array[t].worker = worker;
        thread_data_array[t].args = std::tuple<Args...>(start, end, args...);

        return_code = pthread_create(&threads[t], &attr, WorkerWrapper<R, Args...>, (void*)&thread_data_array[t]);
        if (return_code) {
            printf("ERROR; return code from pthread_create() is %d\n", return_code);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);

    for (int t = 0; t < num_threads; t++) {
        return_code = pthread_join(threads[t], NULL);
        if (return_code) {
            printf("ERROR; return code from pthread_join() is %d\n", return_code);
            exit(-1);
        }
        printf("rank %d -> Completed join with thread %ld\n", rank, t);
    }

    // Last thing that main() should do
    //pthread_exit(NULL);  // TODO put this in main

    return 1;  // TODO Change the return type
}

// MPI implementations

// Templated function to return the MPI datatype
// Code adapted from: https://stackoverflow.com/questions/42490331/generic-mpi-code
template <typename T>
MPI_Datatype ResolveType() // Default function, returns the custom datatype
{
    return MPI_Custom;
}

template <>
MPI_Datatype ResolveType<float>()
{
    return MPI_FLOAT;
}

template <>
MPI_Datatype ResolveType<int>()
{
    return MPI_INT;
}

template <>
MPI_Datatype ResolveType<double>()
{
    return MPI_DOUBLE;
}

template <>
MPI_Datatype ResolveType<char>()
{
    return MPI_CHAR;
}

template <>
MPI_Datatype ResolveType<long>()
{
    return MPI_LONG;
}

template <>
MPI_Datatype ResolveType<short>()
{
    return MPI_SHORT;
}

// Custom offsetOf fuction
// Code taken from: https://stackoverflow.com/questions/13180842/how-to-calculate-offset-of-a-class-member-at-compile-time
template<typename T, typename U> constexpr size_t offsetOf(U T::* member)
{
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

template <typename C, typename T>
void access(C& cls, T C::* member) {
    //printf("rank %d -> In smallest access, member: %d\n", rank, cls.*member);
    _type.push_back(ResolveType<typename std::remove_all_extents<T>::type>());
    _blocklen.push_back(1);
    _disp.push_back(offsetOf(member));
}

template <typename C, typename T, typename... Mems>
void access(C& cls, T C::* member, Mems... rest) {
    //printf("rank %d -> In bigger access, sizeof rest:%d\n", rank, sizeof...(Mems));
    access(cls, member);
    if (sizeof...(Mems) > 0) access(cls, rest...);
}

template <typename T, typename... Members>
void MakeCustomDatatype(T* a, Members... mems) {
    printf("rank %d -> In MakeCustomDatatype\n", rank);
    access(*a, mems...);

    // Creating the datatype
    MPI_Datatype* type = &_type[0];
    int* blocklen = &_blocklen[0];
    MPI_Aint* disp = &_disp[0];
    MPI_Type_create_struct(_type.size(), blocklen, disp, type, &MPI_Custom);
    MPI_Type_commit(&MPI_Custom);
}

void Init() {
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
}

template<typename R, typename... Args>
void Load(R (*func)(Args...), Args... args) {  // TODO make this work for no arguments
    if (rank == 0) {
        printf("rank %d -> In load\n", rank);
        (*func)(args...);
    }
}

template <typename T>
void Scatter(T* send_buffer, int count, T* receive_buffer) {
    printf("rank %d -> In Scatter\n", rank);
    const int kChunkSize = count / numTasks;
    printf("rank %d -> kChunkSize:%d\n", rank, kChunkSize);

    MPI_Datatype data_type = ResolveType<typename std::remove_all_extents<T>::type>();

    MPI_Scatter(send_buffer, kChunkSize, data_type, receive_buffer, kChunkSize,
        data_type, kSource, MPI_COMM_WORLD);
}

template <typename T>
void Broadcast(T* send_buffer, int count) {
    printf("rank %d -> In Broadcast\n", rank);

    MPI_Datatype data_type = ResolveType<typename std::remove_all_extents<T>::type>();

    MPI_Bcast(send_buffer, count, data_type, kSource, MPI_COMM_WORLD);
}

template <typename T>
void Gather(T* send_buffer, int count, T* receive_buffer) {
    printf("rank %d -> In Gather\n", rank);
    const int kChunkSize = count / numTasks;

    MPI_Datatype data_type = ResolveType<typename std::remove_all_extents<T>::type>();

    // Gather results from all nodes to main node
    MPI_Igather(send_buffer, kChunkSize, data_type, receive_buffer, kChunkSize,
        data_type, kSource, MPI_COMM_WORLD, &reqs[0]);

    if (rank == 0) {
        MPI_Wait(&reqs[0], &stats[0]);
        printf("rank %d -> Finished gathering\n", rank);
    }
}

void Finish() {
    printf("rank %d -> Finished exectuion.\n", rank);
    MPI_Finalize();
    
    if (rank != 0) {
        printf("rank %d -> Exiting not-main process\n", rank);
        exit(0);
    }
}

void Abort() {
    printf("rank %d -> Aborting.\n", rank);
    MPI_Abort(MPI_COMM_WORLD, 1);
}

// Pipeline implementations
template<typename R, typename... Args>
void AddStage(R(*func)(Args...), Args... args) {
    printf("rank %d -> In AddStage.\n", rank);
    stages.push_back(func);
    printf("rank %d -> there are %d stages now.\n", rank, stages.size());
}

void RunPipeline() {
    printf("rank %d -> In RunPipeline.\n", rank);
    prev = (rank - 1) % numTasks;
    next = (rank + 1) % numTasks;

    int input = 5;
    
    MPI_Irecv(&input, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &pipeline_reqs[0]);

    if (rank != 0) {
        MPI_Wait(&reqs[0], &stats[0]);
    }

    int result = stages.at(rank)(input);

    MPI_Isend(&result, 1, MPI_INT, next, 0, MPI_COMM_WORLD, &pipeline_reqs[1])

}

#endif  // _MLPPLIB_H_
