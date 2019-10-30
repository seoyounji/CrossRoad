#include "Collision.h"

bool Collision(glm::vec3 position1, float boundary1[6], glm::vec3 position2, float boundary2[6]) {
	bool colxyz[3];
	float obj1[6];
	float obj2[6];

	obj1[0] = boundary1[0] + position1.x;
	obj1[1] = boundary1[1] + position1.x;
	obj1[2] = boundary1[2] + position1.y;
	obj1[3] = boundary1[3] + position1.y;
	obj1[4] = boundary1[4] + position1.z;
	obj1[5] = boundary1[5] + position1.z;

	obj2[0] = boundary2[0] + position2.x;
	obj2[1] = boundary2[1] + position2.x;
	obj2[2] = boundary2[2] + position2.y;
	obj2[3] = boundary2[3] + position2.y;
	obj2[4] = boundary2[4] + position2.z;
	obj2[5] = boundary2[5] + position2.z;

	for (int i = 0; i < 3; i++) {
		colxyz[i] = (obj1[2 * i + 0] < obj2[2 * i + 1] && obj1[2 * i + 1] > obj2[2 * i + 0]);
	}

	return colxyz[0] && colxyz[1] && colxyz[2];
}

bool MapCollisionx(float mapboundary[6], glm::vec3 position, float boundary[6],int dir) {
	//consider collision of x,y,z coordinates but return only x effect.
	//dir=0 : left x boundary collision
	//dir=1 : right x boundary collision
	//dir=2 : small y boundary collision
	//dir=3 : big y boundary collision
	//dir=4 : small z boundary collision
	//dir=5 : big z boundary collision
	float obj[6];
	
	obj[0] = boundary[0] + position.x;
	obj[1] = boundary[1] + position.x;
	obj[2] = boundary[2] + position.y;
	obj[3] = boundary[3] + position.y;
	obj[4] = boundary[4] + position.z;
	obj[5] = boundary[5] + position.z;
	/*
	bool colxyz[3];
	colxyz[0] = obj[0]<mapboundary[0] || obj[1] > mapboundary[1];
	colxyz[1] = obj[2]<mapboundary[2] || obj[3] > mapboundary[3];
	colxyz[2] = obj[4]<mapboundary[4] || obj[5] > mapboundary[5];
	*/
	switch (dir) {
	case 0: return obj[0] < mapboundary[0]; break;
	case 1: return obj[1] > mapboundary[1]; break;
	case 2: return obj[2] < mapboundary[2]; break;
	case 3: return obj[3] > mapboundary[3]; break;
	case 4: return obj[4] < mapboundary[4]; break;
	case 5: return obj[5] > mapboundary[5]; break;
	}

}

bool MapCollisionz(float mapboundary[6], glm::vec3 position, float boundary[6], int dir) {
	//consider collision of x,y,z coordinates but return only x effect.
	//dir=-1 : left boundary collision
	//dir=1  : right boundary collision

	float obj[6];
	obj[4] = boundary[4] + position.z;
	obj[5] = boundary[5] + position.z;

	if(dir == -1)
		return obj[4] < mapboundary[4];
	else
		return obj[5] > mapboundary[5];
}

glm::vec3 Reposition(glm::vec3 activeP, float activeboundary[6], glm::vec3 staticP, float staticboundary[6] ) { //axis 0:x, 1:y, 2:z
	//
	float Px;
	float Py = activeP.y;
	float Pz;

	if (staticP.x > activeP.x) {
		float boundvals = staticboundary[0];
		float boundvala = activeboundary[1];
		Px = staticP.x + boundvals - boundvala;
	}
	else {
		float boundvals = staticboundary[1];
		float boundvala = activeboundary[0];
		Px = staticP.x + boundvals - boundvala;
	}
	if (staticP.z > activeP.z) {
		float boundvals = staticboundary[4];
		float boundvala = activeboundary[5];
		Pz = staticP.z + boundvals - boundvala;
	}
	else {
		float boundvals = staticboundary[5];
		float boundvala = activeboundary[4];
		Pz = staticP.z + boundvals - boundvala;
	}

	return glm::vec3(Px, Py, Pz);
}