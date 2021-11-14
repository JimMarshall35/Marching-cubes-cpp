#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <iostream>
#include <iomanip>
#include <cstdlib>


#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "ErrorHandling.h"
#include <chrono>
#include <iostream>


#include "OpenGLAdaptor.h"
#include "MetaBalls.h"



void render(const Shader& shader, const Camera& camera, const GLuint VAO);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void fps(f64 deltatime);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

static int width, height;
static Camera* camera;
static bool wantMouseInput = false;
static bool wantKeyboardInput = false;
static GLuint num_vertices;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (!wantMouseInput) {
		static double lastx = 0;
		static double lasty = 0;

		double dx = xpos - lastx;
		double dy = ypos - lasty;

		lastx = xpos;
		lasty = ypos;
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS) {
			camera->ProcessMouseMovement(dx, dy);
		}

		//std::cout << dx << " " << dy << std::endl;
	}

}


static void processInput(GLFWwindow* window, float delta)
{
	if (!wantKeyboardInput) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera->ProcessKeyboard(FORWARD, delta);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->ProcessKeyboard(BACKWARD, delta);

		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->ProcessKeyboard(LEFT, delta);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->ProcessKeyboard(RIGHT, delta);
		}
		
	}



}
static void inputInit(GLFWwindow* window) {
	glfwSetCursorPosCallback(window, cursor_position_callback);
}

struct MovingMetaball {
	GUID id;
	vec3 direction;
	f32 speed;
};
bool RandomBool() {
	return rand() % 2 == 1;
}
static std::vector<MovingMetaball> movingMetaballs;
static void SpawnRandomMovingMetaballs(u32 number_to_spawn) {
	for (u32 i = 0; i < number_to_spawn; i++) {
		MovingMetaball m;
		f32 randx = -2 + ((f32)rand() / (f32)RAND_MAX) * 4.75;
		f32 randy = -2 + ((f32)rand() / (f32)RAND_MAX) * 4.75;
		f32 randz = -2 + ((f32)rand() / (f32)RAND_MAX) * 4.75;
		f64 randradius = 0.1 +((f32)rand() / (f32)RAND_MAX) * 0.5f;
		PHASE phase = RandomBool() ? PHASE::POSITIVE : PHASE::NEGATIVE;
		
		m.id = MetaBalls::addMetaball(randradius, vec3(randx,randy,randz), PHASE::POSITIVE);
		m.direction = vec3(
			-1.0f + ((f32)rand() / (f32)RAND_MAX) * 2.0f, 
			-1.0f + ((f32)rand() / (f32)RAND_MAX) * 2.0f, 
			-1.0f + ((f32)rand() / (f32)RAND_MAX) * 2.0f
		).normalize();

		m.speed = ((f32)rand() / (f32)RAND_MAX) * 0.5f;
		movingMetaballs.push_back(m);
	}
}
static void MoveMetaBalls(f32 delta) {
	for (MovingMetaball m : movingMetaballs) 
		MetaBalls::getPointer(m.id)->position += m.direction * m.speed * delta;
}
#include <time.h>       /* time */

int main(int argc, char* argv[])
{
	srand(time(NULL));
	CubeMarcher m;
	m.SetCubeDims(0.2, 0.2, 0.2);
	m.SetGridDims(55, 55, 55);
	f64 varing = 0.5;
	m.SetIsoLevel(0.3f);
	m.SetStartPoint(-2.75, -2.75, -2.75);

	//MetaBalls::addMetaball(1.0, vec3(0.0));
	//MetaBalls::addMetaball(1.0, vec3(1.5,1.5,1.5));
	SpawnRandomMovingMetaballs(10);

	auto t1 = std::chrono::high_resolution_clock::now();
	m.March(MetaBalls::getValueAtPoint);
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	std::cout << ms_double.count() << " ms" << std::endl;
	num_vertices = m.Vertices.size();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	width = 800;
	height = 600;
	GLFWwindow* window = glfwCreateWindow(width,height, "Marching cubes Demo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shader("vert.glsl", "frag.glsl");
	camera = new Camera();
	camera->Position = glm::vec3(0.0,0.0,13.5);
	inputInit(window);
	f64 last = glfwGetTime();
	VAO_VBO_Pair p = loadVerticesInitial(m);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,0);

	while (!glfwWindowShouldClose(window))
	{
		f64 now = glfwGetTime();
		f64 delta = now - last;
		last = now;

		processInput(window, delta);

		varing += delta;
		
		MoveMetaBalls(delta);
		//m.March([varing](vec3 pos) {
		//	return sin(pos.x + varing) + cos(pos.y +varing ) + sin(pos.z + varing);
		//});
		
		m.March(MetaBalls::getValueAtPoint);
		updateVertices(p.VBO, m);
		num_vertices = m.GetNumVerts();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		render(shader,*camera,p.VAO);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete camera;
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int newwidth, int newheight)
{
	width = newwidth;
	height = newheight;
	glViewport(0, 0, width, height);
	
}

void fps(f64 deltatime)
{
	static double accumulated = 0.0;
	static int times_called = 0;
}

void render(const Shader& shader, const Camera& camera, const GLuint VAO) {
	shader.use();
	glBindVertexArray(VAO);
	shader.setMat4("model", glm::identity<glm::mat4>());
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f));

	GLClearErrors();
	shader.setVec3("lightPos", glm::vec3(0, 0, 0));
	GLPrintErrors("lightPos");
	shader.setVec3("lightColor", glm::vec3(0.7,0.7,0.7));
	GLPrintErrors("lightColor");
	shader.setVec3("objectColor", glm::vec3(0.0, 1.0, 1.0));
	GLPrintErrors("objectColor");
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	GLPrintErrors("141");
}

