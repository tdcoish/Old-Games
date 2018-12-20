#include "Shader.h"
#include <GL/glew.h>

void SetShader(const GLchar* pVertexPath, const GLchar* pFragmentPath, Shader* pShader)
{
	//1. Retrieve the vertex/fragment source code from filepath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//ensures ifstream objects can throw exceptions
	vShaderFile.exceptions (std::ifstream::badbit);
	fShaderFile.exceptions (std::ifstream::badbit);
	try
	{
		//Open Files
		vShaderFile.open(pVertexPath);
		fShaderFile.open(pFragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handlers
		vShaderFile.close();
		fShaderFile.close();
		//convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure e){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	//now we convert std::strings that contain our shaders into GL format
	const GLchar* pVShaderCode = vertexCode.c_str();
	const GLchar* pFShaderCode = fragmentCode.c_str();
	//Compile shaders
	GLuint vertexID, fragmentID;
	GLint successFlg;
	GLchar shaderInfoLog[512];

	//Vertex Shader
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &pVShaderCode, NULL);
	glCompileShader(vertexID);
	//Print compile errors if any
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &successFlg);
	if(!successFlg){
		glGetShaderInfoLog(vertexID, 512, NULL, shaderInfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << shaderInfoLog << std::endl;
	}
	//Fragment Shader
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &pFShaderCode, NULL);
	glCompileShader(fragmentID);
	//error checking
	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &successFlg);
	if(!successFlg){
		glGetShaderInfoLog(fragmentID, 512, NULL, shaderInfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << shaderInfoLog << std::endl;
	}

	//shader program
	pShader->Program = glCreateProgram();		//call to .program member of Shader class. Should be programID
	glAttachShader(pShader->Program, vertexID);
	glAttachShader(pShader->Program, fragmentID);
	glLinkProgram(pShader->Program);
	//Print linking errors if any
	glGetProgramiv(pShader->Program, GL_LINK_STATUS, &successFlg);
	if(!successFlg){
		glGetProgramInfoLog(pShader->Program, 512, NULL, shaderInfoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << shaderInfoLog << std::endl;
	}
	//Delete the shaders since they're linked into our program and no longer necessary
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

void UseShader(Shader shad){
	glUseProgram(shad.Program);
}