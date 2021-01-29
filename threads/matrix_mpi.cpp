#include "mpi.h"
#include <stdio.h>

#define MATRIX_SIZE 12

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];


int main(int argc, char* argv[]) {
    int numtasks, rank, tag1 = 1;
    
    MPI_Request reqs[1];   // required variable for non-blocking calls
    MPI_Status stats[1];   // required variable for Waitall routine

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize matrix values
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }

   const int SPLIT = MATRIX_SIZE / numtasks;
    int from = rank * SPLIT;
    int to = from + SPLIT;
    int res[SPLIT * MATRIX_SIZE];
    printf("In rank %d: from:%d, to:%d, res_size:%d\n", rank, from, to, SPLIT * MATRIX_SIZE);
    for (int i = from; i < to; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += matrix1[i][k] * matrix2[k][j];
            }
            res[MATRIX_SIZE * (i-SPLIT*rank) + j] = c;
        }
    }

    int buf[SPLIT * MATRIX_SIZE];

    if (rank == 0) {
        printf("Greetings from rank %d\n", rank);
        printf("res rank 0: %d, %d\n", res[0], res[49]);
	
	// Send calculated values as 1d array to rank 1
	MPI_Isend(res, SPLIT * MATRIX_SIZE, MPI_INT, 1, tag1, MPI_COMM_WORLD, &reqs[0]);
    }
    else {
        printf("Greetings from rank %d\n", rank);
        printf("res rank %d: %d, %d\n", rank, res[0], res[49]);

	// Receive calculated values from rank 0
        MPI_Irecv(buf, SPLIT * MATRIX_SIZE, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
    }

    // MPI wait for all messages to be delivered
    MPI_Waitall(1, reqs, stats);

    if (rank == 0) {
        printf("Greetings from rank %d again\n", rank);
    }
    else {
        printf("Greetings from rank %d again\n", rank);
        //printf("Integer in received buffer is %d\n", buf[0]);
        printf("Integer in received buffer is %d, %d\n", buf[0], buf[49]);
	
	for(int i=0;i<MATRIX_SIZE*MATRIX_SIZE;i++){
	    int _i = i / MATRIX_SIZE;
	    int _j = i % MATRIX_SIZE;
	    if(i < MATRIX_SIZE*MATRIX_SIZE/numtasks){
	      matrix3[_i][_j] = buf[i];
	    }
	    else{
              matrix3[_i][_j] = res[i-MATRIX_SIZE*MATRIX_SIZE/numtasks];
	    }
	}

	printf("FINAL:\n");
	for(int i=0;i<MATRIX_SIZE;i++){
	    for(int j=0;j<MATRIX_SIZE;j++){
                printf("%.2f ", matrix3[i][j]);
	    }
	    printf("\n");
	}
    }

    MPI_Finalize();
}




