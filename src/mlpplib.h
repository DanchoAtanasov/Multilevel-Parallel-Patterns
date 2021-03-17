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
std::vector<MPI_Datatype> _type;
std::vector<int> _blocklen;
std::vector<MPI_Aint> _disp;

MPI_Datatype MPI_Custom;

// MPI variables for the non-blocking message used in the Gather routine
MPI_Request reqs[1];
MPI_Status stats[1];

// pthread prototypes
template<typename R, typename... Args, typename... AArgs>
int Farm(int num_threads, int input_len, R(*worker)(Args...), AArgs... args);

// MPI prototypes
void Init();
void Load(void(*func)());
template <typename T, size_t send_size, size_t receive_size>
void Scatter(T(&send_buffer)[send_size], int count, T(&receive_buffer)[receive_size]);
template <typename T, size_t send_size>
void Broadcast(T(&send_buffer)[send_size], int count);
template <typename T, size_t send_size, size_t receive_size>
void Gather(T(&send_buffer)[send_size], int count, T(&receive_buffer)[receive_size]);
void Finish();

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
MPI_Datatype ResolveType() {
    printf("rank %d -> In default ResolveType, creating new datatype\n", rank);

    //MPI_Datatype type[2] = { MPI_INT, MPI_INT };
    /*std::vector<MPI_Datatype> _type;
    _type.push_back(MPI_INT);
    _type.push_back(MPI_INT);*/
    MPI_Datatype* type = &_type[0];
    //int blocklen[2] = { 1, 1 };
    int* blocklen = &_blocklen[0];
    //MPI_Aint disp[2] = { offsetof(T, a), offsetof(T, b) };
    MPI_Aint* disp = &_disp[0];
    MPI_Type_create_struct(2, blocklen, disp, type, &MPI_Custom);
    MPI_Type_commit(&MPI_Custom);

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

template <typename T>
void MakeCustomDatatype() {
    printf("rank %d -> In MakeCustomDatatype\n", rank);

    //T dancho = T();
    //dancho.a = 5;
    //dancho.b = 4.2f;

    //MPI_Datatype MPI_T;
    MPI_Datatype a = ResolveType<typename std::remove_all_extents<int>::type>();
    MPI_Datatype b = ResolveType<typename std::remove_all_extents<T>::type>();
    
    
    
    MPI_Datatype type[2] = { MPI_INT, MPI_FLOAT };
    int blocklen[2] = { 1, 1 };
    MPI_Aint disp[2] = { offsetof(T, a), offsetof(T, b) };
    MPI_Type_create_struct(2, blocklen, disp, type, &MPI_Custom);
    MPI_Type_commit(&MPI_Custom);

    // This needs work to be adaptable for all kinds of structs
    /*MPI_Datatype MPI_OptionData;
    MPI_Datatype type[9] = { MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT,
        MPI_CHAR,  MPI_FLOAT,  MPI_FLOAT };
    int blocklen[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    MPI_Aint disp[9] = { offsetof(OptionData, s), offsetof(OptionData, strike), offsetof(OptionData, r),
        offsetof(OptionData, divq),  offsetof(OptionData, v),  offsetof(OptionData, t),
        offsetof(OptionData, OptionType),  offsetof(OptionData, divs),  offsetof(OptionData, DGrefval), };
    MPI_Type_create_struct(9, blocklen, disp, type, &MPI_OptionData);
    MPI_Type_commit(&MPI_OptionData);*/

}

// Testing accessing class members function
template<typename T, typename U> constexpr size_t offsetOf(U T::* member)
{
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

template <typename C, typename T>
void access(C& cls, T C::* member) {
    printf("rank %d -> In smallest access, member: %d\n", rank, cls.*member);
    _type.push_back(ResolveType<typename std::remove_all_extents<T>::type>());
    _blocklen.push_back(1);
    _disp.push_back(offsetOf(member));
    //return (cls.*member);
}

template <typename C, typename T, typename... Mems>
void access(C& cls, T C::* member, Mems... rest) {
    printf("rank %d -> In bigger access, sizeof rest:%d\n", rank, sizeof...(Mems));
    access(cls, member);
    if (sizeof...(Mems) > 0) access(cls, rest...);
    //return access((cls), rest...);
}

template <typename T, typename... Members>
void doSomething(T* a, Members... mems) {
    printf("rank %d -> In do sth\n", rank);
    access(*a, mems...);
}

void Init() {
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
}

void Load(void(*func)()) {
    if (rank == 0) {
        printf("rank %d -> In load\n", rank);
        (*func)();
    }
}

template <typename T, size_t send_size, size_t receive_size>
void Scatter(T (&send_buffer)[send_size], int count, T (&receive_buffer)[receive_size]) {
    printf("rank %d -> In Scatter\n", rank);
    const int kChunkSize = count / numTasks;

    MPI_Datatype data_type = ResolveType<typename std::remove_all_extents<T>::type>();

    // Scattering matrix1 to all nodes in chunks
    MPI_Scatter(send_buffer, kChunkSize, data_type, receive_buffer, kChunkSize,
        data_type, kSource, MPI_COMM_WORLD);
}

template <typename T, size_t send_size>
void Broadcast(T(&send_buffer)[send_size], int count) {
    printf("rank %d -> In Broadcast\n", rank);

    MPI_Datatype data_type = ResolveType<typename std::remove_all_extents<T>::type>();

    // Broadcasting the whole matrix2 to all nodes
    MPI_Bcast(send_buffer, count, data_type, kSource, MPI_COMM_WORLD);
}

template <typename T, size_t send_size, size_t receive_size>
void Gather(T (&send_buffer)[send_size], int count, T (&receive_buffer)[receive_size]) {
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

#endif  // _MLPPLIB_H_
