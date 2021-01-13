// library.h

const int MAX_THREADS = 16;

int addOne(int number);
int addTwo(int number);

int farm(int (*worker)(int), int arr_len, int* input_arr, int NUM_THREADS = MAX_THREADS);
