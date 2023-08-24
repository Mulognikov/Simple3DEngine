#pragma once

#include "../Abstract/Input.h"
#include "../Platform.h"
#include "ScreenOpenGL.h"

class InputGLFW : public Input
{
public:
	void initialize() override;
	bool keyPressed(int keyCode) override;

private:
	static inline bool* keys = new bool[1024];
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
