#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

const int IMAGE_SIZE = 10;
int image[IMAGE_SIZE][IMAGE_SIZE];

// Intermediate storage
int stg1[IMAGE_SIZE][IMAGE_SIZE];
int stg2[IMAGE_SIZE][IMAGE_SIZE];
int stg3[IMAGE_SIZE][IMAGE_SIZE];
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

int readImage(const char* name) {
	printf("Mock reading image with name %s\n", name);
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			image[i][j] = i * IMAGE_SIZE + j;
			printf("%d ", image[i][j]);
		}
		printf("\n");
	}
	return 1;
}

int addOneToImage() {
	printf("addOneToImage\n");
	printf("beep boop\n");
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			stg1[i][j] += 1;
			printf("%d ", stg1[i][j]);
		}
		printf("\n");
	}
	return 1;
}

int addFourToImage() {
	printf("addFourToImage\n");
	printf("beep boop\n");
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			stg2[i][j] += 4;
			printf("%d ", stg2[i][j]);
		}
		printf("\n");
	}
	return 1;
}

int sumImage() {
	printf("sumImage\n");
	printf("beep boop\n");
	int sum = 0;
	for (int i = 0; i < IMAGE_SIZE; i++) {
		for (int j = 0; j < IMAGE_SIZE; j++) {
			sum += stg3[i][j];
		}
	}
	printf("sum is: %d\n", sum);
	return 1;
}


int main() {

	Init();
	SetPipelineRuns(2);
	AddStage(image, IMAGE_SIZE * IMAGE_SIZE, image, readImage, "Dancho");
	AddStage(stg1, IMAGE_SIZE * IMAGE_SIZE, stg1, addOneToImage);
	AddStage(stg2, IMAGE_SIZE * IMAGE_SIZE, stg2, addFourToImage);
	AddStage(stg3, IMAGE_SIZE * IMAGE_SIZE, final_res, sumImage);
	//AddStage(6, IMAGE_SIZE * IMAGE_SIZE, readImage, "Dancho");
	//AddStage(IMAGE_SIZE * IMAGE_SIZE, 1, addOneToImage);
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
