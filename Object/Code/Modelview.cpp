#include "Modelview.h"

glm::mat4 Topdown_CTM(float boundary[6], glm::vec3 position) {
	float H = boundary[5] - boundary[4];
	float W = boundary[1] - boundary[0];
	//	cout << "H is" << H << "W is" << W << endl;
	float halfH = H / 2.0f;
	float halfW = W / 2.0f;

	glm::mat4 view;

	if (position.z > boundary[5] - halfW) {
		glm::vec3 camera = glm::vec3(0.0f, 3.0f, boundary[5] - halfW);
		glm::vec3 look = glm::vec3(0.0f, 0.0f, boundary[5] - halfW);
		glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
		view = glm::lookAt(camera, look, up);
	}
	else if (position.z < boundary[4] + halfW) {
		glm::vec3 camera = glm::vec3(0.0f, 3.0f, boundary[4] + halfW);
		glm::vec3 look = glm::vec3(0.0f, 0.0f, boundary[4] + halfW);
		glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
		view = glm::lookAt(camera, look, up);
	}
	else {
		glm::vec3 camera = glm::vec3(0.0f, 3.0f, position.z);
		glm::vec3 look = glm::vec3(0.0f, 0.0f, position.z);
		glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
		view = glm::lookAt(camera, look, up);
	}
	return view;
}

glm::mat4 Topdown_Projection(float boundary[6], glm::vec3 position) {
	float H = boundary[5] - boundary[4];
	float W = boundary[1] - boundary[0];
	//	cout << "H is" << H << "W is" << W << endl;
	float halfH = H / 2.0f;
	float halfW = W / 2.0f;

	glm::mat4 projection;

	if (position.z > boundary[5] - halfW) {
		glm::vec3 camera = glm::vec3(0.0f, 3.0f, boundary[5] - halfW);
		glm::vec3 look = glm::vec3(0.0f, 0.0f, boundary[5] - halfW);
		glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
		projection = glm::ortho(-halfW, halfW, -halfW, halfW, 1.0f, 3.0f);
	}
	else if (position.z < boundary[4] + halfW) {
		glm::vec3 camera = glm::vec3(0.0f, 3.0f, boundary[4] + halfW);
		glm::vec3 look = glm::vec3(0.0f, 0.0f, boundary[4] + halfW);
		glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
		projection = glm::ortho(-halfW, halfW, -halfW, halfW, 1.0f, 3.0f);
	}
	else {
		glm::vec3 camera = glm::vec3(0.0f, 3.0f, position.z);
		glm::vec3 look = glm::vec3(0.0f, 0.0f, position.z);
		glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
		projection = glm::ortho(-halfW, halfW, -halfW, halfW, 1.0f, 3.0f);
	}
	return projection;
}


glm::mat4 Perspective_1st_CTM(glm::vec3 position, float boundary[6],float theta){
	glm::mat4 r(1.0f);
	r = glm::rotate(r, glm::radians(theta), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec4 camera_pos_rel = glm::vec4(0.0f, 0.0f, boundary[4] -0.2f,0.0f);
	glm::vec4 eye_rel = glm::vec4(0.0f, 0.0f, boundary[4] -1.0f, 0.0f);
	glm::vec3 camera = position + glm::vec3(r * camera_pos_rel);
	glm::vec3 eye = position + glm::vec3(r * eye_rel);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 view = glm::lookAt(camera, eye, up);
	return view  ;
}

glm::mat4 Perspective_1st_Projection(glm::vec3 position, float boundary[6], float theta) {
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 20.0f);
	return projection;
}

glm::mat4 Perspective_3rd_CTM(glm::vec3 position){
	glm::vec3 camera_pos_rel = glm::vec3(0.0f, 1.0f, 3.0f);
	glm::vec3 camera = position + camera_pos_rel;
	glm::vec3 look_rel = glm::vec3(0.0f, 0.0f, -0.5f);
	glm::vec3 look = position + look_rel;
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

	glm::mat4 view = glm::lookAt(camera, look, up);
	return view;
}
glm::mat4 Perspective_3rd_Projection(glm::vec3 position) {
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 20.0f);
	return projection;
}

