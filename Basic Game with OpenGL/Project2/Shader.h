/******************************************************************
The Shader class creates a shader program.
1) Load in two text files that specify our vertex shader and fragment shader
2) Convert loaded strings into GLchar*'s
3) Compile Shaders
4) Link shaders
5) Perform error checking all along the way.

Additionally we have an easy wrapper for our shader. So we just call
shaderName.Use() 
in our rendering code.

This is copy-pasted from learnOpenGL.com. All credit goes to him.
******************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

struct Shader{
	GLuint Program;
};

void SetShader(const GLchar* pVertexPath, const GLchar* pFragmentPath, Shader* pShader);

void UseShader(Shader shader);

#endif