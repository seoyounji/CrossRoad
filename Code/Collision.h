#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

bool Collision(glm::vec3 position1, float boundary1[6],  glm::vec3 position2, float boundary2[6]);
bool MapCollisionx(float mapboundary[6], glm::vec3 position, float boundary[6], int dir);
bool MapCollisionz(float mapboundary[6], glm::vec3 position, float boundary[6], int dir);
glm::vec3 Reposition(glm::vec3 activeP, float activeboundary[6], glm::vec3 staticP, float staticboundary[6]);