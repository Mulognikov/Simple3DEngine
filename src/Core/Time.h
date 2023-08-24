#pragma once

#include <iostream>


class Time
{
public:
	static void startUpdateTimer();
	static void stopUpdateTimer();
	static inline float deltaTime = 0.0f;

private:
	Time() = default;
	static inline uint32_t startTime = 0;
	static inline uint32_t stopTime = 0;
};
