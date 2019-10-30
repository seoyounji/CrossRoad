#include "InitializeShader.h"


void Initialize_Shader(GLuint* ShaderProgram,const char* vshadername, const char* fshadername)
{
	GLint success;
	GLchar error_log[1024];

	*ShaderProgram = glCreateProgram();
	if (*ShaderProgram == 0) {
		cerr << "Error creating shader program" << endl;
		exit(1);
	}

	string vertex_shader_text = ReadFile(vshadername); //
	string frag_shader_text = ReadFile(fshadername); //

	GLuint vertex_shader_obj = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* p[1];
	GLint lengths[1];
	p[0] = vertex_shader_text.c_str();	lengths[0] = vertex_shader_text.size();
	glShaderSource(vertex_shader_obj, 1, p, lengths);
	glCompileShader(vertex_shader_obj);
	glGetShaderiv(vertex_shader_obj, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader_obj, sizeof(error_log), NULL, error_log);
		cerr << "Error compiling Vertex Shader: " << error_log << endl;
		exit(1);
	}

	GLuint frag_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
	p[0] = frag_shader_text.c_str();	lengths[0] = frag_shader_text.size();
	glShaderSource(frag_shader_obj, 1, p, lengths);
	glCompileShader(frag_shader_obj);
	glGetShaderiv(frag_shader_obj, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader_obj, 1024, NULL, error_log);
		cerr << "Error compiling Fragment Shader: " << error_log << endl;
		exit(1);
	}

	glAttachShader(*ShaderProgram, vertex_shader_obj);
	glAttachShader(*ShaderProgram, frag_shader_obj);

	glLinkProgram(*ShaderProgram);
	glGetProgramiv(*ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(*ShaderProgram, sizeof(error_log), NULL, error_log);
		cerr << "Error linking shader program: " << error_log << endl;
		//exit(1);
	}

	glValidateProgram(*ShaderProgram);
	glGetProgramiv(*ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(*ShaderProgram, sizeof(error_log), NULL, error_log);
		cerr << "Invalid shader program: " << error_log << endl;
		exit(1);
	}

}
