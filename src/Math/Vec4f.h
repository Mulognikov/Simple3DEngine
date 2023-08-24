#pragma once
#include "MathFwd.h"

struct Vec4f
{
	Vec4f(float x = 0, float y = 0, float z = 0, float w = 1) : x(x), y(y), z(z), w(w) {}
	float x, y, z, w;

	inline operator Vec3f() const;

	inline Vec4f operator + (const Vec4f& v1) const
	{
		return {x + v1.x, y + v1.y, z + v1.z, w + v1.w};
	}

	inline Vec4f operator - (const Vec4f& v1) const
	{
		return {x - v1.x, y - v1.y, z - v1.z, w - v1.w};
	}

	inline Vec4f operator * (const int i) const
	{
		return {x * i, y * i, z * i, w * i};
	}

	inline Vec4f operator * (const float f) const
	{
		return {x * f, y * f, z * f, w * f};
	}
};
