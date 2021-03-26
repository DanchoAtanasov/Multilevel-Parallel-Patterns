#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

const int IMAGE_SIZE = 10;
int image[IMAGE_SIZE][IMAGE_SIZE];

// Intermediate storage
int stg1[IMAGE_SIZE][IMAGE_SIZE];
int final_res[IMAGE_SIZE][IMAGE_SIZE];

int stage0(int a) {
	printf("In stage0 with %d\n", a);
	return a + 1;
}

int stage1(int a) {
	printf("In stage1 with %d\n", a);
	int b = 0;
	// Make it take longer
	for (int i=0;i<10000000;i++) b += i;
	return a + 2;
}

float stage2(int a) {
	printf("In stage2 with %d\n", a);
	return 10.0f;
	//return a + 3;
}

int stage3(float a) {
	printf("In stage3 with %d\n", a);
	return int(a) + 4;
}

int readImage(char* name) {
	printf("Mock reading image with name %s\n", name);
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			image[i][j] = i * IMAGE_SIZE + j;
		}
	}
	return 1
}

int processImage() {
	printf("Processing image\n");
	printf("beep boop\n");
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			stg1[i][j] += 5;
		}
	}
	return 1;
}


int main() {

	Init();
	SetPipelineRuns(1);
	AddStage(image, IMAGE_SIZE * IMAGE_SIZE, stg1, readImage, "Dancho");
	AddStage(stg1, IMAGE_SIZE * IMAGE_SIZE, final_res, processImage);
	//AddStage(6, IMAGE_SIZE * IMAGE_SIZE, readImage, "Dancho");
	//AddStage(IMAGE_SIZE * IMAGE_SIZE, 1, processImage);
	//AddStage(1, 1, stage0, 1);
	//AddStage(1, 1, stage1);
	//AddStage(1, 1, stage2);
	//AddStage(1, 1, stage3);
	Finish();
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			printf("%d", final_res[i][j]);
		}
		printf("\n");
	}

	return 0;
}
