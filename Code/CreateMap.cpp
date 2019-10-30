#include "CreateMap.h"
#include "random.h"

void CreateMap(Map_prop* map_prop) {
	//create map size and procure memory to save datas.
	const int sign[2] = { -1,1 };
	int width = Random(5, 10);
	int height = Random(10, 20);
	int * map = new int[height]; map[0] = 0; map[height-1] = 0;
	int * fieldindex = new int[height]; fieldindex[0] = 0; //fieldindex[i]=map index of i'th field .
	
	// create map propertiy randomly and save datas.
	int det = 2; int index = 1; int f = 1; // 
	while (det != height) {
		int tmp = Random(2, 4); //create random number of sequential roads. The maximum sequential number of roads is 4.
		if (tmp >= height - det) { //code for the case which of the number of sequential roads is bigger than the number of undetermined map property.
			while (0 < height - det) {
				map[index] = Random(sign,2); index++; det++; //sign determine the direction car will pass.
			}
			break;
		}
		else {
			for (int j = 0; j < tmp; j++) {
				map[index] = Random(sign,2); index++; det++;
			}
		}
		map[index] = 0; fieldindex[f] = index;	//make 1 safe field every after sequential roads. ...  
		index++; f++; det++;					//This code has possibility to assign 0 to last index of map[] but it doesn't matter because the value of last index was originally 0;
	}
	f++; //first and final index of map is set to field. I intended this final f value becoms the number of field. first field is counted in starting f as 1. Final field is counted now.
	fieldindex[f - 1]=height-1;

	int * treenumarr = new int[f]; treenumarr[0] = 0; treenumarr[f - 1] = 0;
	int ** tree = new int*[f]; 
	for (int i = 0; i < f; i++) {
		tree[i] = new int[width];
	}

	// tree is set in field. tree matrix has same size of field. element 0 means no tree. element 1 means there is tree. Before determine tree position, first set no tree.
	for (int i = 0; i < f; i++) {
		for (int j = 0; j < width; j++) {
			tree[i][j] = 0;
		}
	}

	//starting point and goal has no tree. construct tree in filed this part.
	int tree_tot_number=0;
	for (int i = 1; i < f - 1; i++)
	{
		int NofTree = Random(0, width - 4);
		treenumarr[i] = NofTree;
		tree_tot_number = tree_tot_number + NofTree;
		ChooseRand(tree[i], width, NofTree);
	}
	map_prop->height = height;
	map_prop->width = width;
	map_prop->map = map;
	map_prop->tree = tree;
	map_prop->fieldindex = fieldindex;
	map_prop->NofField = f;
	map_prop->NofTree = tree_tot_number;
	map_prop->tree_number_arr = treenumarr;
	Print_map_prop(*map_prop);
	map_prop->map_model=Create_map_buffer(map_prop);
}

void DestructMap(Map_prop* map_prop) {
	delete(map_prop->map);
	for (int i=0; i < map_prop->NofField; i++) {
		delete(map_prop->tree[i]);
	}
	delete(map_prop->tree);
	delete(map_prop->fieldindex);
	delete(map_prop->map_model.vertexptr);
	delete(map_prop->map_model.colorptr);
}

void Print_map_prop(Map_prop maprop) {
	// print out map property.
	int height = maprop.height;
	int width = maprop.width;
	int f = maprop.NofField;
	int* map = maprop.map;
	int* fieldindex = maprop.fieldindex;
	int ** tree = maprop.tree;

	cout << endl;
	cout << "H=" << height << " W=" << width << " f=" << f << endl;
	cout << "map : " << endl;
	for (int i = 0; i < height; i++) {
		cout << map[i] << " ";
	}
	cout << endl;
	cout << "fieldindex : " << endl;
	for (int i = 0; i < f; i++) {
		cout << fieldindex[i] << " ";
	}
	cout << endl;
	cout << "tree : " << endl;
	for (int i = 0; i < f; i++) {
		for (int j = 0; j < width; j++) {
			cout << tree[i][j] << " ";
		}
		cout << "" << endl;
	}
	cout << endl;
}

Model_prop Create_map_buffer(Map_prop* map_prop) {
	Model_prop out;
	int W = map_prop->width;
	int H = map_prop->height;
	int F = map_prop->NofField;

	int NofTri = 0;
	for (int i = 0; i < H; i++) {
		if (map_prop->map[i] == 0) {
			NofTri= NofTri + 2;
		}
		else {
			NofTri = NofTri + 2;
			if (map_prop->map[i-1] != 0) {
				for (int j = 0; j < W; j++) {
					NofTri = NofTri + 2;
				}
			}
		}
	}

//	NofTri = F * 2 + (H - F) * 2+ (H - F)*W*2; //field square, road square, centerline square 
	float sizeup = 1.6f;
	int NofVertices = NofTri * 3;
	int NofElements = NofVertices * 3;
	float * vertexbuffer = new float[NofElements];
	float * colorbuffer = new float[NofElements];
	float * normalbuffer = new float[NofElements];
	float d[3] = { sizeup * (float)W / 2.0f ,0.0f ,sizeup * 0.5f };
	float d_dot[3] = { sizeup * 0.2f,0.0f,0.1f };
	int index = 0;
	for (int i = 0; i < H; i++) {
		float x_pos = 0.0f;
		float y_pos = 0.1f;
		float z_pos = - sizeup * (float)i;

		if (map_prop->map[i] == 0) {
			//1st triangle
			vertexbuffer[index] = x_pos - d[0]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 1.0f; index++;
			vertexbuffer[index] = z_pos - d[2]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = x_pos - d[0]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 1.0f; index++;
			vertexbuffer[index] = z_pos + d[2]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = x_pos + d[0]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 1.0f; index++;
			vertexbuffer[index] = z_pos - d[2]; colorbuffer[index] = 0.0f; index++;
			//2nd triangle
			vertexbuffer[index] = x_pos - d[0]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 1.0f; index++;
			vertexbuffer[index] = z_pos + d[2]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = x_pos + d[0]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 1.0f; index++;
			vertexbuffer[index] = z_pos - d[2]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = x_pos + d[0]; colorbuffer[index] = 0.0f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 1.0f; index++;
			vertexbuffer[index] = z_pos + d[2]; colorbuffer[index] = 0.0f; index++;
		}
		else {
			//1st triangle
			vertexbuffer[index] = x_pos - d[0]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = z_pos - d[2]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = x_pos - d[0]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = z_pos + d[2]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = x_pos + d[0]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = z_pos - d[2]; colorbuffer[index] = 0.5f; index++;
			//2nd triangle
			vertexbuffer[index] = x_pos - d[0]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = z_pos + d[2]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = x_pos + d[0]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = z_pos - d[2]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = x_pos + d[0]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = y_pos + d[1]; colorbuffer[index] = 0.5f; index++;
			vertexbuffer[index] = z_pos + d[2]; colorbuffer[index] = 0.5f; index++;

			if (map_prop->map[i-1] != 0) {
				for (int j = 0; j < W; j++) {
					float x_pos = 0.0f - d[0] + d_dot[0] + sizeup * (float)j;
					float y_pos = 0.11f;
					float z_pos = sizeup * (-(float)i + 0.5f);
					//Center line 1st triangle
					vertexbuffer[index] = x_pos - d_dot[0]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = y_pos + d_dot[1]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = z_pos - d_dot[2]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = x_pos - d_dot[0]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = y_pos + d_dot[1]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = z_pos + d_dot[2]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = x_pos + d_dot[0]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = y_pos + d_dot[1]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = z_pos - d_dot[2]; colorbuffer[index] = 1.0f; index++;
					//Center line 2nd triangle
					vertexbuffer[index] = x_pos - d_dot[0]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = y_pos + d_dot[1]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = z_pos + d_dot[2]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = x_pos + d_dot[0]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = y_pos + d_dot[1]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = z_pos - d_dot[2]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = x_pos + d_dot[0]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = y_pos + d_dot[1]; colorbuffer[index] = 1.0f; index++;
					vertexbuffer[index] = z_pos + d_dot[2]; colorbuffer[index] = 1.0f; index++;
				}
			}
		}
	}
	for (int i = 0; i < NofVertices; i++) {
		normalbuffer[3 * i + 0] = 0.0f;
		normalbuffer[3 * i + 1] = 1.0f;
		normalbuffer[3 * i + 2] = 0.0f;

	}
//	cout << "in createmap, index is " << index << "sizeofarray is " << NofElements << endl;
	out.NofV = NofVertices;
	out.colorptr = colorbuffer;
	out.vertexptr = vertexbuffer;
	out.texcoordptr=nullptr;
	out.normalptr= normalbuffer;
	return out;
}