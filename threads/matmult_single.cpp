#include <stdio.h>
#include <stdlib.h>

const int SIZE = 2048;
float matrix1[SIZE][SIZE];
float matrix2[SIZE][SIZE];
float matrix3[SIZE][SIZE];


int main() {
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < SIZE; j++) {
            matrix1[i][j] = 2.0f;
            matrix2[i][j] = 3.0f;
        }

    for(int i = 0; i < SIZE; i++) {
    //if (i % 10 == 0) printf("%d\n", i);
        for(int j = 0; j < SIZE; j++) {
            float c = 0.0f;
           //printf("%d, %d\n", i, j);
            for(int k = 0; k < SIZE; k++) {
	        c+=matrix1[i][k]*matrix2[k][j];
	    }
	    matrix3[i][j]=c;
	}
    }
    printf("First and last values in the matrix are: %.0f, %.0f\n", matrix3[0][0], matrix3[SIZE - 1][SIZE - 1]);

return EXIT_SUCCESS;
}

