#include <iostream>
#include "../src/library.h"

#define MATRIX_SIZE 8

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

    int res = farm(4, 8, matrixmult, 35.50);

    printf("All values in matrix are: %.0f\n", matrix3[0][0]);
    printf("All values in matrix are: %.0f\n", matrix3[1][1]);
}
