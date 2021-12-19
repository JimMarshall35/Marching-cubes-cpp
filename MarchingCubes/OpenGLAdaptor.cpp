#include "OpenGLAdaptor.h"
#include <glad/glad.h> 

VAO_VBO_Pair loadVerticesInitial(const CubeMarcher& marcher) {
	VAO_VBO_Pair p;
	glGenVertexArrays(1, &p.VAO);
	glGenBuffers(1, &p.VBO);
	glBindVertexArray(p.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, p.VBO);
	glBufferData(GL_ARRAY_BUFFER, marcher.GetGridSize() * sizeof(Vertex) * MAX_VERTS_PER_CUBE, &marcher.Vertices[0], GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, marcher.GetNumVerts() * sizeof(Vertex), &marcher.Vertices[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	
	return p;
}

void updateVertices(GLuint vbo, const CubeMarcher& m) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m.GetNumVerts() * sizeof(Vertex), &m.Vertices[0]);
}