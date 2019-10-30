#version 330

uniform mat4 AniMatrix;
uniform mat4  MVP;
uniform mat4 Projection;
uniform vec4 LightPosition;


in vec3 obj_vertices;
in vec3 obj_normal;
in vec3 icolor;

out vec4 FragColor;
out vec3 fN;
out vec3 fV;
out vec3 fL;


void main()
{
	vec4 normal = MVP * AniMatrix * vec4(obj_normal,0.0f);
	vec4 vertices = MVP * AniMatrix * vec4(obj_vertices,1.0f);
	vec4 light_position = MVP * LightPosition;

	fN = normal.xyz;
	fV = -vertices.xyz;
	fL = (light_position).xyz;
	if(light_position.w != 0.0f){
		fL=(light_position - vertices).xyz;
	}

    gl_Position=Projection * vertices;
    FragColor= vec4(icolor,1.0f);
}