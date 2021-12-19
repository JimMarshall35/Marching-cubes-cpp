#include "ComputeShader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ErrorHandling.h"

ComputeShader::ComputeShader(const char* path)
{
    Load(path);
}

void checkCompileErrors(GLuint shader, std::string type)
{
    GLClearErrors();
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        GLPrintErrors("glGetShaderiv");
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            GLPrintErrors("glGetShaderInfoLog");
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        GLPrintErrors("glGetProgramiv");
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            GLPrintErrors("glGetProgramInfoLog");
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void ComputeShader::Load(const char* path)
{
    GLClearErrors();
	std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::cout << path << std::endl;
	try {
		shaderFile.open(path);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* shaderCode_c = shaderCode.c_str();
	unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
    GLPrintErrors("glCreateShader");
	glShaderSource(compute, 1, &shaderCode_c, NULL);
    GLPrintErrors("glShaderSource");
	glCompileShader(compute);
    GLPrintErrors("glCompileShader");
    checkCompileErrors(compute, "COMPUTE");
    ID = glCreateProgram();
    GLPrintErrors("glCreateProgram");
    glAttachShader(ID, compute);
    GLPrintErrors("glAttachShader");
    glLinkProgram(ID);
    GLPrintErrors("glLinkProgram");
    checkCompileErrors(ID, "PROGRAM");
    glDeleteShader(compute);
    GLPrintErrors("glDeleteShader");
}

void ComputeShader::Use() const
{
    GLClearErrors();
    glUseProgram(ID);
    GLPrintErrors("Use");
}

void ComputeShader::SetFloat(const char* name, float value) const
{
    GLClearErrors();
    glUniform1f(glGetUniformLocation(ID, name), value);
    GLPrintErrors(name);
}

void ComputeShader::SetInt(const char* name, int value) const
{
    GLClearErrors();
    glUniform1i(glGetUniformLocation(ID, name), value);
    GLPrintErrors(name);
}

void ComputeShader::SetVec3(const char* name, const glm::vec3& value) const
{
    GLClearErrors();
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
    GLPrintErrors(name);
}

void ComputeShader::SetIvec3(const char* name, const glm::ivec3& value) const
{
    GLClearErrors();
    glUniform3iv(glGetUniformLocation(ID, name), 1, &value[0]);
    GLPrintErrors(name);
}
