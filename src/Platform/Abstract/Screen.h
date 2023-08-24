#pragma once

#include <iostream>

#define SCREEN_W 320
#define SCREEN_H 240

class Screen
{
public:
	virtual int32_t initialize() = 0;
	virtual void finalize() = 0;
	virtual void sendToScreen(uint8_t *frameBuffer) = 0;
};
