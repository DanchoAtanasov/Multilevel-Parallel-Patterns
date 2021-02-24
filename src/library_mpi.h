// library_mpi.h
#ifndef _LIBRARY_MPI_H_
#define _LIBRARY_MPI_H_

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int numtasks, rank, sendcount, recvcount, source;

MPI_Request reqs[3];
MPI_Status stats[3];

float submatrix[5][10]; // Remove this

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

void scatter(float matrix[][10], int SIZE) {
    printf("In scatter\n");
    //const int SUBMATRIX_ROWS = SIZE / numtasks;
    const int SUBMATRIX_TOTAL_SIZE = SIZE / numtasks;
    printf("SUBMATRIX:%d\n", SUBMATRIX_TOTAL_SIZE);
    //float submatrix[SUBMATRIX_ROWS][10];

    // Scattering matrix1 to all nodes in chunks
    MPI_Iscatter(matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, submatrix, SUBMATRIX_TOTAL_SIZE,
        MPI_FLOAT, 0, MPI_COMM_WORLD, &reqs[0]);
}

void broadcast(float matrix[][10], int SIZE) {
    printf("In broadcast\n");
    // Broadcasting the whole matrix2 to all nodes
    MPI_Ibcast(matrix, SIZE, MPI_FLOAT,
        0, MPI_COMM_WORLD, &reqs[1]);
}

void finish() {
    if (rank != 0) {
        printf("Exiting not main process, rank:%d\n", rank);
        exit(0);
    }
}

template<typename R, typename... Args>
int farm(R(*worker)(Args...), const int MATRIX_SIZE, float matrix3[][10]){ 
    printf("In farm\n");
	/*int numtasks, rank, sendcount, recvcount, source;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	MPI_Request reqs[3];
	MPI_Status stats[3];*/

	/*const int MATRIX_TOTAL_SIZE = MATRIX_SIZE * MATRIX_SIZE;
	const int SUBMATRIX_ROWS = MATRIX_SIZE / numtasks;
	const int SUBMATRIX_TOTAL_SIZE = SUBMATRIX_ROWS * MATRIX_SIZE;*/
	//float submatrix[SUBMATRIX_ROWS][10];

	/*if (rank == 0) {
		for (int i = 0; i < MATRIX_SIZE; i++)
			for (int j = 0; j < MATRIX_SIZE; j++) {
				matrix1[i][j] = 2.0f;
				matrix2[i][j] = 3.0f;
			}
	}*/

	/*source = 0;
	sendcount = SUBMATRIX_TOTAL_SIZE;
	recvcount = SUBMATRIX_TOTAL_SIZE;*/

    // Scattering matrix1 to all nodes in chunks
    /*MPI_Iscatter(matrix1, sendcount, MPI_FLOAT, submatrix, recvcount,
       MPI_FLOAT, source, MPI_COMM_WORLD, &reqs[0]);*/
    //// Broadcasting the whole matrix2 to all nodes
    //MPI_Ibcast(matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT,
    //    source, MPI_COMM_WORLD, &reqs[1]);

    // Wait for messages to be received
    MPI_Waitall(2, reqs, stats);

    printf("rank: %d, submatrix[0][0]: %.2f\n", rank, submatrix[0][0]);
    //printf("rank: %d, matrix2[0][0]: %.2f\n", rank, matrix2[0][0]);

    const int SUBMATRIX_TOTAL_SIZE = MATRIX_SIZE / numtasks;
    float result_matrix[SUBMATRIX_TOTAL_SIZE / 10][10];
    printf("rank: %d, SUBMATRIX_TOTAL_SIZE: %d\n", rank, SUBMATRIX_TOTAL_SIZE);
    // Call worker function, save result in result_matrix
    int res = 1;
     (*worker)(submatrix, SUBMATRIX_TOTAL_SIZE / 10, result_matrix);

    // Gather results from all nodes to main node
    MPI_Igather(result_matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, matrix3, SUBMATRIX_TOTAL_SIZE,
        MPI_FLOAT, source, MPI_COMM_WORLD, &reqs[2]);

    if (rank == 0) {
        MPI_Wait(&reqs[2], &stats[2]);

        //printf("Gathered results:\n");
        /*for (int i = 0; i < MATRIX_SIZE/10; i++) {
            for (int j = 0; j < MATRIX_SIZE/10; j++) {
                printf("%.2f ", matrix3[i][j]);
            }
            printf("\n");
        }*/
    }

    printf("rank: %d finished exectuion.\n", rank);

	MPI_Finalize();

	return res;
}

#endif
