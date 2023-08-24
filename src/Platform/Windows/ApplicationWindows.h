#pragma once
#include "../Platform.h"
#include "ScreenOpenGL.h"


class ApplicationWindows : public Application
{
public:
	inline bool shouldExit() override
	{
		return dynamic_cast<ScreenOpenGL*>(Platform::screen)->shouldClose();
	}
};
