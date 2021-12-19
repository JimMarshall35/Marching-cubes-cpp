#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <iostream>
#include <iomanip>
#include <cstdlib>


#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <chrono>
#include <iostream>

#include "gui.h"
#include "MetaBallApplication.h"



#include <time.h>       /* time */
#include "Terrain.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void fps(f64 deltatime);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void imguiInit(GLFWwindow* window);


static Camera camera;
static bool wantMouseInput = false;
static bool wantKeyboardInput = false;
static void printGLSLExtensions();

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
			camera.ProcessMouseMovement(dx, dy);
		}
	}

}

void imguiInit(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

static void printGLSLExtensions()
{
	GLint n = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);

	PFNGLGETSTRINGIPROC glGetStringi = 0;
	glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");

	for (GLint i = 0; i < n; i++)
	{
		const char* extension =
			(const char*)glGetStringi(GL_EXTENSIONS, i);
		printf("Ext %d: %s\n", i, extension);
	}

}


static void processInput(GLFWwindow* window, float delta)
{
	if (!wantKeyboardInput) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.ProcessKeyboard(FORWARD, delta);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboard(BACKWARD, delta);
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.ProcessKeyboard(LEFT, delta);
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboard(RIGHT, delta);
		}
		
	}



}
static void inputInit(GLFWwindow* window) {
	glfwSetCursorPosCallback(window, cursor_position_callback);
}



void initTimers() {
	MetaBallApplication::RegisterTimer("march: ", duration(1000));
	MetaBallApplication::RegisterTimer("update vertices: ", 50);
}





int main(int argc, char* argv[])
{
	//printGLSLExtensions();
	//printNumVertsTable();
	//printFlatGLSLTrisTable();
	std::cout << "max work group size: " << GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS << std::endl;
	srand(time(NULL));
	MetaBallApplication::paused = true;
	initTimers();
	MetaBallApplication::InitMarcher();
	MetaBallApplication::SpawnRandomMovingMetaballs();
	auto t1 = std::chrono::high_resolution_clock::now();
	MetaBallApplication::March();
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	std::cout << ms_double.count() << " ms" << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	Renderer::window_w = 800;
	Renderer::window_h = 600;
	GLFWwindow* window = glfwCreateWindow(Renderer::window_w, Renderer::window_h, "Marching cubes Demo", NULL, NULL);
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
	glfwMaximizeWindow(window);

	Renderer::InitShader("vert.glsl", "frag.glsl");
	

	
	camera.Position = glm::vec3(-6.47688, 11.3699, 23.4017);
	camera.Yaw = -66.1;
	camera.Pitch = -24.4;
	camera.updateCameraVectors();
	inputInit(window);
	f64 last = glfwGetTime();
	VAO_VBO_Pair p = MetaBallApplication::LoadVerticesIntial();

	GLClearErrors();
	glEnable(GL_DEPTH_TEST);
	GLPrintErrors("glEnable GL_DEPTH_TEST");
	glEnable(GL_TEXTURE_3D);
	GLPrintErrors("glEnable GL_TEXTURE_3D");

	glClearColor(0,0,0,0);

	//ComputeShaderMarcher m;
	//m.Evaluate(MetaBalls::_MetaBalls, glm::vec3(0), 0.4);
	//m.GenerateMesh(MetaBalls::_MetaBalls, glm::vec3(0,-0.5f,0), glm::vec3(1.0,1.0,1.0), glm::ivec3(32, 32, 32), 0.0f);

	Terrain t;
	t.Init();
	//WireFrameCubeGL wireframe;
	//wireframe.SetDimsInitial(1.0,1.0,1.0, vec3(0, -0.5f, 0));

	imguiInit(window);
	const ImGuiIO& io = ImGui::GetIO(); (void)io;
	//printGLSLExtensions();
	while (!glfwWindowShouldClose(window))
	{
		f64 now = glfwGetTime();
		f64 delta = now - last;
		last = now;
		wantMouseInput = io.WantCaptureMouse;
        wantKeyboardInput = io.WantCaptureKeyboard;

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		

		processInput(window, delta);

		//ImGui::ShowDemoWindow();
		doUI(p.VBO);
		/*
		MetaBallApplication::MoveMetaBalls(delta);

		MetaBallApplication::UpdateMarcherIso();
		if (!MetaBallApplication::paused) {
			MetaBallApplication::StartTimer("march: ");
			MetaBallApplication::March();
			MetaBallApplication::StopTimer("march: ");
			MetaBallApplication::StartTimer("update vertices: ");
			MetaBallApplication::UpdateVertices(p.VBO);
			MetaBallApplication::StopTimer("update vertices: ");
		}
		*/
		

		ImGui::Render();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//m.GenerateMesh(MetaBalls::_MetaBalls, glm::vec3(0, -0.5f, 0), glm::vec3(1,1,1), glm::ivec3(32,32,32), 0.0f);
		//Renderer::render(camera, p.VAO, MetaBallApplication::GetNumVertices());
		t.Render(camera, Renderer::window_w, Renderer::window_h);
		//wireframe.Render(camera, Renderer::window_w, Renderer::window_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	
		
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int newwidth, int newheight)
{
	Renderer::window_w = newwidth;
	Renderer::window_h = newheight;
	glViewport(0, 0, newwidth, newheight);
	
}

void fps(f64 deltatime)
{
	static double accumulated = 0.0;
	static int times_called = 0;
}



