#include "mpi.h"
#include <stdio.h>

#define MATRIX_SIZE 12

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

const int TAG = 1;

void work(float submatrix[][MATRIX_SIZE], int SUBMATRIX_ROWS, float result_matrix[][MATRIX_SIZE]) {
    printf("Working beep boop...\n");
    for (int i = 0; i < SUBMATRIX_ROWS; i++) {
       for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += submatrix[i][k] * matrix2[k][j];
            }
            result_matrix[i][j] = c;
        }
    }
}


int main(int argc, char* argv[]) {
    int numtasks, rank;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Request reqs[5];
    MPI_Status stats[5];

    const int MATRIX_TOTAL_SIZE = MATRIX_SIZE * MATRIX_SIZE;
    const int SUBMATRIX_ROWS = MATRIX_SIZE / numtasks;
    const int SUBMATRIX_TOTAL_SIZE = SUBMATRIX_ROWS * MATRIX_SIZE;
    float submatrix[SUBMATRIX_ROWS][MATRIX_SIZE];
    
    
    if (rank == 0) {
        // Initialize matrix values
        for (int i = 0; i < MATRIX_SIZE; i++)
            for (int j = 0; j < MATRIX_SIZE; j++) {
                matrix1[i][j] = 2.0f;
                matrix2[i][j] = 3.0f;
            }

        // Send matrix values to all nodes including itself
        for (int dest = 0; dest < numtasks; dest++) {
            MPI_Isend(matrix1 + (dest * SUBMATRIX_ROWS), SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD, &reqs[0]);
            MPI_Isend(matrix2, MATRIX_TOTAL_SIZE, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD, &reqs[1]);
        }
    }

    printf("Greetings from rank %d\n", rank);

    // Receive matrices
    MPI_Irecv(submatrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(matrix2, MATRIX_TOTAL_SIZE, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &reqs[1]);

    MPI_Waitall(2, reqs, stats);

    float final_matrix[2][SUBMATRIX_ROWS][MATRIX_SIZE];
    if (rank == 0) {
        // Receive results from other nodes
	//printf("matrixsize:%d, submatrix_total_size:%d, submatrix_rows:%d\n", MATRIX_SIZE, SUBMATRIX_TOTAL_SIZE, SUBMATRIX_ROWS);
        for (int dest = 0; dest < numtasks; dest++) {
            MPI_Irecv(final_matrix[dest], SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, dest, TAG, MPI_COMM_WORLD, &reqs[dest]);
        }
    }

    float result_matrix[SUBMATRIX_ROWS][MATRIX_SIZE];

    printf("Greetings from rank %d again\n", rank);
    printf("submatrix[0][0] is:  %.2f\n", submatrix[0][0]);
    printf("matrix1[0][0] is:  %.2f\n", matrix1[0][0]);
    printf("matrix2[0][0] is:  %.2f\n", matrix2[0][0]);
    work(submatrix, SUBMATRIX_ROWS, result_matrix);
        
    printf("result_matrix[0][0] after work is:  %.2f\n", result_matrix[0][0]);

    // Send result of work back to main node
    MPI_Isend(result_matrix, SUBMATRIX_TOTAL_SIZE, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &reqs[2]);

    //MPI_Waitall(1, reqs+2, stats+2);

    if (rank == 0) {
        printf("Greetings from rank %d for the third time\n", rank);
        MPI_Waitall(2, reqs, stats);
        printf("Received final_matrix[0][0][0] is %.2f\n", final_matrix[0][0][0]);
        printf("Received final_matrix[1][0][0] is %.2f\n", final_matrix[1][0][0]);

        // Combine results and print matrix
        for(int i = 0; i < MATRIX_SIZE; i++){
	        for(int j = 0; j < MATRIX_SIZE; j++){
                    matrix3[i][j] = final_matrix[0][i][j];//final_matrix[i - rank * SUBMATRIX_ROWS][j];
	            printf("%.2f ", matrix3[i][j]); 
	        }
	        printf("\n");
	    }
    }

    printf("rank: %d over\n", rank);

    MPI_Finalize();

    return 0;
}
