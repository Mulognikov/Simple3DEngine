#include "Platform.h"
#include "Windows/ScreenOpenGL.h"
#include "Windows/TimeWindows.h"
#include "Windows/ApplicationWindows.h"
#include "Windows/InputGLFW.h"


void Platform::initialize()
{
	screen = new ScreenOpenGL;
	systemTime = new TimeWindows;
	application = new ApplicationWindows;
	input = new InputGLFW;
}
