#pragma once

class SystemTime
{
public:
	virtual void initialize() = 0;
	virtual uint32_t getTimeFromStart() = 0;
};


