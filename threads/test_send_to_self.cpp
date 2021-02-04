#include "mpi.h"
#include <stdio.h>

const int TAG = 1; 

int main(int argc, char* argv[]) {
    int numtasks, rank;
    MPI_Request reqs[1];
    MPI_Status stats[1];

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int a = 42;
    int rec = 0;

    printf("Hi from rank %d, numtasks:%d\n", rank, numtasks);
    MPI_Irecv(&rec, 1, MPI_INT, rank, TAG, MPI_COMM_WORLD, &reqs[0]);
		       
    MPI_Isend(&a, 1, MPI_INT, rank, TAG, MPI_COMM_WORLD, &reqs[0]);
    
    MPI_Wait(&reqs[0], &stats[0]);
        
    printf("We here again: rank:%d, rec:%d\n", rank, rec);
    

    MPI_Finalize();
}

