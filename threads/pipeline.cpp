#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

int stage1(int a, int b) {
	printf("In stage1 with %d and %d\n", a, b);
	return a + b;
}

int stage2(int a, int b) {
	printf("In stage2 with %d and %d\n", a, b);
	return a - b;
}

int main() {

	Init();
	AddStage(stage1, 1, 2);
	AddStage(stage2, 2, 3);
	RunPipeline();
	return 0;
}
