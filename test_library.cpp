// test_library.cpp

#include "src/library.h"
#include <iostream>

int main(){
    int arr[5] = { 1, 2, 3, 4, 5 };
    int res = farm(addOne, 5, arr);
    printf("%d\n", res);
	
    return 0;
}

