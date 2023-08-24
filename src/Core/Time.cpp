#include "Time.h"
#include "../Platform/Platform.h"


void Time::startUpdateTimer()
{
	startTime = Platform::systemTime->getTimeFromStart();
}

void Time::stopUpdateTimer()
{
	stopTime = Platform::systemTime->getTimeFromStart();
	deltaTime = static_cast<float>(stopTime - startTime) * 0.000001f;
}
