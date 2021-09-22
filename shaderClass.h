#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glew.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);
	Shader();
	
	void link(const char* vertexFile, const char* fragmentFile);
	// Activates the Shader Program
	void Activate();
	void DeActivate();
	// Deletes the Shader Program
	void Delete();
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};
#endif