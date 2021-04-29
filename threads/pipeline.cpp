#include <iostream>
#include<unistd.h>
#include "../src/mlpplib.h"

const unsigned int microsecond = 1000000;

const int NODES = 4;

const int IMAGE_SIZE = 10;
int image[IMAGE_SIZE][IMAGE_SIZE];
int file_idx = 0;

// Intermediate storage
int stg1[IMAGE_SIZE][IMAGE_SIZE];
int stg2[IMAGE_SIZE][IMAGE_SIZE];
int stg3[IMAGE_SIZE][IMAGE_SIZE];
int final_sum;


int readImage(int start, int end, char** filenames) {
	char* name = filenames[file_idx];
	//file_idx++;  // TODO without this it will read only one file, with the threads will break it
	printf("Mock reading next image\n");
	for (int i = start; i < end; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			image[i][j] = i * IMAGE_SIZE + j;
			//printf("%d ", image[i][j]);
		}
		//printf("\n");
	}
        usleep(3.0 * microsecond);//sleeps 
	return 1;
}

int addOneToImage(int start, int end) {
	printf("addOneToImage\n");
	for (int i = start; i < end; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			stg1[i][j] += 1;
			//printf("%d ", stg1[i][j]);
		}
		//printf("\n");
	}
	return 1;
}

int addFourToImage(int start, int end) {
	printf("addFourToImage\n");
	for (int i = start; i < end; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			stg2[i][j] += 4;
			//printf("%d ", stg2[i][j]);
		}
		//printf("\n");
	}
        //usleep(0.5 * microsecond);//sleeps 
	return 1;
}

int sumImage(int start, int end) {
	final_sum = 0;
	printf("sumImage\n");
	for (int i = start; i < end; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			final_sum += stg3[i][j];
		}
	}
        usleep(1.5 * microsecond);//sleeps 
	return 1;
}


int main() {

	char * filenames[] = {"file0.txt", "file1.txt", "file2.txt"};
	Init();
	SetPipelineRuns(3); // Set how many times to run pipeline

	// Add as many stages as there are nodes
	AddStage(2, IMAGE_SIZE, &final_sum, 1, image, IMAGE_SIZE * IMAGE_SIZE, readImage, filenames);
	AddStage(2, IMAGE_SIZE, stg1, IMAGE_SIZE * IMAGE_SIZE, stg1, IMAGE_SIZE * IMAGE_SIZE, addOneToImage);
	AddStage(2, IMAGE_SIZE, stg2, IMAGE_SIZE * IMAGE_SIZE, stg2, IMAGE_SIZE * IMAGE_SIZE, addFourToImage);
	AddStage(1, IMAGE_SIZE, stg3, IMAGE_SIZE * IMAGE_SIZE, &final_sum, 1, sumImage);
	
	Finish();
	printf("sum is: %d\n", final_sum);

	return 0;
}
