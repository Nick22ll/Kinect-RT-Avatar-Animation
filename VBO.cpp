#include"VBO.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(const GLfloat* vertices, GLsizeiptr size)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VBO::VBO() {
	
}

void VBO::giveID() {
	glGenBuffers(1, &ID);
}


//size = byte dimension of the buffer objects
void VBO::fill(const GLfloat* vertices, GLsizeiptr size) {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::fill(const std::vector<float> vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
}

void VBO::fill(const std::vector<Vertex> vertices) {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

// Binds the VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}