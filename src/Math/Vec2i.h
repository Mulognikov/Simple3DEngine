#pragma once
#include "Vec3f.h"

struct Vec2i
{
	Vec2i(int x = 0, int y = 0): x(x), y(y) {}
	int x, y;


	Vec2i operator + (const Vec2i& vec) const
	{
		return {x + vec.x, y + vec.y};

	}
	Vec2i operator - (const Vec2i& vec) const
	{
		return {x - vec.x, y - vec.y};
	}

	Vec2i operator * (const int s) const
	{
		return {x * s, y * s};
	}

	operator Vec3f() const
	{
		return {(float)x, (float)y, 0};
	}
};



