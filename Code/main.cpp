#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp> //glm::mat4
#include <glm/vec4.hpp> //glm::vec4
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>// glm::translate,rotate,scale,perspective
///////////////////////////////////
#include "obj_loader.h"
#include "InitializeShader.h"
#include "Modelview.h"
#include "CreateMap.h"
#include "random.h"
#include "Collision.h"
#include "animation.h"
#include "Imageloader.h"

using namespace std;

//variables
int winid;
unsigned short int wingame = 0;
float sizeup = 1.6f; //map is generated with int variables. This parameter size up the maps by it's magnitude.
bool player_move = FALSE;

struct light {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 positon;
};
struct optical_prop {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shine;
};

string policecar_s = "policecar";
string taxi_s = "taxi";
string truck_s = "truck";
string christmastree_s = "christmastree";
string player_s = "player";
string arm_right_s = "arm_right";
string arm_left_s = "arm_left";
string vertex_s = "v";
string color_s = "c";
string normal_s = "n";

Model_prop policecar_m;
Model_prop christmastree_m;
Model_prop taxi_m;
Model_prop truck_m;
Model_prop player_m;
Model_prop arm_right_m;
Model_prop arm_left_m;

Animation_obj* animationlist;
int size_of_animationlist;

GLuint shaderprogram_ic;
GLuint shaderprogram_tex;
GLuint vao;
GLuint map_buffer[3];
GLuint player_buffer[3];
GLuint arm_right_buffer[3];
GLuint arm_left_buffer[3];

GLuint objects_buffer[4]; // tree : 0, policecar : 1, taxi : 2, truck : 3
GLuint objects_normal_buffer[4];
GLuint objects_tangent_buffer[4];
GLuint objects_texcoord_buffer[4];
GLuint objects_texture[4];
GLuint objects_norm_texture[4];
GLubyte* img_ptr[4];
GLubyte* img_norm_ptr[4];

ILuint obj_img_name[4];
ILuint obj_norm_img_name[4];

Map_prop map;

glm::mat4 MVP;
glm::mat4 Projection;
int perspective;

glm::vec3 light_rel_position = glm::vec3(0.0f, 1.0f, 0.0f);
light light_soruce1;
optical_prop object_optical[4];
optical_prop player_optical;
optical_prop map_optical;


//functions
//mathmatical functions
float sgn(float input) {
	int out = (input > 0) - (input < 0);
	return float(out);
}
float AngleRelax(float input) {
	float theta = input;
	while (theta < 0) {
		theta += 360.0f;
	}
	while (theta - 360.0 > 0) {
		theta -= 360.0f;
	}
	return theta;
}

bool AngleCheckDegree(float theta, float check, float accuracy) {
	float target = AngleRelax(theta) - AngleRelax(check);

	if (target < accuracy && target > -accuracy) {
		return TRUE;
	}
	else {
		return FALSE;
	}

}

void BoundaryRotation(Animation_obj* obj) {
	float xmin = obj->model->boundary[0];
	float xmax = obj->model->boundary[1];
	float ymin = obj->model->boundary[2];
	float ymax = obj->model->boundary[3];
	float zmin = obj->model->boundary[4];
	float zmax = obj->model->boundary[5];
	float theta_y = obj->theta_y;
	theta_y = AngleRelax(theta_y);

	float test = theta_y / 90.0f;
	if ( test< 0.5){
		obj->boundary[0] = xmin;
		obj->boundary[1] = xmax;
		obj->boundary[2] = ymin;
		obj->boundary[3] = ymax;
		obj->boundary[4] = zmin;
		obj->boundary[5] = zmax;
	}
	else if (test < 1.5) {
		obj->boundary[0] = zmin;
		obj->boundary[1] = zmax;
		obj->boundary[2] = ymin;
		obj->boundary[3] = ymax;
		obj->boundary[4] = -xmax;
		obj->boundary[5] = -xmin;
	}
	else if (test < 2.5) {
		obj->boundary[0] = -xmax;
		obj->boundary[1] = -xmin;
		obj->boundary[2] = ymin;
		obj->boundary[3] = ymax;
		obj->boundary[4] = -zmax;
		obj->boundary[5] = -zmin;
	}
	else {
		obj->boundary[0] = -zmax;
		obj->boundary[1] = -zmin;
		obj->boundary[2] = ymin;
		obj->boundary[3] = ymax;
		obj->boundary[4] = xmin;
		obj->boundary[5] = xmax;
	}

}

glm::vec4 multiply_vector(glm::vec4 a, glm::vec4 b) {
	return glm::vec4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

//other functions
glm::mat4 Set_CTM(int mode) {
	switch (mode){
	case 1:
		return Perspective_1st_CTM(animationlist[0].position, player_m.boundary, animationlist[0].theta_y); break;
	case 2:
		return Perspective_3rd_CTM(animationlist[0].position); break;
	case 3:
		return Topdown_CTM(map.map_model.boundary, animationlist[0].position); break;
	default :
		return Perspective_3rd_CTM(animationlist[0].position);
	}
}
glm::mat4 Set_Projection(int mode) {
	switch (mode) {
	case 1:
		return Perspective_1st_Projection(animationlist[0].position, player_m.boundary, animationlist[0].theta_y); break;
	case 2:
		return Perspective_3rd_Projection(animationlist[0].position); break;
	case 3:
		return Topdown_Projection(map.map_model.boundary, animationlist[0].position); break;
	default:
		return Perspective_3rd_Projection(animationlist[0].position);
	}
}

GLuint FindBuffer(string name, string vc) {
	string c = "c";
	string v = "v";
	string n = "n";
	if (! name.compare(player_s)) {
		if (! vc.compare(v)) {
			return player_buffer[0];
		}
		if (! vc.compare(c)) {
			return player_buffer[1];
		}
		if (!vc.compare(n)) {
			return player_buffer[2];
		}
	}
	if (!name.compare(arm_right_s)) {
		if (!vc.compare(v)) {
			return arm_right_buffer[0];
		}
		if (!vc.compare(c)) {
			return arm_right_buffer[1];
		}
		if (!vc.compare(n)) {
			return arm_right_buffer[2];
		}
	}
	if (!name.compare(arm_left_s)) {
		if (!vc.compare(v)) {
			return arm_left_buffer[0];
		}
		if (!vc.compare(c)) {
			return arm_left_buffer[1];
		}
		if (!vc.compare(n)) {
			return arm_left_buffer[2];
		}
	}
	cout << "FindBuffer() noreturn" << endl;
}
Model_prop CreatePlayerVertex() {
	Model_prop out;
	int tri = 15;
	int NofV = 15 * 3;
	int NofElement = NofV * 3;
	float * playervertex = new float[NofElement];
	float * playercolor = new float[NofElement];
	float * playernormal = new float[NofElement];
	float len = 0.25f;
	glm::vec4 square[6][6];
	glm::vec4 arrow[9];
	glm::vec4 normal[6];
	normal[0] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	square[0][0] = glm::vec4(-len, len, -len, 1.0f);
	square[0][1] = glm::vec4(-len, len, len, 1.0f);
	square[0][2] = glm::vec4(len, len, -len, 1.0f);

	square[0][3] = glm::vec4(len, len, -len, 1.0f);
	square[0][4] = glm::vec4(-len, len, len, 1.0f);
	square[0][5] = glm::vec4(len, len, len, 1.0f);

	glm::mat4 ro1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 ro2 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ro3 = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	for (int i = 0; i < 6; i++) {
		square[1][i] = ro1 * square[0][i]; ro1 * normal[0];
		square[2][i] = ro1 * square[1][i]; ro1 * normal[1];
		square[3][i] = ro1 * square[2][i]; ro1 * normal[2];
		square[4][i] = ro2 * square[0][i]; ro2 * normal[0];
		square[5][i] = ro3 * square[0][i]; ro3 * normal[0];
	}

	arrow[0] = glm::vec4( 0.0f * len, len + 0.01f,-0.8f * len, 1.0f);
	arrow[1] = glm::vec4(-0.5f * len, len + 0.01f, 0.0f	* len, 1.0f);
	arrow[2] = glm::vec4( 0.5f * len, len + 0.01f, 0.0f	* len, 1.0f);

	arrow[3] = glm::vec4(-0.25f * len, len + 0.01f, 0.0f * len, 1.0f);
	arrow[4] = glm::vec4(-0.25f * len, len + 0.01f, 0.8f * len, 1.0f);
	arrow[5] = glm::vec4( 0.25f * len, len + 0.01f, 0.0f * len, 1.0f);

	arrow[6] = glm::vec4( 0.25f * len, len + 0.01f, 0.0f * len, 1.0f);
	arrow[7] = glm::vec4(-0.25f * len, len + 0.01f, 0.8f * len, 1.0f);
	arrow[8] = glm::vec4( 0.25f * len, len + 0.01f, 0.8f * len, 1.0f);

	int index = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			playervertex[index] = square[i][j].x; playercolor[index] = (float) 1.0f;  playernormal[index] = normal[i].x; index++;
			playervertex[index] = square[i][j].y; playercolor[index] = (float) 0.72f; playernormal[index] = normal[i].y; index++;
			playervertex[index] = square[i][j].z; playercolor[index] = (float) 0.27f; playernormal[index] = normal[i].z; index++;
		}
	}
	for (int i = 0; i < 9; i++) {
		playervertex[index] = arrow[i].x; playercolor[index] = 1.0f; playernormal[index] = normal[0].x; index++;
		playervertex[index] = arrow[i].y; playercolor[index] = 0.0f; playernormal[index] = normal[0].y; index++;
		playervertex[index] = arrow[i].z; playercolor[index] = 0.0f; playernormal[index] = normal[0].z; index++;
	}

	out.NofV = NofV;
	out.vertexptr = playervertex;
	out.colorptr = playercolor;
	out.texcoordptr=nullptr;
	out.normalptr = playernormal;
	return out;
}
Model_prop CreatePlayerArmVertex(int dir) {
	Model_prop out;
	int tri = 12;
	int NofV = 3 * tri;
	int NofElement = 3 * NofV;
	float * playervertex = new float[NofElement];
	float * playercolor = new float[NofElement];
	float * playernormal = new float[NofElement];
	float len = 0.25f;
	glm::vec4 square[6][6];
	glm::vec4 normal[6];
	normal[0] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	square[0][0] = glm::vec4(-len, len, -len, 1.0f);
	square[0][1] = glm::vec4(-len, len, len, 1.0f);
	square[0][2] = glm::vec4(len, len, -len, 1.0f);

	square[0][3] = glm::vec4(len, len, -len, 1.0f);
	square[0][4] = glm::vec4(-len, len, len, 1.0f);
	square[0][5] = glm::vec4(len, len, len, 1.0f);

	glm::mat4 ro1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 ro2 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ro3 = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 scale1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, 0.3f));
	glm::mat4 scale2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 0.3f));
	glm::mat4 scale3 = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.75f, 0.0f));
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(len*0.05f, -len * 0.375f, 0.0f));
	glm::mat4 left = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (int i = 0; i < 6; i++) {
		square[1][i] = ro1 * square[0][i]; ro1 * normal[0];
		square[2][i] = ro1 * square[1][i]; ro1 * normal[1];
		square[3][i] = ro1 * square[2][i]; ro1 * normal[2];
		square[4][i] = ro2 * square[0][i]; ro2 * normal[0];
		square[5][i] = ro3 * square[0][i]; ro3 * normal[0];
	}
	for (int i = 0; i < 6; i++) {
		square[0][i] = trans * scale1 * square[0][i];
		square[1][i] = trans * scale2 * square[1][i];
		square[2][i] = trans * scale1 * square[2][i];
		square[3][i] = trans * scale2 * square[3][i];
		square[4][i] = trans * scale3 * square[4][i];
		square[5][i] = trans * scale3 * square[5][i];
	}
	if (dir == -1) {
		for (int i = 0; i < 6; i++) {
			square[0][i] = left * square[0][i]; left * normal[0];
			square[1][i] = left * square[1][i]; left * normal[1];
			square[2][i] = left * square[2][i]; left * normal[2];
			square[3][i] = left * square[3][i]; left * normal[3];
			square[4][i] = left * square[4][i]; left * normal[4];
			square[5][i] = left * square[5][i]; left * normal[5];
		}
	}

	int index = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			playervertex[index] = square[i][j].x; playercolor[index] = (float) 0.905f; playernormal[index] = normal[i].x; index++;
			playervertex[index] = square[i][j].y; playercolor[index] = (float) 0.517f; playernormal[index] = normal[i].y; index++;
			playervertex[index] = square[i][j].z; playercolor[index] = (float) 0.627f; playernormal[index] = normal[i].z; index++;
		}
	}
	out.NofV = NofV;
	out.vertexptr = playervertex;
	out.colorptr = playercolor;
	out.texcoordptr = nullptr;
	out.normalptr = playernormal;

	return out;
}
string car_string(int input) {
	switch (input) {
	case 1:return policecar_s; break;
	case 2:return taxi_s; break;
	case 3:return truck_s; break;
	}
	cout << "car_string() noreturn" << endl;
}
Model_prop * car_model(int input) {
	switch (input) {
	case 1:return &policecar_m; break;
	case 2:return &taxi_m; break;
	case 3:return &truck_m; break;
	}
	cout << "car_model() noreturn" << endl;
}
void Create_Player_Animation_obj(Animation_obj * player) {
	float len = 0.25f;
	player->name = player_s;
	player->position = glm::vec3(0.0f, 0.1 + len, 0.0f);
	player->model = &player_m;
	player->name = player_s;
	player->activation = TRUE;

	Animation_obj arm1 = Animation_obj();
	arm1.position = glm::vec3(len, len - 0.05, 0.0f);
	arm1.model = &arm_right_m;
	arm1.name = arm_right_s;
	BoundaryRotation(&arm1);

	Animation_obj arm2 = Animation_obj();
	arm2.position = glm::vec3(len*0.05f, -len * 0.75f, 0.0f);
	arm2.model = &arm_right_m;
	arm2.name = arm_right_s;
	BoundaryRotation(&arm2);

	Animation_obj arm3 = Animation_obj();
	arm3.position = glm::vec3(-len, len - 0.05, 0.0f);
	arm3.model = &arm_left_m;
	arm3.name = arm_left_s;
	BoundaryRotation(&arm3);

	Animation_obj arm4 = Animation_obj();
	arm4.position = glm::vec3(-len*0.05f, -len * 0.75f, 0.0f);
	arm4.model = &arm_left_m;
	arm4.name = arm_left_s;
	BoundaryRotation(&arm4);
	player->AddSon(arm1);
	player->Son[0].AddSon(arm2);
	player->AddSon(arm3);
	player->Son[1].AddSon(arm4);

}
Animation_obj* Create_Animation_obj(Map_prop* map ) {
	
	Animation_obj* animation_list;
	int carperroad = map->width / 4;
	int NofAnimationObject = (map->height - map->NofField)*carperroad + map->NofTree + 1;
	size_of_animationlist = NofAnimationObject;
	animation_list = new Animation_obj[NofAnimationObject];
	if (animation_list==nullptr) {
		cout << "failed to construct animationlist" << endl;
	}
	int index = 0;
	int treeindex = 0;
	//zeros index is player
	Create_Player_Animation_obj(&animation_list[index]);
	index ++;

	//Initialize objects
	for (int i = 0; i < map->height; i++) {
		if (map->map[i] == 0) { //field. assign tree data.
			for (int j = 0; j < map->width; j++) {
				if (map->tree[treeindex][j] == 1) {
					animation_list[index].obj_index = 0;
					animation_list[index].vel = glm::vec3(0.0f, 0.0f, 0.0f);
					animation_list[index].color = glm::vec3(0.1f, 0.7f, 0.1f);
					animation_list[index].theta_y = 0.0f;
					animation_list[index].model = & christmastree_m;
					animation_list[index].name = christmastree_s;
					animation_list[index].position = glm::vec3((float)j - (float)(map->width) / 2.0f, 0.1f, -(float)i);
					animation_list[index].position = glm::vec3(sizeup*animation_list[index].position.x, animation_list[index].position.y, sizeup * animation_list[index].position.z);
					animation_list[index].activation = TRUE;
					index++; 
				}
			}
			treeindex++;
		}
		else { //cars data.
			int * ranarr = RandomArray(map->width);
			for (int j = 0; j < carperroad; j++) {
				int split = 1+Random(3);
				animation_list[index].obj_index = split;
				animation_list[index].vel = glm::vec3(map->map[i] * 0.1f, 0.0f, 0.0f);
				animation_list[index].color = glm::vec3(0.0f, 0.0f, 1.0f);
				animation_list[index].theta_y = map->map[i]*90.0f;
				animation_list[index].model = car_model(split);
				animation_list[index].name = car_string(split);
				animation_list[index].position = glm::vec3((float)ranarr[j] - (float)(map->width) / 2.0f, 0.1f, -(float)i);
				animation_list[index].position = glm::vec3(sizeup*animation_list[index].position.x, animation_list[index].position.y, sizeup * animation_list[index].position.z);
				animation_list[index].activation = TRUE;
				index++;
			}
			delete ranarr;
		}
	}
	for (int i = 0; i < NofAnimationObject; i++) {
		BoundaryRotation(&animation_list[i]);
	}
	
//	cout << "index is"<<index <<"NofAnimationObject is "<< NofAnimationObject << endl;
	return animation_list;
}

void Setboundary(Model_prop* modeling ) {
	modeling->boundary[0] = modeling->vertexptr[0];
	modeling->boundary[1] = modeling->vertexptr[0];
	modeling->boundary[2] = modeling->vertexptr[1];
	modeling->boundary[3] = modeling->vertexptr[1];
	modeling->boundary[4] = modeling->vertexptr[2];
	modeling->boundary[5] = modeling->vertexptr[2];

	for (int i = 0; i < modeling->NofV; i++) {
		if (modeling->boundary[0] > modeling->vertexptr[3 * i + 0]) {
			modeling->boundary[0] = modeling->vertexptr[3 * i + 0];
		}
		if (modeling->boundary[1] < modeling->vertexptr[3 * i + 0]) {
			modeling->boundary[1] = modeling->vertexptr[3 * i + 0];
		}
		if (modeling->boundary[2] > modeling->vertexptr[3 * i + 1]) {
			modeling->boundary[2] = modeling->vertexptr[3 * i + 1];
		}
		if (modeling->boundary[3] < modeling->vertexptr[3 * i + 1]) {
			modeling->boundary[3] = modeling->vertexptr[3 * i + 1];
		}
		if (modeling->boundary[4] > modeling->vertexptr[3 * i + 2]) {
			modeling->boundary[4] = modeling->vertexptr[3 * i + 2];
		}
		if (modeling->boundary[5] < modeling->vertexptr[3 * i + 2]) {
			modeling->boundary[5] = modeling->vertexptr[3 * i + 2];
		}
	}
}

void Init_modeling_obj() {
	cObj obj_policecar("./object/policecar/police_car.obj");
	cObj obj_christmastree("./object/christmastree/christmas_tree.obj");
	cObj obj_taxi("./object/taxi/taxi.obj");
	cObj obj_truck("./object/truck/truck_blue.obj");

	policecar_m.NofV= obj_policecar.NumberofV();
	policecar_m.vertexptr = obj_policecar.vertices_buffer_data;
	policecar_m.normalptr = obj_policecar.normal_data;
	policecar_m.tangentptr = obj_policecar.tangent_data;
	policecar_m.texcoordptr = obj_policecar.texcoord_data;
	policecar_m.colorptr = nullptr;
	taxi_m.NofV = obj_taxi.NumberofV();
	taxi_m.vertexptr = obj_taxi.vertices_buffer_data;
	taxi_m.normalptr = obj_taxi.normal_data;
	taxi_m.tangentptr = obj_taxi.tangent_data;
	taxi_m.texcoordptr = obj_taxi.texcoord_data;
	taxi_m.colorptr = nullptr;
	truck_m.NofV = obj_truck.NumberofV();
	truck_m.vertexptr = obj_truck.vertices_buffer_data;
	truck_m.normalptr = obj_truck.normal_data;
	truck_m.tangentptr = obj_truck.tangent_data;
	truck_m.texcoordptr = obj_truck.texcoord_data;
	truck_m.colorptr = nullptr;
	christmastree_m.NofV = obj_christmastree.NumberofV();
	christmastree_m.vertexptr = obj_christmastree.vertices_buffer_data;
	christmastree_m.normalptr = obj_christmastree.normal_data;
	christmastree_m.tangentptr = obj_christmastree.tangent_data;
	christmastree_m.texcoordptr = obj_christmastree.texcoord_data;
	christmastree_m.colorptr = nullptr;

	obj_policecar.~cObj();
	obj_christmastree.~cObj();
	obj_taxi.~cObj();
	obj_truck.~cObj();

	Setboundary(& policecar_m);
	Setboundary(& taxi_m);
	Setboundary(& truck_m);
	Setboundary(& christmastree_m);
	Setboundary(&map.map_model);
	Setboundary(&player_m);
	Setboundary(&arm_right_m);
	Setboundary(&arm_left_m);
}

void Init_Imgfile() {

	InitializeImgLoader();
	obj_img_name[0] = loadimage("./object/christmastree/christmas_tree-tex.tex.png");
	obj_img_name[1] = loadimage("./object/policecar/police_car-tex.tex.png");
	obj_img_name[2] = loadimage("./object/taxi/Taxi-tex.tex.png");
	obj_img_name[3] = loadimage("./object/truck/truck_blue-tex.tex.png");
	
	obj_norm_img_name[0] = loadimage("./object/christmastree/normal_map_for_tree.jpg");
	obj_norm_img_name[1] = loadimage("./object/policecar/normal_map_for_police_car.jpg");
	obj_norm_img_name[2] = loadimage("./object/taxi/normal_map_for_taxi.jpg");
	obj_norm_img_name[3] = loadimage("./object/truck/noraml_map_for_truck.jpg");

	for (int i = 0; i < 4; i++) {
		int NofElement = WidthOfImg(obj_img_name[i]) * HeightOfImg(obj_img_name[i])*4;
		img_ptr[i] = ImagePtr(obj_img_name[i]);
	}
	for (int i = 0; i < 4; i++) {
		int NofElement = WidthOfImg(obj_norm_img_name[i]) * HeightOfImg(obj_norm_img_name[i]) * 4;
		img_norm_ptr[i] = ImagePtr(obj_norm_img_name[i]);
	}

}

void Init_texture() {
	glEnable(GL_TEXTURE_2D);
	glGenTextures(4, objects_texture);
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, objects_texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WidthOfImg(obj_img_name[i]), HeightOfImg(obj_img_name[i]), 0, GL_RGBA, GL_UNSIGNED_BYTE, img_ptr[i]);
	}
	glGenTextures(4, objects_norm_texture);
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, objects_norm_texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WidthOfImg(obj_norm_img_name[i]), HeightOfImg(obj_norm_img_name[i]), 0, GL_RGBA, GL_UNSIGNED_BYTE, img_norm_ptr[i]);
	}

}

void Init_buffer() {
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(4, objects_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, objects_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * christmastree_m.NofV, christmastree_m.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * policecar_m.NofV, policecar_m.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * taxi_m.NofV, taxi_m.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * truck_m.NofV, truck_m.vertexptr, GL_STATIC_DRAW);

	glGenBuffers(4, objects_texcoord_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, objects_texcoord_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * christmastree_m.NofV, christmastree_m.texcoordptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_texcoord_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * policecar_m.NofV, policecar_m.texcoordptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_texcoord_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * taxi_m.NofV, taxi_m.texcoordptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_texcoord_buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * truck_m.NofV, truck_m.texcoordptr, GL_STATIC_DRAW);

	glGenBuffers(4, objects_normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, objects_normal_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * christmastree_m.NofV, christmastree_m.normalptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_normal_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * policecar_m.NofV, policecar_m.normalptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_normal_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * taxi_m.NofV, taxi_m.normalptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_normal_buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * truck_m.NofV, truck_m.normalptr, GL_STATIC_DRAW);

	glGenBuffers(4, objects_tangent_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, objects_tangent_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * christmastree_m.NofV, christmastree_m.tangentptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_tangent_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * policecar_m.NofV, policecar_m.tangentptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_tangent_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * taxi_m.NofV, taxi_m.tangentptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, objects_tangent_buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * truck_m.NofV, truck_m.tangentptr, GL_STATIC_DRAW);

	glGenBuffers(3, map_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, map_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * map.map_model.NofV, map.map_model.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, map_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * map.map_model.NofV, map.map_model.colorptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, map_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * map.map_model.NofV, map.map_model.normalptr, GL_STATIC_DRAW);

	glGenBuffers(3, player_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, player_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * player_m.NofV, player_m.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, player_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * player_m.NofV, player_m.colorptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, player_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * player_m.NofV, player_m.normalptr, GL_STATIC_DRAW);

	glGenBuffers(3, arm_right_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, arm_right_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * arm_right_m.NofV, arm_right_m.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, arm_right_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * arm_right_m.NofV, arm_right_m.colorptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, arm_right_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * arm_right_m.NofV, arm_right_m.normalptr, GL_STATIC_DRAW);

	glGenBuffers(3, arm_left_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, arm_left_buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * arm_left_m.NofV, arm_left_m.vertexptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, arm_left_buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * arm_left_m.NofV, arm_left_m.colorptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, arm_left_buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * arm_left_m.NofV, arm_left_m.normalptr, GL_STATIC_DRAW);
}

void Init_light() {
	light_soruce1.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	light_soruce1.diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	light_soruce1.specular = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	light_soruce1.positon = glm::vec4(0.0f,5.0f, - (float) map.height * 1.6f / 2.0f, 1.0f);

	object_optical[0].ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	object_optical[0].diffuse = glm::vec4(1.0f, 1.0f, 0.1f, 1.0f);
	object_optical[0].specular = glm::vec4(1.0f, 1.0f, 0.1f, 1.0f);
	object_optical[0].shine = 2.0f;

	object_optical[1].ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	object_optical[1].diffuse = glm::vec4(0.1f, 1.0f, 1.0f, 1.0f);
	object_optical[1].specular = glm::vec4(0.1f, 1.0f, 1.0f, 1.0f);
	object_optical[1].shine = 5.0f;

	object_optical[2].ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	object_optical[2].diffuse = glm::vec4(1.0f, 1.0f, 0.1f, 1.0f);
	object_optical[2].specular = glm::vec4(1.0f, 1.0f, 0.1f, 1.0f);
	object_optical[2].shine = 5.0f;

	object_optical[3].ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	object_optical[3].diffuse = glm::vec4(1.0f, 0.8f, 1.0f, 1.0f);
	object_optical[3].specular = glm::vec4(1.0f, 0.8f, 1.0f, 1.0f);
	object_optical[3].shine = 5.0f;

	player_optical.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	player_optical.diffuse = glm::vec4(1.0f, 1.0f, 0.1f, 1.0f);
	player_optical.specular = glm::vec4(1.0f, 1.0f, 0.1f, 1.0f);
	player_optical.shine = 1.0f;

	map_optical.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	map_optical.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	map_optical.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	map_optical.shine = 1.0f;

}

void Init()
{
	arm_right_m = CreatePlayerArmVertex(1);
	arm_left_m = CreatePlayerArmVertex(-1);
	player_m = CreatePlayerVertex();
	cout << "sec1" << endl;
	CreateMap(&map);
	cout << "sec2" << endl;
	Init_modeling_obj();
	cout << "sec3" << endl;
	Init_Imgfile();
	cout << "sec4" << endl;
	Init_texture();
	cout << "sec5" << endl;
	Init_buffer();
	cout << "sec6" << endl;
	animationlist = Create_Animation_obj(&map);
	cout << "sec7" << endl;
	Init_light();
	cout << "sec8" << endl;
	perspective = 2;
	Initialize_Shader(&shaderprogram_tex, "vertex_shader_tex.glsl", "fragment_shader_tex.glsl");
	Initialize_Shader(&shaderprogram_ic, "vertex_shader_ic.glsl", "fragment_shader_ic.glsl");
	
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Destruct(void) {
	delete(animationlist);
	DestructMap(&map);
	delete(policecar_m.vertexptr);
	delete(christmastree_m.vertexptr);
	delete(taxi_m.vertexptr);
	delete(truck_m.vertexptr);

	delete(policecar_m.texcoordptr);
	delete(christmastree_m.texcoordptr);
	delete(taxi_m.texcoordptr);
	delete(truck_m.texcoordptr);

	delete(policecar_m.normalptr);
	delete(christmastree_m.normalptr);
	delete(taxi_m.normalptr);
	delete(truck_m.normalptr);

	delete(player_m.vertexptr);
	delete(player_m.colorptr);
	delete(arm_right_m.vertexptr);
	delete(arm_right_m.colorptr);
	delete(arm_left_m.vertexptr);
	delete(arm_left_m.colorptr);

	for (int i = 0; i < 4; i++)
		delete(img_ptr[i]);
	ilDeleteImages(4, obj_img_name);

}
void display_player_object(Animation_obj aniobj) {

	if (aniobj.Son != nullptr) {
		for (int i = 0; i < 10; i++) {
			if (aniobj.Son[i].activation == TRUE)
				display_player_object(aniobj.Son[i]);
		}
	}

	glBindVertexArray(vao);
	
	glPolygonMode(GL_FRONT, GL_FILL);
	GLuint shaderprogram;
	shaderprogram = shaderprogram_ic;
	glUseProgram(shaderprogram);

	GLint obj_vertice_loc = glGetAttribLocation(shaderprogram, "obj_vertices");
	GLint color_loc = glGetAttribLocation(shaderprogram, "icolor");
	GLint normal_loc = glGetAttribLocation(shaderprogram, "obj_normal");

	GLint MVP_loc = glGetUniformLocation(shaderprogram, "MVP");
	GLint Projection_loc = glGetUniformLocation(shaderprogram, "Projection");
	GLint AniMatrix_loc = glGetUniformLocation(shaderprogram, "AniMatrix");
	GLint AmbientProduct_loc = glGetUniformLocation(shaderprogram, "AmbientProduct");
	GLint DiffuseProduct_loc = glGetUniformLocation(shaderprogram, "DiffuseProduct");
	GLint SpecularProduct_loc = glGetUniformLocation(shaderprogram, "SpecularProduct");
	GLint LightPosition_loc = glGetUniformLocation(shaderprogram, "LightPosition");
	GLint Shininess_loc = glGetUniformLocation(shaderprogram, "Shininess");

	glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(Projection_loc, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(AniMatrix_loc, 1, GL_FALSE, glm::value_ptr(aniobj.Animation_matrix()));
	glUniform4fv(AmbientProduct_loc, 1, glm::value_ptr(multiply_vector(light_soruce1.ambient, player_optical.ambient)));  
	glUniform4fv(DiffuseProduct_loc, 1, glm::value_ptr(multiply_vector(light_soruce1.diffuse, player_optical.diffuse))); 
	glUniform4fv(SpecularProduct_loc, 1, glm::value_ptr(multiply_vector(light_soruce1.specular, player_optical.specular)));  
	glUniform4fv(LightPosition_loc, 1, glm::value_ptr(light_soruce1.positon));
	glUniform1f(Shininess_loc, player_optical.shine);
	
	glBindBuffer(GL_ARRAY_BUFFER, FindBuffer(aniobj.name, vertex_s));
	glEnableVertexAttribArray(obj_vertice_loc);
	glVertexAttribPointer(obj_vertice_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, FindBuffer(aniobj.name, color_s));
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, FindBuffer(aniobj.name, normal_s));
	glEnableVertexAttribArray(normal_loc);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));

	glDrawArrays(GL_TRIANGLES, 0, aniobj.model->NofV);

	glDisableVertexAttribArray(obj_vertice_loc);
	glDisableVertexAttribArray(color_loc);
	glDisableVertexAttribArray(normal_loc);
	
}
void display_object(Animation_obj aniobj) {
	glBindVertexArray(vao);
	GLuint shaderprogram;
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shaderprogram = shaderprogram_tex;
	glUseProgram(shaderprogram);

	GLint obj_vertice_loc = glGetAttribLocation(shaderprogram, "obj_vertices");
	GLint texcoord_loc = glGetAttribLocation(shaderprogram, "vtex_coord");
	GLint obj_normal_loc = glGetAttribLocation(shaderprogram, "obj_normal");
	GLint tangent_loc = glGetAttribLocation(shaderprogram, "vtangent");

	GLint MVP_loc = glGetUniformLocation(shaderprogram, "MVP");
	GLint Projection_loc = glGetUniformLocation(shaderprogram, "Projection");
	GLint AniMatrix_loc = glGetUniformLocation(shaderprogram, "AniMatrix");
	GLint AmbientProduct_loc = glGetUniformLocation(shaderprogram, "AmbientProduct");
	GLint DiffuseProduct_loc = glGetUniformLocation(shaderprogram, "DiffuseProduct");
	GLint SpecularProduct_loc = glGetUniformLocation(shaderprogram, "SpecularProduct");
	GLint LightPosition_loc = glGetUniformLocation(shaderprogram, "LightPosition");
	GLint Shininess_loc = glGetUniformLocation(shaderprogram, "Shininess");
	GLint texture_loc = glGetUniformLocation(shaderprogram, "texture");
	GLint normtexture_loc = glGetUniformLocation(shaderprogram, "normtexture");
	

	glBindBuffer(GL_ARRAY_BUFFER, objects_buffer[aniobj.obj_index]);
	glEnableVertexAttribArray(obj_vertice_loc);
	glVertexAttribPointer(obj_vertice_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, objects_texcoord_buffer[aniobj.obj_index]);
	glEnableVertexAttribArray(texcoord_loc);
	glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, objects_normal_buffer[aniobj.obj_index]);
	glEnableVertexAttribArray(obj_normal_loc);
	glVertexAttribPointer(obj_normal_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, objects_tangent_buffer[aniobj.obj_index]);
	glEnableVertexAttribArray(tangent_loc);
	glVertexAttribPointer(tangent_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	
	//	light light_soruce1;
//	optical_prop object_optical[4];

	glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(Projection_loc, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(AniMatrix_loc, 1, GL_FALSE, glm::value_ptr(aniobj.Animation_matrix()));
	glUniform4fv(AmbientProduct_loc, 1, glm::value_ptr( multiply_vector(light_soruce1.ambient, object_optical[aniobj.obj_index].ambient) ) );//aniobj.obj_index  
	glUniform4fv(DiffuseProduct_loc, 1, glm::value_ptr( multiply_vector(light_soruce1.diffuse, object_optical[aniobj.obj_index].diffuse) ) );//aniobj.obj_index  
	glUniform4fv(SpecularProduct_loc, 1, glm::value_ptr( multiply_vector(light_soruce1.specular, object_optical[aniobj.obj_index].specular) ) );//aniobj.obj_index  
	glUniform4fv(LightPosition_loc, 1, glm::value_ptr(light_soruce1.positon));
	glUniform1f(Shininess_loc, object_optical[aniobj.obj_index].shine);
	glUniform1i(texture_loc, 0);
	glUniform1i(normtexture_loc, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objects_texture[aniobj.obj_index]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, objects_norm_texture[aniobj.obj_index]);

	glDrawArrays(GL_TRIANGLES, 0, aniobj.model->NofV);

	glDisableVertexAttribArray(obj_vertice_loc);
	glDisableVertexAttribArray(texcoord_loc);
	glDisableVertexAttribArray(obj_normal_loc);
}

void display_map(Map_prop mapprop) {
	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GLuint shaderprogram = shaderprogram_ic;
	glUseProgram(shaderprogram);

	GLint obj_vertice_loc = glGetAttribLocation(shaderprogram, "obj_vertices");
	GLint color_loc = glGetAttribLocation(shaderprogram, "icolor");
	GLint normal_loc = glGetAttribLocation(shaderprogram, "obj_normal");

	GLint AniMatrix_loc = glGetUniformLocation(shaderprogram, "AniMatrix");
	GLint MVP_loc = glGetUniformLocation(shaderprogram, "MVP");
	GLint Projection_loc = glGetUniformLocation(shaderprogram, "Projection");
	GLint AmbientProduct_loc = glGetUniformLocation(shaderprogram, "AmbientProduct");
	GLint DiffuseProduct_loc = glGetUniformLocation(shaderprogram, "DiffuseProduct");
	GLint SpecularProduct_loc = glGetUniformLocation(shaderprogram, "SpecularProduct");
	GLint LightPosition_loc = glGetUniformLocation(shaderprogram, "LightPosition");
	GLint Shininess_loc = glGetUniformLocation(shaderprogram, "Shininess");

	glm::mat4 m = glm::mat4(1.0f);
	glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(Projection_loc, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(AniMatrix_loc, 1, GL_FALSE, glm::value_ptr(m));
	glUniform4fv(AmbientProduct_loc, 1, glm::value_ptr(multiply_vector(light_soruce1.ambient, map_optical.ambient)));
	glUniform4fv(DiffuseProduct_loc, 1, glm::value_ptr(multiply_vector(light_soruce1.diffuse, map_optical.diffuse)));
	glUniform4fv(SpecularProduct_loc, 1, glm::value_ptr(multiply_vector(light_soruce1.specular, map_optical.specular)));
	glUniform4fv(LightPosition_loc, 1, glm::value_ptr(light_soruce1.positon));
	glUniform1f(Shininess_loc, map_optical.shine);

	glBindBuffer(GL_ARRAY_BUFFER, map_buffer[0]);
	glEnableVertexAttribArray(obj_vertice_loc);
	glVertexAttribPointer(obj_vertice_loc, 3, GL_FLOAT, GL_TRUE, 0, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, map_buffer[1]);
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_TRUE, 0, (GLvoid*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, map_buffer[2]);
	glEnableVertexAttribArray(normal_loc);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_TRUE, 0, (GLvoid*)(0));

	glDrawArrays(GL_TRIANGLES, 0,mapprop.map_model.NofV);
	glDisableVertexAttribArray(obj_vertice_loc);
	glDisableVertexAttribArray(color_loc);
	glDisableVertexAttribArray(normal_loc);
}

void display(void) {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	MVP=Set_CTM(perspective);
	Projection = Set_Projection(perspective);

	display_map(map);
	display_player_object(animationlist[0]);
	for (int i = 1; i < size_of_animationlist; i++) {
		display_object(animationlist[i]);
	}

	glutSwapBuffers();
	glDisable(GL_DEPTH_TEST);
}

void timer(int){
	//movement of cars.
	glm::vec4 test_light = MVP * glm::vec4(light_soruce1.positon);
	for (int i = 1; i < size_of_animationlist; i++) {
		if (animationlist[i].name.compare(christmastree_s)) { //This means that if it is not a christmas tree
			animationlist[i].position = animationlist[i].position + animationlist[i].vel;
			//collision test
			if (MapCollisionx(map.map_model.boundary,animationlist[i].position, animationlist[i].model->boundary,0)) 
				animationlist[i].position.x = map.map_model.boundary[1] - animationlist[i].model->boundary[1] -0.1f;
			if (MapCollisionx(map.map_model.boundary, animationlist[i].position, animationlist[i].model->boundary,1)) 
				animationlist[i].position.x = map.map_model.boundary[0] - animationlist[i].model->boundary[0] + 0.1f;
		}
	}

	//movement of player
	animationlist[0].position = animationlist[0].position + animationlist[0].vel;
	Animation1(animationlist[0].Son, 0, 1, player_move);
	//collision test
	if (MapCollisionx(map.map_model.boundary, animationlist[0].position, animationlist[0].model->boundary, 0))
		animationlist[0].position.x = map.map_model.boundary[0] + 0.1f;
	if (MapCollisionx(map.map_model.boundary, animationlist[0].position, animationlist[0].model->boundary, 1))
		animationlist[0].position.x = map.map_model.boundary[1] - 0.1f;
	if (MapCollisionx(map.map_model.boundary, animationlist[0].position, animationlist[0].model->boundary, 5))
		animationlist[0].position.z = map.map_model.boundary[5] - 0.1f;
	for (int i = 1; i < size_of_animationlist; i++) {
		if (animationlist[i].name.compare(christmastree_s)) { //This means that if it is not a christmas tree
			if (Collision(animationlist[0].position, player_m.boundary, animationlist[i].position, animationlist[i].boundary)) {
				cout << "!!WASTED!!" << endl;
				exit(0);
			}
		}
		else {
			if (Collision(animationlist[0].position, player_m.boundary, animationlist[i].position, animationlist[i].model->boundary)) {
				animationlist[0].position = Reposition(animationlist[0].position, player_m.boundary, animationlist[i].position, animationlist[i].model->boundary);
			}
		}
	}

	if (animationlist[0].position.z < map.map_model.boundary[4]+0.5) {
		wingame = 1;
		glutDestroyWindow(winid);
		cout << "you won the game" << endl;
		cout << "press enter to quit the game" << endl;
	}

	//
	glutTimerFunc(33, timer, 0);
	glutPostRedisplay();
}


void keyboardCallBack(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		glutDestroyWindow(winid);
		cout << "user quit program" << endl;
		break;
	case '1': break;
	case '2': break;
	case '3': break;
	}
}
void keyboardUpCallBack(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': exit(0); break;
	case '1': perspective = 1; break;
	case '2': perspective = 2; break;
	case '3': perspective = 3; break;
	}
}
void SpecialKeyCallBack(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		player_move = TRUE;
		if (!AngleCheckDegree(animationlist[0].theta_y, 0.0f, 0.1f)) {
			animationlist[0].theta_y = 0.0f;
		}
		animationlist[0].vel = glm::vec3(0.0f, 0.0f, -0.2f);
		break;
	case GLUT_KEY_LEFT:
		player_move = TRUE;
		if (!AngleCheckDegree(animationlist[0].theta_y, 90.0f, 0.1f)) {
			animationlist[0].theta_y = 90.0f;
		}
		animationlist[0].vel = glm::vec3(- 0.2f, 0.0f, 0.0f);
		break;
	case GLUT_KEY_RIGHT:
		player_move = TRUE;
		if (!AngleCheckDegree(animationlist[0].theta_y, - 90.0f, 0.1f)) {
			animationlist[0].theta_y = - 90.0f;
		}
		animationlist[0].vel = glm::vec3(0.2f, 0.0f, 0.0f);
		break;
	case GLUT_KEY_DOWN:
		player_move = TRUE;
		if (!AngleCheckDegree(animationlist[0].theta_y, 180.0f, 0.1f)) {
			animationlist[0].theta_y = 180.0f;
		}
		animationlist[0].vel = glm::vec3(0.0f, 0.0f, 0.2f);
	case 13:
		if (wingame == 1) {
			exit(0);
		}
	}
}
void SpecialKeyUpCallBack(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		player_move = FALSE;
		animationlist[0].vel = glm::vec3(animationlist[0].vel.x, animationlist[0].vel.y, 0.0f); break;
	case GLUT_KEY_LEFT:
		player_move = FALSE;
		animationlist[0].vel = glm::vec3(0.0f, animationlist[0].vel.y, animationlist[0].vel.z); break;
	case GLUT_KEY_RIGHT:
		player_move = FALSE; 
		animationlist[0].vel = glm::vec3(0.0f, animationlist[0].vel.y, animationlist[0].vel.z); break;
	case GLUT_KEY_DOWN:
		player_move = FALSE; 
		animationlist[0].vel = glm::vec3(animationlist[0].vel.x, animationlist[0].vel.y, 0.0f); break;
	}
}

void InitGlutCallbacks() {
	glutTimerFunc(33, timer, 0);
	glutDisplayFunc(display);
	
	glutKeyboardFunc(keyboardCallBack);
	glutKeyboardUpFunc(keyboardUpCallBack);

	glutSpecialFunc(SpecialKeyCallBack);
	glutSpecialUpFunc(SpecialKeyUpCallBack);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(500, 100);
	glutInitContextVersion(3,2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	winid = glutCreateWindow("test");
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		cerr << "Error: " << glewGetErrorString(res) << endl;
		return 1;
	}

	Init();

	InitGlutCallbacks();
	glutMainLoop();

	Destruct();
	return 0;
}


