#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class ComputeShader
{
public:
	ComputeShader(){}
	ComputeShader(const char* path);
	GLuint GetId() { return ID; }
	void Load(const char* path);
	void Use() const;
	void SetFloat(const char* name, float value) const;
	void SetInt(const char* name, int value) const;
	void SetVec3(const char* name, const glm::vec3& value) const;
	void SetIvec3(const char* name, const glm::ivec3& value) const;

private:
	GLuint ID;
};

