#pragma once

#include "Abstract/Screen.h"
#include "Abstract/SystemTime.h"
#include "Abstract/Application.h"
#include "Abstract/Input.h"


class Platform
{
public:
	static void initialize();

	static inline Screen *screen = nullptr;
	static inline SystemTime *systemTime = nullptr;
	static inline Application *application = nullptr;
	static inline Input *input = nullptr;

private:
	Platform() = default;
};
