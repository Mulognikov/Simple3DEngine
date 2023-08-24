#pragma once
#include "Vec3f.h"
#include "Vec2i.h"

struct Vec3i
{
	Vec3i(int x = 0, int y = 0, int z = 0): x(x), y(y), z(z) {}
	int x, y, z;


	inline Vec3i operator + (const Vec3i& vec) const
	{
		return {x + vec.x, y + vec.y, z + vec.z};
	}

	inline Vec3i operator - (const Vec3i& vec) const
	{
		return {x - vec.x, y - vec.y, z - vec.z};
	}

	inline Vec3i operator * (const int s) const
	{
		return {x * s, y * s, z * s};
	}

	inline operator Vec3f() const
	{
		return {(float)x, (float)y, (float)z};
	}

	inline operator Vec2i() const
	{
		return {x, y};
	}
};




