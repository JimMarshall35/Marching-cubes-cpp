#include "CubeDrawer.h"

Shader WireFrameCubeGL::s_Shader;
std::string WireFrameCubeGL::s_VertexCode =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;"

"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"

"void main()"
"{"
"gl_Position = projection * view * model * vec4(aPos, 1.0);"
"}";

std::string WireFrameCubeGL::s_FragmentCode =
"#version 330 core\n"

"uniform vec3 objectColor;"
"out vec4 FragColor;"

"void main()"
"{"
"FragColor = vec4(objectColor, 1.0);"
"}";


WireFrameCubeGL::WireFrameCubeGL()
{
	InitialiseShader();
}

void WireFrameCubeGL::SetDimsInitial(f64 w, f64 h, f64 d, vec3 startpos)
{
	getWireFrameCubeVAO(w, h, d, startpos);
}

void WireFrameCubeGL::Render(const Camera& camera, u32 window_w, u32 window_h)
{
	s_Shader.use();
	glBindVertexArray(_VAO);
	s_Shader.setMat4("model", glm::identity<glm::mat4>());
	s_Shader.setMat4("view", camera.GetViewMatrix());
	s_Shader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float)window_w / (float)window_h, 0.1f, 100.0f));
	s_Shader.setVec3("objectColor", glm::vec3(1.0, 1.0, 1.0));

	glDrawArrays(GL_LINES, 0, 24);
}

void WireFrameCubeGL::getWireFrameCubeVAO(f64 w, f64 h, f64 d, vec3 startpos)
{
	f32 x = startpos.x;
	f32 y = startpos.y;
	f32 z = startpos.z;

	vec3 bbl = vec3(x, y, z);
	vec3 bbr = vec3(x+w, y, z);
	vec3 bfl = vec3(x, y, z+d);
	vec3 bfr = vec3(x+w, y, z+d);

	vec3 tbl = vec3(x, y+h, z);
	vec3 tbr = vec3(x+w, y+h, z);
	vec3 tfl = vec3(x, y+h, z+d);
	vec3 tfr = vec3(x+w, y+h, z+d);

	vec3 vertices[] = {

		bbl, tbl,
		bbr, tbr,
		bfl, tfl,
		bfr, tfr,

		bbl, bbr,
		bbr, bfr,
		bfr, bfl,
		bfl, bbl,

		tbl, tbr,
		tbr, tfr,
		tfr, tfl,
		tfl, tbl,
	};

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec3), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void WireFrameCubeGL::InitialiseShader()
{
	if (!s_Shader.ID) {
		s_Shader.LoadFromString(s_VertexCode, s_FragmentCode);
	}
}
