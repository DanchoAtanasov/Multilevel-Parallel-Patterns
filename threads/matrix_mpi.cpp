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
//    int numtasks, rank, sendcount, recvcount, source;

    int numtasks, rank, tag1 = 1;

    MPI_Request reqs[1];   // required variable for non-blocking calls
    MPI_Status stats[1];   // required variable for Waitall routine

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }

   const int SPLIT = MATRIX_SIZE / numtasks;
    int from = rank * SPLIT;
    int to = from + SPLIT;
    int res[MATRIX_SIZE * SPLIT];
    printf("In rank %d: from:%d, to:%d, res_size:%d\n", rank, from, to, MATRIX_SIZE * SPLIT);
    for (int i = from; i < to; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += matrix1[i][k] * matrix2[k][j];
            }
            res[MATRIX_SIZE * i + j] = c;
            //matrix3[i][j] = c;
        }
    }

    int buf[MATRIX_SIZE * SPLIT];

    int payload = 313;
    if (rank == 0) {
        printf("Greetings from rank %d\n", rank);
        printf("res: %d, %d\n", res[0], res[MATRIX_SIZE * MATRIX_SIZE / numtasks - 1]);
        MPI_Isend(res, MATRIX_SIZE * SPLIT, MPI_INT, 1, tag1, MPI_COMM_WORLD, &reqs[0]);
    }
    else {
        printf("Greetings from rank %d\n", rank);
        printf("res: %d, %d\n", res[0], res[MATRIX_SIZE * MATRIX_SIZE / numtasks - 1]);
        MPI_Irecv(buf, MATRIX_SIZE * SPLIT, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[0]);
    }

    

    MPI_Waitall(1, reqs, stats);

    if (rank == 0) {
        printf("Greetings from rank %d again\n", rank);
        //    printf("Integer in received buffer is %d\n", buf[0]);
    }
    else {
        printf("Greetings from rank %d again\n", rank);
        //printf("Integer in received buffer is %d\n", buf[0]);
        printf("Integer in received buffer is %d, %d\n", buf[0], buf[MATRIX_SIZE * SPLIT - 1]);
    }

    MPI_Finalize();
}




