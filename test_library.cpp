// test_library.cpp

#include "src/library.h"
#include <iostream>

int main(){
    printf("Start of main\n");
    int arr[5] = { 1, 2, 3, 4, 5 };
    int res = farm(addTwo, 5, arr);

    return 0;
}
