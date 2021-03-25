#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;
int b[] = { 1, 2, 3 };

int* stage0(int a) {
	printf("In stage0 with %d\n", a);
	return b;
}

int stage1(int* a) {
	printf("In stage1 with %d\n", a[0]);
	int b = 0;
	for (int i=0;i<10000000;i++) b += i;
	return 2;
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


int main() {

	Init();
	SetPipelineRuns(2);
	AddStage(1, 3, stage0, 1);
	AddStage(3, 1, stage1);
	AddStage(1, 1, stage2);
	AddStage(1, 1, stage3);
	Finish();
	return 0;
}
