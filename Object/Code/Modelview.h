#pragma once
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
using namespace std;

glm::mat4 Topdown_CTM(float boundary[6], glm::vec3 position);
glm::mat4 Topdown_Projection(float boundary[6], glm::vec3 position);
glm::mat4 Perspective_1st_CTM(glm::vec3 position, float boundary[6], float theta);
glm::mat4 Perspective_1st_Projection(glm::vec3 position, float boundary[6], float theta);
glm::mat4 Perspective_3rd_CTM(glm::vec3 position);
glm::mat4 Perspective_3rd_Projection(glm::vec3 position);
