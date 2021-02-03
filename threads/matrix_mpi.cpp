#include "mpi.h"
#include <stdio.h>

#define MATRIX_SIZE 12

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

float dancho[1][2]; 

void work(float batch_matrix1[][MATRIX_SIZE], int batch_size, float matrix_result[][MATRIX_SIZE]) {
    printf("Working beep boop...\n");
    //const int BATCH_SIZE = MATRIX_SIZE / numtasks;
    //int from = rank * SPLIT;
    //int to = from + SPLIT;
    for (int i = 0; i < batch_size; i++) {
       for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += batch_matrix1[i][k] * matrix2[k][j];
            }
            matrix_result[i][j] = c;
        }
    }
    //return res;
}


int main(int argc, char* argv[]) {
    int numtasks, rank, tag1 = 1, tag2 = 2, tag3 = 3;
    
    MPI_Request reqs[3], final_req;   // required variable for non-blocking calls
    MPI_Status stats[3], final_stats;   // required variable for Waitall routine

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
                matrix1[i][j] = 2.0f;//(float)i;//2.0f;
                matrix2[i][j] = 3.0f;(float)j;//3.0f;
            }


	float _dancho[2][2] = {{1.0, 2.0}, {3.0, 4.0}};
        //printf("DANCHO[1][1]: %.2f\n", _dancho[1][1]);	
        for (int dest = 1; dest < numtasks; dest++) {
            // Send matrix values
            //MPI_Isend(_dancho, 2, MPI_FLOAT, dest, tag1, MPI_COMM_WORLD, &reqs[0]);
            MPI_Isend(matrix1+(dest * BATCH_SIZE), BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, tag1, MPI_COMM_WORLD, &reqs[0]);
            MPI_Isend(matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD, &reqs[1]);
            //MPI_Isend(matrix1, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, tag1, MPI_COMM_WORLD, &reqs[0]);
            //MPI_Isend(matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD, &reqs[1]);
        }
	for(int i = 0; i < BATCH_SIZE; i++){
		for(int j=0;j<MATRIX_SIZE;j++){
                    batch_matrix1[i][j] = matrix1[i][j];    
		}
	    
	}
	//batch_matrix1 = matrix1;

        //work();
        
    }
    else {
        printf("Greetings from rank %d\n", rank);

        float _matrix1[BATCH_SIZE][MATRIX_SIZE];
        float _matrix2[MATRIX_SIZE][MATRIX_SIZE];

        // Receive matrices
        //MPI_Irecv(dancho, 2, MPI_FLOAT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(batch_matrix1, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, tag2, MPI_COMM_WORLD, &reqs[1]);
        //MPI_Irecv(_matrix1, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
        //MPI_Irecv(_matrix2, MATRIX_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, tag2, MPI_COMM_WORLD, &reqs[1]);
    
        //work();
    }

    MPI_Waitall(2, reqs, stats);

    float matrix_res[BATCH_SIZE][MATRIX_SIZE];
    if (rank == 0) {
        printf("Greetings from rank %d again\n", rank);
        work(batch_matrix1, BATCH_SIZE, matrix_result);
        printf("matrix_result[0][0] after work is:  %.2f\n", matrix_result[0][0]);
        MPI_Irecv(matrix_res, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 1, tag3, MPI_COMM_WORLD, &reqs[2]);
    }
    else {
        printf("Greetings from rank %d again\n", rank);
        printf("batch_matrix1[0][0] is:  %.2f\n", batch_matrix1[0][0]);
        printf("matrix1[0][0] is:  %.2f\n", matrix1[0][0]);
        printf("matrix2[0][0] is:  %.2f\n", matrix2[0][0]);
        work(batch_matrix1, BATCH_SIZE, matrix_result);
        //work();
        printf("matrix_result[0][0] after work is:  %.2f\n", matrix_result[0][0]);
        //res = 10.5 + rank;
        MPI_Isend(matrix_result, BATCH_SIZE * MATRIX_SIZE, MPI_FLOAT, 0, tag3, MPI_COMM_WORLD, &reqs[2]);
    }

    MPI_Wait(&reqs[2], &stats[2]);

    if (rank == 0) {
        printf("Greetings from rank %d for the third time\n", rank);
        printf("Received matrix_res[0][0] is %.2f\n", matrix_res[0][0]);

        for(int i = 0; i < MATRIX_SIZE; i++){
	    for(int j = 0; j < MATRIX_SIZE; j++){
                if(i < BATCH_SIZE){
		    matrix3[i][j] = matrix_result[i][j];
		}
		else{
		    matrix3[i][j] = matrix_res[i-BATCH_SIZE][j];
		}
	        printf("%.2f ", matrix3[i][j]); 
	    }
	    printf("\n");
	}
    }

    

 //   const int SPLIT = MATRIX_SIZE / numtasks;
 //   int from = rank * SPLIT;
 //   int to = from + SPLIT;
 //   int res[SPLIT * MATRIX_SIZE];
 //   printf("In rank %d: from:%d, to:%d, res_size:%d\n", rank, from, to, SPLIT * MATRIX_SIZE);
 //   for (int i = from; i < to; i++) {
 //       for (int j = 0; j < MATRIX_SIZE; j++) {
 //           float c = 0.0f;
 //           for (int k = 0; k < MATRIX_SIZE; k++) {
 //               c += matrix1[i][k] * matrix2[k][j];
 //           }
 //           res[MATRIX_SIZE * (i-SPLIT*rank) + j] = c;
 //       }
 //   }

 //   int buf[SPLIT * MATRIX_SIZE];

 //   if (rank == 0) {
 //       printf("Greetings from rank %d\n", rank);
 //       printf("res rank 0: %d, %d\n", res[0], res[49]);
	//
	//// Send calculated values as 1d array to rank 1
	//MPI_Isend(res, SPLIT * MATRIX_SIZE, MPI_INT, 1, tag1, MPI_COMM_WORLD, &reqs[0]);
 //   }
 //   else {
 //       printf("Greetings from rank %d\n", rank);
 //       printf("res rank %d: %d, %d\n", rank, res[0], res[49]);

	//// Receive calculated values from rank 0
 //       MPI_Irecv(buf, SPLIT * MATRIX_SIZE, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
 //   }

 //   // MPI wait for all messages to be delivered
 //   MPI_Waitall(1, reqs, stats);

 //   if (rank == 0) {
 //       printf("Greetings from rank %d again\n", rank);
 //   }
 //   else {
 //       printf("Greetings from rank %d again\n", rank);
 //       //printf("Integer in received buffer is %d\n", buf[0]);
 //       printf("Integer in received buffer is %d, %d\n", buf[0], buf[49]);
	//
	//for(int i=0;i<MATRIX_SIZE*MATRIX_SIZE;i++){
	//    int _i = i / MATRIX_SIZE;
	//    int _j = i % MATRIX_SIZE;
	//    if(i < MATRIX_SIZE*MATRIX_SIZE/numtasks){
	//      matrix3[_i][_j] = buf[i];
	//    }
	//    else{
 //             matrix3[_i][_j] = res[i-MATRIX_SIZE*MATRIX_SIZE/numtasks];
	//    }
	//}

	//printf("FINAL:\n");
	//for(int i=0;i<MATRIX_SIZE;i++){
	//    for(int j=0;j<MATRIX_SIZE;j++){
 //               printf("%.2f ", matrix3[i][j]);
	//    }
	//    printf("\n");
	//}
 //   }

    MPI_Finalize();

    return 0;
}
