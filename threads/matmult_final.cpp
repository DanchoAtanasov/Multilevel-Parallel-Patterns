#include <iostream>
#include "../src/mlpplib.h"

const int MATRIX_SIZE = 8;

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

float submatrix[4][MATRIX_SIZE];

int matrixmult(int start, int end, double additionalOption) {
    printf("In matrixmult with %d %d\n", start, end);
    printf("additionalOption: %.2f\n", additionalOption);
    for (int i = start; i < end; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += submatrix[i][k] * matrix2[k][j];
            }
            matrix3[i][j] = c;
        }
    }
    return 1;
}

// Worker function that calculates matrix multiplication
void work(float submatrix[][MATRIX_SIZE], int SUBMATRIX_ROWS, float result_matrix[][MATRIX_SIZE]) {
    printf("Working beep boop %d...\n", SUBMATRIX_ROWS);
    for (int i = 0; i < SUBMATRIX_ROWS; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                //printf("sub:%f, m2:%f\n", submatrix[i][k], matrix2[k][j]);
                c += submatrix[i][k] * matrix2[k][j];
                result_matrix[i][j] = c;
            }
        }
    }
}

void init_matrices() {
    printf("Beginning of init_matrices\n");
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }
}

int main() {
    printf("Beginning of main\n");

    init();
    load(init_matrices);
    scatter(matrix1, MATRIX_SIZE * MATRIX_SIZE, submatrix);
    broadcast(matrix2, MATRIX_SIZE * MATRIX_SIZE);
    // MPI farm
    //int res = mpi_farm(work, MATRIX_SIZE * MATRIX_SIZE, submatrix, matrix3);
    // pthread farm
    int res = farm(2, 4, matrixmult, 35.50);
    finish();

    printf("First and last values in the matrix are: %.0f, %.0f\n", matrix3[0][0], matrix3[MATRIX_SIZE - 1][MATRIX_SIZE - 1]);
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%.2f ", matrix3[i][j]);
        }
        printf("\n");
    }

    return 0;
}
