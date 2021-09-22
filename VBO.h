#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glew.h>
#include<vector>
#include"ComplexModel.h"

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(const GLfloat* vertices, GLsizeiptr size);
	VBO();
	void giveID();

	void fill(const GLfloat* vertices, GLsizeiptr size);
	void fill(const std::vector<float> vertices);
	void fill(const std::vector<Vertex> vertices);
	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif