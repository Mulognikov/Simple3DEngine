#include <iostream>
#include "ScreenOpenGL.h"
#include "../Platform.h"

int32_t ScreenOpenGL::initialize()
{
	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "glfwInit failed!" << std::endl;
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(SCREEN_W * RESOLUTION_MULTIPLIER, SCREEN_H * RESOLUTION_MULTIPLIER, "OpenGL Render test", nullptr, nullptr);
	if (!pWindow)
	{
		std::cout << "glfwCreateWindow failed!" << std::endl;
		glfwTerminate();
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(pWindow);

	if (!gladLoadGL())
	{
		std::cout << "Can't load GLAD!" << std::endl;
	}

	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	glClearColor(0.5f, 0.5f, 0, 1);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	GLfloat point[] = {
			1.0f, 1.0f,
			1.0f, -1.0f,
			-1.0f, -1.0f,
			-1.0f, 1.0f,
	};

	GLuint indices[] = {
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
	};

	GLfloat color[] = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f,
	};

	GLfloat uv[] = {
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,


	};

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

	GLuint colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	GLuint uv_vbo = 0;
	glGenBuffers(1, &uv_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

	ebo = 0;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	frameBufferRGB24 = new unsigned char[SCREEN_W * SCREEN_H * 3];
	return 0;
}

void ScreenOpenGL::finalize()
{
	glfwTerminate();
}

void ScreenOpenGL::sendToScreen(uint8_t *frameBuffer)
{
	for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
	{
		unsigned char RGB = frameBuffer[i];

		unsigned char b = RGB & 0b00000011;
		RGB = RGB >> 2;
		unsigned char g = RGB & 0b00000111;
		RGB = RGB >> 3;
		unsigned char r = RGB & 0b00000111;

		int position = i * 3;

		if (Platform::input->keyPressed(90))
		{
			frameBufferRGB24[position] = frameBuffer[i];
			frameBufferRGB24[position + 1] = frameBuffer[i];
			frameBufferRGB24[position + 2] = frameBuffer[i];

			continue;
		}
		frameBufferRGB24[position] = r * 36;
		frameBufferRGB24[position + 1] = g * 36;
		frameBufferRGB24[position + 2] = b * 85;
	}


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, frameBufferRGB24);
	glGenerateMipmap(GL_TEXTURE_2D);

	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader_program);

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	/* Swap front and back buffers */
	glfwSwapBuffers(pWindow);

	/* Poll for and process events */
	glfwPollEvents();
}

bool ScreenOpenGL::shouldClose()
{
	return glfwWindowShouldClose(pWindow);
}
