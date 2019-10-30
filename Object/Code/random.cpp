#include "random.h"


int Random(int start, int end) {

	random_device rn;
	mt19937_64 rnd(rn());

	uniform_int_distribution<int> region(0, 100000);
	int N = start;
	int M = end + 1 - N;
	return region(rnd) % M + N;
}
int Random(const int arr[], int length) {
	return arr[Random(0, length - 1)];
}
int * RandomArray(int length) {
	//return int witch satisfy i<length;
	int *arrindex = new int[length];
	for (int i = 0; i < length; i++) {
		arrindex[i] = i;
	}
	for (int i = 0; i < length; i++) {
		int chosen1 = Random(0, length - 1);
		int chosen2 = Random(0, length - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	for (int i = 0; i < length; i++) {
		int chosen1 = length - 1 - Random(0, length - 1);
		int chosen2 = length - 1 - Random(0, length - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	for (int i = 0; i < length; i++) {
		int chosen1 = Random(0, length - 1);
		int chosen2 = length - 1 - Random(0, length - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	return arrindex;
}

int Random(int length) {
	//return random int witch satisfy i<length;
	int *arrindex = new int[length];
	int output;
	for (int i = 0; i < length; i++) {
		arrindex[i] = i;
	}
	for (int i = 0; i < length; i++) {
		int chosen1 = Random(0, length - 1);
		int chosen2 = Random(0, length - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	for (int i = 0; i < length; i++) {
		int chosen1 = length - 1 - Random(0, length - 1);
		int chosen2 = length - 1 - Random(0, length - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	for (int i = 0; i < length; i++) {
		int chosen1 = Random(0, length - 1);
		int chosen2 = length - 1 - Random(0, length - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	output = arrindex[0];
	delete arrindex;
	return output;
}

void ChooseRand(int * arr, int length_of_arr, int num_tobe_chosen) {
	int *arrindex = new int[length_of_arr];
	for (int i = 0; i < length_of_arr; i++) {
		arrindex[i] = i;
	}
	//randomly mix indexes of array. My ramdon function seems to choose low number more frequently. Below 3 loops are provided to compensate it's property. 
	for (int i = 0; i < length_of_arr; i++) {
		int chosen1 = Random(0, length_of_arr - 1);
		int chosen2 = Random(0, length_of_arr - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	for (int i = 0; i < length_of_arr; i++) {
		int chosen1 = length_of_arr - 1 - Random(0, length_of_arr - 1);
		int chosen2 = length_of_arr - 1 - Random(0, length_of_arr - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	for (int i = 0; i < length_of_arr; i++) {
		int chosen1 = Random(0, length_of_arr - 1);
		int chosen2 = length_of_arr - 1 - Random(0, length_of_arr - 1);
		int tmp = arrindex[chosen1];
		arrindex[chosen1] = arrindex[chosen2];
		arrindex[chosen2] = tmp;
	}
	// assign 1 to chosen index. 1 means there is a tree.
	for (int i = 0; i < num_tobe_chosen; i++) {
		int index = arrindex[i];
		arr[index] = 1;
	}
	delete(arrindex);
}
