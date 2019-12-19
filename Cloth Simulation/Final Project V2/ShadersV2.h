/*****************************************
 *
 *           ShadersV2.cpp
 *
 *	Custom Shader class which uses Mark Green Code to
 * compile shader code with ease
 *  Utility routines to assist with creating
 *  sharders.
 *
 ****************************************/

#pragma once

#ifndef SHADERS_V2_H
#define SHADERS_V2_H

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class Shaders {
	unsigned int ID;
public:
	Shaders(std::string vertexFilePath, std::string fragmentFilePath) {
		std::string vsCode;
		std::string fsCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		//Sets the error
		//http://www.cplusplus.com/reference/ios/ios/exceptions/
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vShaderFile.open(vertexFilePath);
			fShaderFile.open(fragmentFilePath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vsCode = vShaderStream.str();
			fsCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			fprintf(stderr, "Shader files could not be read in!");
		}

		const char* vShaderCode = vsCode.c_str();
		const char* fShaderCode = fsCode.c_str();

		int result;
		unsigned int vertex, fragment;
		char* buffer;

		//Compile Vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, 0);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			std::cout << "vertex shader compile error: " << vertexFilePath << std::endl;
			//std::printf("shader compile error: %s\n", vertexFilePath);
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &result);
			buffer = new char[result];
			glGetShaderInfoLog(vertex, result, 0, buffer);
			printf("%s\n", buffer);
			delete buffer;
		}

		//Compile Fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, 0);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			std::cout << "fragment shader compile error: " << vertexFilePath << std::endl;
			//std::printf("shader compile error: %s\n", fragmentFilePath);
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &result);
			buffer = new char[result];
			glGetShaderInfoLog(fragment, result, 0, buffer);
			printf("%s\n", buffer);
			delete buffer;
		}

		//Create the shader program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &result);
		if (result != GL_TRUE) {
			printf("program link error\n");
			glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &result);
			buffer = new char[result];
			glGetProgramInfoLog(ID, result, 0, buffer);
			printf("%s\n", buffer);
			delete buffer;
		}

	}
	void dumpProgram(char* description) {
		char name[256];
		GLsizei length;
		GLint size;
		GLenum type;
		int uniforms;
		int attributes;
		int shaders;
		int i;

		printf("Information for shader: %s\n", description);

		if (!glIsProgram(ID)) {
			printf("not a valid shader program\n");
			return;
		}

		glGetProgramiv(ID, GL_ATTACHED_SHADERS, &shaders);
		printf("Number of shaders: %d\n", shaders);

		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniforms);
		printf("uniforms: %d\n", uniforms);
		for (i = 0; i < uniforms; i++) {
			glGetActiveUniform(ID, i, 256, &length, &size, &type, name);
			printf("  name: %s\n", name);
		}
		glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTES, &attributes);
		printf("attributes: %d\n", attributes);
		for (i = 0; i < attributes; i++) {
			glGetActiveAttrib(ID, i, 256, &length, &size, &type, name);
			printf("  name: %s\n", name);
		}
		std::cout << std::endl;
	}
	int getShaderID() {
		return this->ID;
	}

	void useShader() {
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void setVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec4(const std::string& name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

};
#endif // !SHADERS_V2_H

