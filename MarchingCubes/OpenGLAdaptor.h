#pragma once
#include "Marcher.h"
typedef unsigned int GLuint;

struct VAO_VBO_Pair{
	GLuint VAO;
	GLuint VBO;
};
// get the vertices from a CubeMarcher into opengl vertex buffer object after you've called CubeMarcher march()
VAO_VBO_Pair loadVerticesInitial(const CubeMarcher& marcher); // returns VAO and VBO
void updateVertices(GLuint vbo, const CubeMarcher& m);
