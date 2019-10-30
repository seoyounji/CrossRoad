#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "readfile.h"
using namespace std;

void Initialize_Shader(GLuint* ShaderProgram, const char* vshadername, const char* fshadername);