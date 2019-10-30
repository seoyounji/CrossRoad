#include "obj_loader.h"

cObj::cObj(std::string filename) {
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::string line, key;
	while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
		key = "";
		std::stringstream stringstream(line);
		stringstream >> key >> std::ws;

		if (key == "v") { // vertex
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			vertices.push_back(v);
		}
		else if (key == "vp") { // parameter
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			parameters.push_back(v);
		}
		else if (key == "vt") { // texture coordinate
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			texcoords.push_back(v);
		}
		else if (key == "vn") { // normal
			vertex v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.v.push_back(x);
			}
			v.normalize();
			normals.push_back(v);
		}
		else if (key == "f") { // face
			face f; int v, t, n;
			while (!stringstream.eof()) {
				stringstream >> v >> std::ws;
				f.vertex.push_back(v - 1);
				if (stringstream.peek() == '/') {
					stringstream.get();
					if (stringstream.peek() == '/') {
						stringstream.get();
						stringstream >> n >> std::ws;
						f.normal.push_back(n - 1);
					}
					else {
						stringstream >> t >> std::ws;
						f.texture.push_back(t - 1);
						if (stringstream.peek() == '/') {
							stringstream.get();
							stringstream >> n >> std::ws;
							f.normal.push_back(n - 1);
						}
					}
				}
			}
			faces.push_back(f);
		}
		else {

		}
	}
	ifs.close();
	std::cout << "               Name: " << filename << std::endl;
	std::cout << "           Vertices: " << vertices.size() << std::endl;
	std::cout << "         Parameters: " << parameters.size() << std::endl;
	std::cout << "Texture Coordinates: " << texcoords.size() << std::endl;
	std::cout << "            Normals: " << normals.size() << std::endl;
	std::cout << "              Faces: " << faces.size() << std::endl << std::endl;
	construct_data();
	Set_tangent();
	clearvector();
}

void cObj::construct_data() {
	NofV = 3 * faces.size();
	vertices_buffer_data = new float[NofV * 3];
	normal_data = new float[NofV * 3];
	texcoord_data = new float[2 * NofV];
	int index = 0;
	for (int i = 0; i < (int)faces.size(); i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++)
			{
				vertices_buffer_data[9 * i + 3 * j + k] = vertices[faces[i].vertex[j]].v[k];
				normal_data[9 * i + 3 * j + k] = normals[faces[i].normal[j]].v[k];
			}
			texcoord_data[6 * i + 2 * j + 0] = texcoords[faces[i].texture[j]].v[0];
			texcoord_data[6 * i + 2 * j + 1] = texcoords[faces[i].texture[j]].v[1];
		}
	}


}
void cObj::Set_tangent() {
	tangent_data = new float[NofV * 3];
	for (int i = 0; i < faces.size(); i++) {
		glm::vec3 v0 = glm::vec3(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
		glm::vec3 v1 = glm::vec3(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
		glm::vec3 v2 = glm::vec3(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);

		glm::vec2 uv0 = glm::vec2(texcoords[faces[i].texture[0]].v[0], texcoords[faces[i].texture[0]].v[1]);
		glm::vec2 uv1 = glm::vec2(texcoords[faces[i].texture[1]].v[0], texcoords[faces[i].texture[1]].v[1]);
		glm::vec2 uv2 = glm::vec2(texcoords[faces[i].texture[2]].v[0], texcoords[faces[i].texture[2]].v[1]);

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;


		float f = 1.0f / (deltaUV1.x - deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 t, b;

		t = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
		
		for (int j = 0; j < 3; j++) {
			tangent_data[9 * i + 3 * j + 0] = t.x;
			tangent_data[9 * i + 3 * j + 1] = t.y;
			tangent_data[9 * i + 3 * j + 2] = t.z;
		}
	}
}

void cObj::clearvector()
{
	vertices.clear();
	texcoords.clear();
	normals.clear();
	faces.clear();
}

int cObj::NumberofV(){
	return NofV;
}

cObj::~cObj() {
	
}

