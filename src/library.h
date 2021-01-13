// library.h

const int MAX_THREADS = 16;

int addOne(int number);
int addTwo(int number);
char addChar(char character);

template<typename T> int farm(T (*worker)(T), int arr_len, int* input_arr, int NUM_THREADS = MAX_THREADS);

template<typename T> void f(T s);

