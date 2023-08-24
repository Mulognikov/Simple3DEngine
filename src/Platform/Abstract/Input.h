#pragma once

class Input
{
public:
	virtual void initialize() = 0;
	virtual bool keyPressed(int keyCode) = 0;
};
