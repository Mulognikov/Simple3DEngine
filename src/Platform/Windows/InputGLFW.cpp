#include "InputGLFW.h"


void InputGLFW::initialize()
{
	std::memset(keys, false, sizeof(bool) * 1024);
	glfwSetKeyCallback(dynamic_cast<ScreenOpenGL*>(Platform::screen)->pWindow, key_callback);
}

bool InputGLFW::keyPressed(int keyCode)
{
	return keys[keyCode];
}

void InputGLFW::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}

	if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
}
