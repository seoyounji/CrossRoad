#version 330

uniform sampler2D texture;
uniform sampler2D normtexture;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

in vec2 ftex_coord;
in vec3 fN;
in vec3 fV;
in vec3 fL;
in vec3 ftangent;

out vec4 color;

void main()
{
	vec3 N = normalize(fN);
	vec3 T = normalize(ftangent);
	vec3 B = cross(T,N);
	mat3 TBN = mat3(T,B,N);

	vec3 Normal_in_normalmap = (texture2D(normtexture,ftex_coord)).xyz;
	Normal_in_normalmap = Normal_in_normalmap * 2.0f - 1.0f;
	vec3 new_normal = normalize(TBN * Normal_in_normalmap);

		 N = new_normal;
	vec3 V = normalize(fV);
	vec3 L = normalize(fL);
	vec3 H = normalize(L+V);

	float kd = max(dot(L,N),0.0f);
	float ks = pow(max(dot(N,H),0.0f),Shininess);

	vec4 ambient = AmbientProduct;
	vec4 diffuse = kd * DiffuseProduct;
	vec4 specular = ks * SpecularProduct;

	if(dot(L,N) < 0.0f)
		specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	vec4 lightcolor = ambient + diffuse + specular;

	color = texture2D(texture,ftex_coord) * lightcolor; //*lightcolor;
//	color = texture2D(texture,vec2(0.01f,0.01f));
}
