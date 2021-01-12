// library.cpp

#include "library.h"
#include <iostream>
#include <pthread.h>

#define NUM_THREADS 5

int addOne(int number){
    printf("Adding one to %d\n", number);
    return number + 1;
}

int farm(int (*worker)(int), int arr_len, int* input_arr) {
    printf("In farm\n");
    //int * result = (int*)malloc(arr_len * sizeof(int));
    
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], &attr, worker, NULL);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);

    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_join(threads[t], NULL);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld\n", t);
    }

    /* Last thing that main() should do */
    pthread_exit(NULL);

    return 1
    //return EXIT_SUCCESS;
    
    /*int result[arr_len];
    for (int i = 0; i < arr_len; i++) {
        result[i] = (*worker)(input_arr[i]);
    }
    for (int i = 0; i < arr_len; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");
    //free(result);

    return 1;*/
}

