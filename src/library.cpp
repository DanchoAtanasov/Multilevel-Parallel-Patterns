// library.cpp

#include "library.h"

int addOne(int number){
    return number + 1;
}

int farm(int (*fcnPtr)(int)) {
    return (*fcnPtr)(5);
}

