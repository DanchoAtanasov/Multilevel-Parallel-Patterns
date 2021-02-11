#include "mpi.h"
#include <stdio.h>

typedef struct bla_{
    int x;
} bla;

int main(int argc, char *argv[])  {
    int numtasks, rank, sendcount, recvcount, source;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    MPI_Request reqs[1];
    MPI_Status stats[1];
    MPI_Datatype Bla;
    MPI_Datatype type[1] = { MPI_INT };
    int blocklen[1] = { 1 };
    MPI_Aint disp[1] = { 0 };

    source = 0;
    sendcount = 1;
    recvcount = 1;

    bla arr[2];
    arr[0].x = 5;    
    arr[1].x = 9;    

    MPI_Type_create_struct(1, blocklen, disp, type, &Bla);
    MPI_Type_commit(&Bla);

    bla dua[1];
    // Scattering matrix1 to all nodes in chunks
    MPI_Iscatter(arr,sendcount,Bla,dua,recvcount,
        Bla,source,MPI_COMM_WORLD, &reqs[0]);

    // Wait for messages to be received
    MPI_Waitall(1, reqs, stats);

    printf("rank:%d, received:%d\n", rank, dua[0].x);

    MPI_Finalize();
}

