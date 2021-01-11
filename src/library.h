// library.h

int addOne(int number);

int farm(int (*fcnPtr)(int));

//int (*fcnPtr)(int) { &foo }; // Initialize fcnPtr with function foo
//(*fcnPtr)(5); // call function foo(5) through fcnPtr.