#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

int stage1(int a) {
	printf("In stage1 with %d\n", a);
	return a + 1;
}

int stage2(int a) {
	printf("In stage2 with %d\n", a);
	return a + 2;
}

int stage3(int a) {
	printf("In stage3 with %d\n", a);
	return a + 3;
}

int stage4(int a) {
	printf("In stage4 with %d\n", a);
	return a + 4;
}


int main() {

	Init();
	AddStage(stage1, 1);
	AddStage(stage2, 2);
	AddStage(stage3, 3);
	AddStage(stage4, 4);
	RunPipeline();
	Finish();
	return 0;
}
