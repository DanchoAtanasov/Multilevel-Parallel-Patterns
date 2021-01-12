// library.cpp

#include "library.h"
#include <iostream>
#include <pthread.h>

int addOne(int number){
    return number + 1;
}

int farm(int (*worker)(int arg), int arr_len, int* input_arr) {
    //int * result = (int*)malloc(arr_len * sizeof(int));
    printf("arg: %d\n", arg);
    int result[arr_len];
    for (int i = 0; i < arr_len; i++) {
        result[i] = (*worker)(input_arr[i]);
    }
    for (int i = 0; i < arr_len; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");
    //free(result);

    return 1;
}

