#pragma once
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "ErrorHandling.h"

static class Renderer
{
private:
	
public:
	static Shader _Shader;
	static void WireframeOn();
	static void WireframeOff();
	static void InitShader(const char* frag, const char* vert);
	static void render(const Camera& camera, const GLuint VAO, unsigned int numVertices);
	static unsigned int window_w;
	static unsigned int window_h;
	static glm::vec3 object_colour;
	static glm::vec3 light_colour;
	static glm::vec3 light_pos;
	static float ambient_strength;
	static float specular_strength;
	static float shininess;
};

