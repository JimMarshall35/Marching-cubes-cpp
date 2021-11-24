#include "Renderer.h"
unsigned int Renderer::window_h;
unsigned int Renderer::window_w;
Shader Renderer::_Shader;
glm::vec3 Renderer::object_colour(0,1,0);
glm::vec3 Renderer::light_colour(1,1,1);
glm::vec3 Renderer::light_pos(0);
float Renderer::specular_strength = 0.2f;
float Renderer::ambient_strength = 0.2f;
float Renderer::shininess = 32.0f;

void Renderer::WireframeOn()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::WireframeOff()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::InitShader(const char* frag, const char* vert)
{
	_Shader = Shader(frag, vert);
}

void Renderer::render(const Camera& camera, const GLuint VAO, unsigned int numVertices)
{
	_Shader.use();
	glBindVertexArray(VAO);
	_Shader.setMat4("model", glm::identity<glm::mat4>());
	_Shader.setMat4("view", camera.GetViewMatrix());
	_Shader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float)window_w / (float)window_h, 0.1f, 100.0f));

	GLClearErrors();
	_Shader.setVec3("lightPos", light_pos);
	GLPrintErrors("lightPos");
	_Shader.setVec3("lightColor", light_colour);
	GLPrintErrors("lightColor");
	_Shader.setVec3("objectColor", object_colour);
	GLPrintErrors("objectColor");
	_Shader.setFloat("ambientStrength", ambient_strength);
	GLPrintErrors("ambientStrength");
	_Shader.setFloat("specularStrength", specular_strength);
	GLPrintErrors("specularStrength");
	_Shader.setFloat("shininess", shininess);
	GLPrintErrors("shininess");
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	GLPrintErrors("141");
}
