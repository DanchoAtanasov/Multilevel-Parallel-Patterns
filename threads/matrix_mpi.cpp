#include "mpi.h"
#include <stdio.h>

#define MATRIX_SIZE 12

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

const int TAG = 1;

void work(float batch_matrix1[][MATRIX_SIZE], int batch_size, float matrix_result[][MATRIX_SIZE]) {
    printf("Working beep boop...\n");
    for (int i = 0; i < batch_size; i++) {
       for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += batch_matrix1[i][k] * matrix2[k][j];
            }
            matrix_result[i][j] = c;
        }
    }
}


int main(int argc, char* argv[]) {
    int numtasks, rank;
    
    MPI_Request reqs[5];
    MPI_Status stats[5];

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int BATCH_SIZE = MATRIX_SIZE / numtasks;
    float batch_matrix1[BATCH_SIZE][MATRIX_SIZE];
    float matrix_result[BATCH_SIZE][MATRIX_SIZE];
    
    if (rank == 0) {
        printf("Greetings from rank %d\n", rank);

        // Initialize matrix values
        for (int i = 0; i < MATRIX_SIZE; i++)
            for (int j = 0; j < MATRIX_SIZE; j++) {
                matrix1[i][j] = 2.0f;
                matrix2[i][j] = 3.0f;
            }

        for (int dest = 0; dest < numtasks; dest++) {
            // Send matrix values
            MPI_Isend(matrix1+(dest * BATCH_SIZE), BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD, &reqs[0]);
            MPI_Isend(matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD, &reqs[1]);
        }
    }
    //else {
        printf("Greetings from rank %d\n", rank);

        //float _matrix1[BATCH_SIZE][MATRIX_SIZE];
        //float _matrix2[MATRIX_SIZE][MATRIX_SIZE];

        // Receive matrices
        //MPI_Irecv(dancho, 2, MPI_FLOAT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(batch_matrix1, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &reqs[1]);
        
    //}

    MPI_Waitall(2, reqs, stats);

    float matrix_res[BATCH_SIZE][MATRIX_SIZE];
    if (rank == 0) {

	for(int dest=0;dest<numtasks;dest++){
            MPI_Irecv(matrix_res, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD, &reqs[2]);
	}
        //printf("Greetings from rank %d again\n", rank);

        //work(batch_matrix1, BATCH_SIZE, matrix_result);
        //printf("matrix_result[0][0] after work is:  %.2f\n", matrix_result[0][0]);
        //MPI_Irecv(matrix_res, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 1, TAG, MPI_COMM_WORLD, &reqs[2]);
    }
    //else {
        printf("Greetings from rank %d again\n", rank);
        printf("batch_matrix1[0][0] is:  %.2f\n", batch_matrix1[0][0]);
        printf("matrix1[0][0] is:  %.2f\n", matrix1[0][0]);
        printf("matrix2[0][0] is:  %.2f\n", matrix2[0][0]);
        work(batch_matrix1, BATCH_SIZE, matrix_result);
        
        printf("matrix_result[0][0] after work is:  %.2f\n", matrix_result[0][0]);

        MPI_Isend(matrix_result, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &reqs[2]);
    //}

    MPI_Wait(&reqs[2], &stats[2]);

    if (rank == 0) {
        printf("Greetings from rank %d for the third time\n", rank);
        printf("Received matrix_res[0][0] is %.2f\n", matrix_res[0][0]);

        for(int i = 0; i < MATRIX_SIZE; i++){
	        for(int j = 0; j < MATRIX_SIZE; j++){
                    matrix3[i][j] = matrix_res[i - rank*BATCH_SIZE][j];
                /*if(i < BATCH_SIZE){
		            matrix3[i][j] = matrix_result[i][j];
		        }
		        else{
		            matrix3[i][j] = matrix_res[i-BATCH_SIZE][j];
		        }*/
	            printf("%.2f ", matrix3[i][j]); 
	        }
	        printf("\n");
	    }
    }

    MPI_Finalize();

    return 0;
}
