#include "animation.h"

Animation_obj::Animation_obj()
{
	name = "NoName";
	position = glm::vec3(0.0f);
	vel = glm::vec3(0.0f);
	color = glm::vec3(0.0f);
	theta_x = 0.0f;
	theta_y = 0.0f;
	model = nullptr;
	activation = FALSE;
	rotvel = 0.0f;
	obj_index = 10;
	for (int i = 0; i < 6; i++) {
		boundary[i] = 0.0f;
	}
	Parent = nullptr;
	Son = nullptr;
	rel_motion = glm::mat4(1.0f);

	NofSon = 0;
	Son_max = 10;

}
void Animation_obj::AddSon(Animation_obj input) {
	if (Son == nullptr) {
		Son = new Animation_obj[Son_max];
	}
	Son[NofSon] = input;
	Son[NofSon].activation = TRUE;
	Son[NofSon].Parent = this;

	NofSon++;
}

glm::mat4 Animation_obj::Animation_matrix() {
	glm::mat4 m = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(m, position);
	glm::mat4 rotate = glm::rotate(m, glm::radians(theta_y), glm::vec3(0.0f, 1.0f, 0.0f));
	if (Parent == nullptr) {
		return translate * rotate * rel_motion;
	}
	else {
		return Parent->Animation_matrix() * translate * rotate * rel_motion;
	}
}

Animation_obj::~Animation_obj()
{
	//Delete_recursive();
	if (Son == nullptr)
		delete Son;
}


void Animation1(Animation_obj* Son,int index1, int index2,bool move) {
	//must be used to Animation_obj which has 2 Son object which has 1 son objects. 
	Son[index1].rotvel = 24.0f;
	Son[index2].rotvel = 24.0f;
	Son[index1].Son[0].rotvel = 12.0f;
	Son[index2].Son[0].rotvel = 12.0f;

	if (move) {
		//first arm movement
		if (Son[index1].theta_x < 200.0f)
			Son[index1].theta_x += Son[index1].rotvel;
		else 
			Son[index1].theta_x = 200.0f;
		
		if (Son[index2].theta_x < 200.0f) 
			Son[index2].theta_x += Son[index2].rotvel;
		else 
			Son[index2].theta_x = 200.0f;
		//second arm movement
		if (Son[index1].Son[0].theta_x > -45.0f)
			Son[index1].Son[0].theta_x -= Son[index1].Son[0].rotvel;
		else
			Son[index1].Son[0].theta_x = -45.0f;
		if (Son[index2].Son[0].theta_x > -45.0f)
			Son[index2].Son[0].theta_x -= Son[index2].Son[0].rotvel;
		else
			Son[index2].Son[0].theta_x = -45.0f;
	}
	else {
		//first arm movement
		if (Son[index1].theta_x > 0.0f)
			Son[index1].theta_x -= Son[index1].rotvel;
		else
			Son[index1].theta_x = 0.0f;

		if (Son[index2].theta_x > 0.0f)
			Son[index2].theta_x -= Son[index2].rotvel;
		else
			Son[index2].theta_x = 0.0f;
		//second arm movement
		if (Son[index1].Son[0].theta_x < 0.0f)
			Son[index1].Son[0].theta_x += Son[index1].Son[0].rotvel;
		else
			Son[index1].Son[0].theta_x = 0.0f;
		if (Son[index2].Son[0].theta_x < 0.0f)
			Son[index2].Son[0].theta_x += Son[index2].Son[0].rotvel;
		else
			Son[index2].Son[0].theta_x = 0.0f;
	}
	Son[index1].rel_motion = glm::rotate(glm::mat4(1.0f), glm::radians(Son[index1].theta_x), glm::vec3(1.0f, 0.0f, 0.0f));
	Son[index2].rel_motion = glm::rotate(glm::mat4(1.0f), glm::radians(Son[index2].theta_x), glm::vec3(1.0f, 0.0f, 0.0f));
	Son[index1].Son[0].rel_motion = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f + Son[index1].Son[0].theta_x), glm::vec3(1.0f, 0.0f, 0.0f));
	Son[index2].Son[0].rel_motion = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f + Son[index2].Son[0].theta_x), glm::vec3(1.0f, 0.0f, 0.0f));
}