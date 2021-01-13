// test_library.cpp

#include "src/library.h"
#include <iostream>


int main(){
    printf("Start of main\n");

    /*int arr[5] = { 1, 2, 3, 4, 5 };
    int res = farm<int>(addOne, 5, arr, 5);
    printf("--------------------------------\n");

    int arr2[7] = { 1, 2, 3, 4, 5, 6, 7 };
    res = farm<int>(addTwo, 7, arr2);
    printf("--------------------------------\n");

    int arr3[5] = { 'a', 'b', 'c', 'd', 'f' };
    res = farm<char>(addChar, 5, arr3);
    printf("--------------------------------\n");*/

    //printf("%d\n", foo(3, 5.5f));

    int arr4[5] = { 1, 2, 3, 4, 5 };
    res = farm<int, int, int>(add2Args, 5, arr4);
    printf("--------------------------------\n");

    printf("End of main\n");
    
    return 0;
}

