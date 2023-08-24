#include "TimeWindows.h"

void TimeWindows::initialize()
{
	startTime = std::chrono::high_resolution_clock::now();
}

uint32_t TimeWindows::getTimeFromStart()
{
	auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
	uint32_t microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	return microseconds;
}
