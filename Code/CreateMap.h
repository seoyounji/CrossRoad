#pragma once
#include <iostream>
#include <vector>


using namespace std;

struct Model_prop {
	int NofV;
	float* vertexptr;
	float* colorptr;
	float* texcoordptr;
	float* normalptr;
	float* tangentptr;
	float boundary[6];
};

struct Map_prop {
	int * map;
	int ** tree;
	int * fieldindex;
	int * tree_number_arr;
	int width;
	int height;
	int NofField;
	int NofTree;
	Model_prop map_model;
};

void CreateMap(Map_prop* map_prop);
void DestructMap(Map_prop* map_prop);
void Print_map_prop(Map_prop maprop);
Model_prop Create_map_buffer(Map_prop* map_prop);