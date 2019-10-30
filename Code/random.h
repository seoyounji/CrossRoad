#pragma once
#include <iostream>
#include <random>

using namespace std;

int Random(int start, int end);
int Random(const int arr[], int length);
int * RandomArray(int length);
void ChooseRand(int * ptr, int length, int num);
int Random(int length);