#include <iostream>
#include "../src/mlpplib.h"

const int NODES = 4;

int stage1(int a, int b) {
	return a + b;
}

int stage2(int a, int b) {
	return a - b;
}

int main() {

	AddStage(stage1, 1, 2);
	AddStage(stage2, 2, 3);
	//RunPipeline();
	return 0;
}
