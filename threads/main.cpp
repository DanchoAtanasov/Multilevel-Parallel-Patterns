#include <iostream>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 5

void * printHello(void * threadid){
	long tid = (long)threadid;
	cout << "Hellow World from thread " << tid << endl;
	pthread_exit(NULL);
}

int main(int argc, char * argv[]){

	pthread_t threads[NUM_THREADS];
	int rc;
	int i;

	for(i = 0; i < NUM_THREADS; i++){
		cout << "Main creating thread " << i << endl;
		rc = pthread_create(&threads[i], NULL, printHello, (void *)i);

		if (rc) {
			cout << "error in creating thread" << endl;
			exit(-1);
		}
	}
	pthread_exit(NULL);
	return 0;
}

