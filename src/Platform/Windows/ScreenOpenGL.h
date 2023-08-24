#pragma once

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>
#include "../Abstract/Screen.h"

#define RESOLUTION_MULTIPLIER 6

class ScreenOpenGL : public Screen
{
public:
	int32_t initialize() override;
	void finalize() override;
	void sendToScreen(uint8_t *frameBuffer) override;
	bool shouldClose();
	GLFWwindow* pWindow;

private:
	GLuint shader_program;
	GLuint textureID = 0;
	GLuint vao = 0;
	GLuint ebo = 0;

	unsigned char* frameBufferRGB24;

	const char* vertex_shader =
			"#version 460\n"
			"layout(location = 0) in vec2 vertex_position;"
			"layout(location = 1) in vec3 vertex_color;"
			"layout(location = 2) in vec2 texture_position;"
			"out vec3 color;"
			"out vec2 texPos;"
			"void main() {"
			"   color = vertex_color;"
			"   gl_Position = vec4(vertex_position, 0.0, 1.0);"
			"	texPos = vec2(texture_position.x, texture_position.y);"
			"}";

	const char* fragment_shader =
			"#version 460\n"
			"in vec3 color;"
			"in vec2 texPos;"
			"out vec4 frag_color;"
			"uniform sampler2D texture1;"
			"void main() {"
			"	frag_color = texture(texture1, texPos);"
			"}";
};