#include "mpi.h"
#include <stdio.h>

#define MATRIX_SIZE 10

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

struct _data {
    float m1[MATRIX_SIZE][MATRIX_SIZE];
    float m2[MATRIX_SIZE][MATRIX_SIZE];
} data;

int main(int argc, char* argv[]) {
    int numtasks, rank, sendcount, recvcount, source;

    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }

    /*float sendbuf[SIZE][SIZE] = {
       {1.0, 2.0},
       {5.0, 6.0}
    };*/
    float sendbuf = matrix1;
    float recvbuf[SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks == SIZE) {
        source = 1;
        sendcount = SIZE;
        recvcount = SIZE;
        MPI_Scatter(sendbuf, sendcount, MPI_FLOAT, recvbuf, recvcount,
            MPI_FLOAT, source, MPI_COMM_WORLD);

        printf("rank= %d  Results: %f %f\n", rank, recvbuf[0],
            recvbuf[1]);
    }
    else
        printf("Must specify %d processors. Terminating.\n", SIZE);

    MPI_Finalize();
}


