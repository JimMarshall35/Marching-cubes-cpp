// Vertex shader:
// ================
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 FragWorldPos;
out vec3 Normal;
out vec3 LightPos;
out vec3 WorldSpaceNormal;

uniform vec3 lightPos; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
	FragWorldPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
	WorldSpaceNormal = vec3(model * vec4(aNormal, 0.0));

    LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
}