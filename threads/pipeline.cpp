#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

int stage0(int a) {
	printf("In stage0 with %d\n", a);
	return a + 1;
}

int stage1(int a) {
	printf("In stage1 with %d\n", a);
	int b = 0;
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


int main() {

	Init();
	SetPipelineRuns(5);
	AddStage(1, stage0, 1);
	AddStage(1, stage1);
	AddStage(1, stage2);
	AddStage(1, stage3);
	Finish();
	return 0;
}
