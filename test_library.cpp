// test_library.cpp

#include "src/library.h"
#include <iostream>

int myfoo(int a, int b, int c){
    return a - b + c;
}

// Placeholder function for testing
int addOne(int number) {
    printf("Adding one to %d\n", number);
    return number + 1;
}

// Placeholder function for testing
int addTwo(int number) {
    printf("Adding two to %d\n", number);
    return number + 2;
}

// Placeholder function for testing
char addChar(char character) {
    printf("Adding char to %c\n", character);
    return character;
}

// Placeholder function for testing
int add2Args(int x, int y) {
    printf("Adding with 2 args %d + %d: \n", x, y);
    return x + y;
}

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
    //wfoo<int, int, int, int>(foo2, 2, 5, 8);
    //wfoo<int, int, int>(foo2, 2, 5);

    int arr[5] = {1, 2, 3, 4, 5};
    wfoo(5, arr, 5, myfoo, 2, 5, 8);

    /*int arr4[5] = { 1, 2, 3, 4, 5 };
    res = farm<int, int, int>(add2Args, 5, arr4);
    printf("--------------------------------\n");

    */
    printf("End of main\n");
    
    return 0;
}

