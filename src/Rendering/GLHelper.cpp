// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>

#include "Rendering/GLHelper.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

GLuint GLHelper::buildProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	Log& log = App::getLog();

	log.logInfo("Building vertex and fragment shaders (%s, %s)", vertexShaderPath, fragmentShaderPath);

	std::ifstream vertexShaderFile(vertexShaderPath);
	std::ifstream fragmentShaderFile(fragmentShaderPath);

	if (!vertexShaderFile.is_open())
		throw std::runtime_error("Could not open vertex shader file");

	if (!fragmentShaderFile.is_open())
		throw std::runtime_error("Could not open fragment shader file");

	std::stringstream vertexShaderSs;
	vertexShaderSs << vertexShaderFile.rdbuf();
	std::string vertexShaderString = vertexShaderSs.str();
	const GLchar* vertexShaderStringPtr = vertexShaderString.c_str();

	std::stringstream fragmentShaderSs;
	fragmentShaderSs << fragmentShaderFile.rdbuf();
	std::string fragmentShaderString = fragmentShaderSs.str();
	const GLchar* fragmentShaderStringPtr = fragmentShaderString.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderStringPtr, 0);
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(vertexShader);

		log.logError("Vertex shader compile error:\n\n%s", &infoLog[0]);
		throw std::runtime_error("Could not compile vertex shader");
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderStringPtr, 0);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

		log.logError("Fragment shader compile error:\n\n%s", &infoLog[0]);
		throw std::runtime_error("Could not compile fragment shader");
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);

	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		log.logError("Shader program link error:\n\n%s", &infoLog[0]);
		throw std::runtime_error("Could not link shader program");
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
