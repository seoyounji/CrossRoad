#version 330


uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

in vec4 FragColor;
in vec3 fN;
in vec3 fV;
in vec3 fL;

out vec4 color;

void main()
{
	vec3 N = normalize(fN);
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

    color = FragColor * lightcolor; //*lightcolor;
	//color = vec4(1.0, 1.0, 1.0, 1.0);
}
