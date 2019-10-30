#pragma once

#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp> //glm::mat4
#include <glm/vec4.hpp> //glm::vec4
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>// glm::translate,rotate,scale,perspective

#include "CreateMap.h"

//class
class Animation_obj
{
public:
	string name;
	glm::vec3 position;
	glm::vec3 vel;
	glm::vec3 color;
	float theta_y;
	float theta_x;
	Model_prop * model;
	float boundary[6];
	bool activation;
	float rotvel;
	int obj_index;
	Animation_obj* Parent;
	Animation_obj* Son;
	glm::mat4 rel_motion;

	//functions
	Animation_obj();
	~Animation_obj();
	void AddSon(Animation_obj input);
	glm::mat4 Animation_matrix();

private:
	int Son_max;
	int NofSon;

};

//functions

void Animation1(Animation_obj* Son, int index1, int index2, bool move);