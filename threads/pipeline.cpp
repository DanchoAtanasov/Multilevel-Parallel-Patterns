#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

int stage1(int a) {
	printf("In stage1 with %d\n", a);
	return a;
}

int stage2(int a) {
	printf("In stage2 with %d\n", a);
	return a;
}

int main() {

	Init();
	AddStage(stage1, 1);
	AddStage(stage2, 2);
	RunPipeline();
	return 0;
}
