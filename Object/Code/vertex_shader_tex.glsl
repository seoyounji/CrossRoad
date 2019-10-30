#version 330

uniform mat4 AniMatrix;
uniform mat4 MVP;
uniform mat4 Projection;
uniform vec4 LightPosition;

in vec3 obj_vertices;
in vec3 obj_normal;
in vec2 vtex_coord;
in vec3 vtangent;

out vec2 ftex_coord;
out vec3 fN;
out vec3 fV;
out vec3 fL;
out vec3 ftangent;

void main()
{
	vec4 normal = MVP * AniMatrix * vec4(obj_normal,0.0f);
	vec4 tangent = MVP * AniMatrix * vec4(vtangent,0.0f);
	vec4 vertices = MVP * AniMatrix * vec4(obj_vertices,1.0f);
	vec4 light_position = MVP * LightPosition;

	fN = normal.xyz;
	fV = -vertices.xyz;
	fL = (light_position).xyz;
	if(light_position.w != 0.0f){
		fL=(light_position - vertices).xyz;
	}

    gl_Position=Projection * vertices;
	ftex_coord=vtex_coord;
	ftangent = tangent.xyz;
	
}