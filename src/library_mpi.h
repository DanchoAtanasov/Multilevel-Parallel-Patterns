// library_mpi.h
#ifndef _LIBRARY_MPI_H_
#define _LIBRARY_MPI_H_

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int numtasks, rank, sendcount, recvcount, source;

MPI_Request reqs[1];
MPI_Status stats[1];


void init() {
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
}

void load(void(*func)()) {
    if (rank == 0) {
        printf("In load\n");
        (*func)();
    }
}

void scatter(float matrix[][10], int SIZE, float submatrix[][10]) {
    printf("In scatter\n");
    const int SUBMATRIX_TOTAL_SIZE = SIZE / numtasks;

    // Scattering matrix1 to all nodes in chunks
    MPI_Scatter(matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, submatrix, SUBMATRIX_TOTAL_SIZE,
        MPI_FLOAT, 0, MPI_COMM_WORLD);
}

void broadcast(float matrix[][10], int SIZE) {
    printf("In broadcast\n");
    // Broadcasting the whole matrix2 to all nodes
    MPI_Bcast(matrix, SIZE, MPI_FLOAT,
        0, MPI_COMM_WORLD);
}

void finish() {
    if (rank != 0) {
        printf("Exiting not main process, rank:%d\n", rank);
        exit(0);
    }
}

template<typename R, typename... Args>
int farm(R(*worker)(Args...), const int MATRIX_SIZE, float submatrix[][10], float matrix3[][10]){
    printf("In farm\n");

    printf("rank: %d, submatrix[0][0]: %.2f\n", rank, submatrix[0][0]);

    const int SUBMATRIX_TOTAL_SIZE = MATRIX_SIZE / numtasks;
    float result_matrix[SUBMATRIX_TOTAL_SIZE / 10][10];
    printf("rank: %d, SUBMATRIX_TOTAL_SIZE: %d\n", rank, SUBMATRIX_TOTAL_SIZE);

    // Call worker function, save result in result_matrix
    int res = 1;
    (*worker)(submatrix, SUBMATRIX_TOTAL_SIZE / 10, result_matrix);

    // Gather results from all nodes to main node
    MPI_Igather(result_matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, matrix3, SUBMATRIX_TOTAL_SIZE,
        MPI_FLOAT, source, MPI_COMM_WORLD, &reqs[0]);

    if (rank == 0) {
        MPI_Wait(&reqs[0], &stats[0]);
    }

    printf("rank: %d finished exectuion.\n", rank);

	MPI_Finalize();

	return res;
}

#endif
