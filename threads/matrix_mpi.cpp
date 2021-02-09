#include "mpi.h"
#include <stdio.h>
#define MATRIX_SIZE 12

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

void work(float submatrix[][MATRIX_SIZE], int SUBMATRIX_ROWS, float result_matrix[][MATRIX_SIZE]) {
    printf("Working beep boop...\n");
    for (int i = 0; i < SUBMATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += submatrix[i][k] * matrix2[k][j];
                result_matrix[i][j] = c;
            }
        }
    }
}

int main(int argc, char *argv[])  {
	int numtasks, rank, sendcount, recvcount, source;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

        MPI_Request reqs[3];
        MPI_Status stats[3];

        const int MATRIX_TOTAL_SIZE = MATRIX_SIZE * MATRIX_SIZE;
        const int SUBMATRIX_ROWS = MATRIX_SIZE / numtasks;
        const int SUBMATRIX_TOTAL_SIZE = SUBMATRIX_ROWS * MATRIX_SIZE;
        float submatrix[SUBMATRIX_ROWS][MATRIX_SIZE];
        
        if(rank == 0){
            for (int i = 0; i < MATRIX_SIZE; i++)
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    matrix1[i][j] = 2.0f;//i*MATRIX_SIZE + j;//2.0f;
                    matrix2[i][j] = 3.0f;
            }
	}

        source = 0;
        sendcount = SUBMATRIX_TOTAL_SIZE;
        recvcount = SUBMATRIX_TOTAL_SIZE;
        
        printf("rank:%d Lets see if we block or not\n", rank);

        MPI_Iscatter(matrix1,sendcount,MPI_FLOAT,submatrix,recvcount,
            MPI_FLOAT,source,MPI_COMM_WORLD, &reqs[0]);

        MPI_Ibcast(matrix2,MATRIX_SIZE*MATRIX_SIZE,MPI_FLOAT,
            source,MPI_COMM_WORLD, &reqs[1]);
        
        MPI_Waitall(2, reqs, stats);

        printf("rank:%d We here or wat\n", rank);

        printf("rank= %d  Results: %.2f %.2f\n",rank,submatrix[0][0], submatrix[0][1]);
        printf("rank= %d  Results m2: %.2f %.2f\n",rank,matrix2[0][0], matrix2[0][1]);

        float result_matrix[SUBMATRIX_ROWS][MATRIX_SIZE];
        work(submatrix, SUBMATRIX_ROWS, result_matrix);

        MPI_Igather(result_matrix,sendcount,MPI_FLOAT,matrix3,recvcount,
            MPI_FLOAT,source,MPI_COMM_WORLD, &reqs[2]);


        if(rank == 0){
            MPI_Wait(&reqs[2], &stats[2]);
            
            printf("Results should be gathered here\n");
            for(int i=0;i<MATRIX_SIZE;i++){
                for(int j=0;j<MATRIX_SIZE;j++){
                    printf("%.2f ",matrix3[i][j]);
                }
                printf("\n");
            }
        }
               
        printf("rank:%d ending\n", rank);
        
        MPI_Finalize();
}
