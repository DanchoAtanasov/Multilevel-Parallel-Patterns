#include "mpi.h"
#include <stdio.h>

#define MATRIX_SIZE 2

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
    //float sendbuf[][] = matrix1;
    float recvbuf[2][MATRIX_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks == MATRIX_SIZE) {
        source = 1;
        sendcount = MATRIX_SIZE;
        recvcount = MATRIX_SIZE;
        MPI_Scatter(matrix1, sendcount, MPI_FLOAT, recvbuf[0], recvcount,
            MPI_FLOAT, source, MPI_COMM_WORLD);

        printf("rank= %d  Results: %f %f\n", rank, recvbuf[0][0],
            recvbuf[0][1]);
        
	MPI_Scatter(matrix2, sendcount, MPI_FLOAT, recvbuf[1], recvcount,
            MPI_FLOAT, source, MPI_COMM_WORLD);
        
	printf("rank= %d  Results: %f %f\n", rank, recvbuf[1][0],
            recvbuf[1][1]);
    }
    else
        printf("Must specify %d processors. Terminating.\n", MATRIX_SIZE);

    MPI_Finalize();
}


