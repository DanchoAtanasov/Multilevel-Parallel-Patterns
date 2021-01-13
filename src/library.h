// library.h

int addOne(int number);
int addTwo(int number);

int farm(int (*worker)(int), int arr_len, int* input_arr, const int NUM_THREADS);
