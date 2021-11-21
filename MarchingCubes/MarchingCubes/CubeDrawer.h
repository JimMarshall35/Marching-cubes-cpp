#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "typedefs.h"
#include "vectors.h"
#include "Camera.h"
class WireFrameCubeGL {
public:
	WireFrameCubeGL();
	void SetDimsInitial(f64 w, f64 h, f64 d, vec3 startpos);
	void Render(const Camera& camera, u32 window_w, u32 window_h);
private:
	static Shader s_Shader;
	GLuint _VAO = 0;
	GLuint _VBO = 0;
	static std::string s_VertexCode;
	static std::string s_FragmentCode;
private:
	void getWireFrameCubeVAO(f64 w, f64 h, f64 d, vec3 startpos);
	void InitialiseShader();
};