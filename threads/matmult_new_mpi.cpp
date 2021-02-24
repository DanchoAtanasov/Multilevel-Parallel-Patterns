#include <iostream>
#include "../src/library_mpi.h"

const int MATRIX_SIZE = 10;

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];


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
    /*for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }*/
    init();
    load(init_matrices);
    scatter(matrix1, MATRIX_SIZE * MATRIX_SIZE);
    broadcast(matrix2, MATRIX_SIZE * MATRIX_SIZE);

    //int res = farm(3, 100, matrixmult, 35.50);

    //int res = farm(foo, matrix1, matrix2, matrix3);
    int res = farm(work, MATRIX_SIZE * MATRIX_SIZE, matrix3);
    finish();



    printf("First and last values in the matrix are: %.0f, %.0f\n", matrix3[0][0], matrix3[MATRIX_SIZE - 1][MATRIX_SIZE - 1]);
}
