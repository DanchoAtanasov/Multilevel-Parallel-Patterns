// library.cpp

#include "library.h"

int addOne(int number){
    return number + 1;
}

int farm(int (*worker)(int), int arr_len, int* input_arr) {
    int * result = (int*)malloc(arr_len * sizeof(int));
    for (int i = 0; i < arr_len; i++) {
        result[i] = (*worker)(input_arr[i]);
    }
    for (int i = 0; i < arr_len; i++) {
        printf("%d ", result[i]);
    }
    prinf("\n");
    free(result);

    return 1;
}

