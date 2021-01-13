// test_library.cpp

#include "src/library.h"
#include <iostream>

int main(){
    printf("Start of main\n");
    int arr[5] = { 1, 2, 3, 4, 5 };

    int res = farm(addOne, 5, arr, 5);
    printf("Between both function calls\n");

    int arr2[7] = { 1, 2, 3, 4, 5, 6, 7 };
    res = farm(addTwo, 7, arr);
    printf("End of main\n");
    
    return 0;
}

