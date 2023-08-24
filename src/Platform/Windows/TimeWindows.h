#pragma once
#include <chrono>
#include "../Abstract/SystemTime.h"

class TimeWindows : public SystemTime
{
public:
	void initialize() override;
	uint32_t getTimeFromStart() override;
private:
	std::chrono::time_point<std::chrono::steady_clock> startTime;
};


