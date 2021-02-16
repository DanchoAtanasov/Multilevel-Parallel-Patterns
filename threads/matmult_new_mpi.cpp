#include <iostream>
#include "../src/library_mpi.h"

#define MATRIX_SIZE 100

float matrix1[MATRIX_SIZE][MATRIX_SIZE];
float matrix2[MATRIX_SIZE][MATRIX_SIZE];
float matrix3[MATRIX_SIZE][MATRIX_SIZE];

int matrixmult(int start, int end, double additionalOption) {
    printf("In matrixmult with %d %d\n", start, end);
    printf("additionalOption: %.2f\n", additionalOption);
    for (int i = start; i < end; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float c = 0.0f;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c += matrix1[i][k] * matrix2[k][j];
            }
            matrix3[i][j] = c;
        }
    }
    return 1;
}

int main() {
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }

    //int res = farm(3, 100, matrixmult, 35.50);

    foo();

    printf("First and last values in the matrix are: %.0f, %.0f\n", matrix3[0][0], matrix3[MATRIX_SIZE - 1][MATRIX_SIZE - 1]);
}
