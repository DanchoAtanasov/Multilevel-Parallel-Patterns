// test_library.cpp

#include "src/library.h"
#include <iostream>

int myfoo(int start, int end, int a, int b, int c){
    printf("In myfoo with start: %d, end: %d\n", start, end);
    printf("IN myfoo with %d %d %d\n", a, b, c);
    return a - b + c;
}

// Placeholder function for testing
int addOne(int start, int end, int number) {
    printf("In myfoo with start: %d, end: %d\n", start, end);
    printf("Adding one to %d\n", number);
    return number + 1;
}

// Placeholder function for testing
int addTwo(int start, int end, int number) {
    printf("In myfoo with start: %d, end: %d\n", start, end);
    printf("Adding two to %d\n", number);
    return number + 2;
}

// Placeholder function for testing
char addChar(int start, int end, char character) {
    printf("In myfoo with start: %d, end: %d\n", start, end);
    printf("Adding char to %c\n", character);
    return character;
}

// Placeholder function for testing
int add2Args(int start, int end, int x, int y) {
    printf("In myfoo with start: %d, end: %d\n", start, end);
    printf("Adding with 2 args %d + %d: \n", x, y);
    return x + y;
}

// Placeholder function for testing
int sumArr(int start, int end, int *arr, int arr_len) {
    printf("In sumArr with start: %d, end: %d\n", start, end);
    printf("arr_len: %d \n", arr_len);
    int res = 0;
    for(int i = 0; i < arr_len; i++){
        //printf("curr: %d\n", arr[i]);
        res += arr[i];
    } 
    printf("res: %d\n", res);
    return res;
}

int main(){
    printf("Start of main\n");

    int res = farm(5, 5, addOne, 3);
    printf("--------------------------------\n");

    res = farm(5, 5, addTwo, 7);
    printf("--------------------------------\n");

    res = farm(5, 5, addChar, 'A');
    printf("--------------------------------\n");

    int arr[5] = {1, 2, 3, 4, 5};
    //res = farm(5, 5, myfoo, 2, 5, 8);
    res = farm(5, 5, sumArr, arr, 5);
    printf("--------------------------------\n");

    res = farm(5, 5, add2Args, 1, 2);
    printf("--------------------------------\n");

    printf("End of main\n");
    
    return 0;
}

