#pragma once
#include <windows.h>

#include <string>
#include <fstream>
#include <streambuf>
#include <crtdbg.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

// read docs: https://github.com/g-truc/glm/blob/master/manual.md#section1
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <gl/GL.h>
#include <SOIL2.h>


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

// macro that returns "char*" with offset "i"
// BUFFER_OFFSET(5) transforms in "(char*)nullptr+(5)"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

using namespace std;

void CrateTexture(char* myImage, GLuint* myTexture)
{
	int image_width = 0;
	int image_height = 0;
	unsigned char* image = SOIL_load_image(myImage, &image_width, &image_height, NULL, SOIL_LOAD_RGBA);
	glGenTextures(1, myTexture);
	glBindTexture(GL_TEXTURE_2D, *myTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *myTexture);
	SOIL_free_image_data(image);
}

void CreateShaders(string vertexShaderName, string geometryShaderName, string fragmantSaderName, GLuint *shaderProgram)
{
	// local buffer to store error strings when compiling.
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	//create vertex shader "name" and store it in "vs"
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	// open .glsl file and put it in a string
	ifstream shaderFile(vertexShaderName);
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	// glShaderSource requires a double pointer.
	// get the pointer to the c style string stored in the string object.
	const char* shaderTextPtr = shaderText.c_str();

	// ask GL to use this string a shader code source
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	// try to compile this shader source.
	glCompileShader(vs);

	// check for compilation error
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// repeat process for Fragment Shader (or Pixel Shader)
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open(fragmantSaderName);
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	if (geometryShaderName != "")
	{
		shaderFile.open(geometryShaderName);
		shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
		shaderFile.close();
		shaderTextPtr = shaderText.c_str();
		glShaderSource(gs, 1, &shaderTextPtr, nullptr);
		glCompileShader(gs);

		compileResult = GL_FALSE;
		glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE) {
			// query information about the compilation (nothing if compilation went fine!)
			memset(buff, 0, 1024);
			glGetShaderInfoLog(gs, 1024, nullptr, buff);
			// print to Visual Studio debug console output
			OutputDebugStringA(buff);
		}
	}
	//link shader program (connect vs and ps)
	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, fs);
	glAttachShader(*shaderProgram, vs);
	if (geometryShaderName != "")
		glAttachShader(*shaderProgram, gs);
	glLinkProgram(*shaderProgram);

	// check once more, if the Vertex Shader and the Fragment Shader can be used
	// together
	compileResult = GL_FALSE;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(*shaderProgram, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(*shaderProgram, vs);
	glDetachShader(*shaderProgram, fs);
	if (geometryShaderName != "")
		glDetachShader(*shaderProgram, gs);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);
}

