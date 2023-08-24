#pragma once
#include <cmath>
#include "MathFwd.h"
#include "Vec4f.h"

struct Vec3f
{
	Vec3f(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
	float x, y, z;

	inline operator Vec4f() const;

	inline Vec3f operator + (const Vec3f& v1) const
	{
		return {x + v1.x, y + v1.y, z + v1.z};
	}

	inline Vec3f operator - (const Vec3f& v1) const
	{
		return {x - v1.x, y - v1.y, z - v1.z};
	}

	inline Vec3f operator * (const int i) const
	{
		return {x * i, y * i, z * i};
	}

	inline Vec3f operator * (const float f) const
	{
		return {x * f, y * f, z * f};
	}

	inline Vec3f operator / (const float f) const
	{
		return {x / f, y / f, z / f};
	}

	static float dotProduct(Vec3f &v1, Vec3f &v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	static Vec3f crossProduct(Vec3f &v1, Vec3f &v2)
	{
		Vec3f v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}

	static float length(Vec3f &v1)
	{
		return sqrtf(dotProduct(v1, v1));
	}

	static Vec3f normalize(Vec3f &v1)
	{
		float length = Vec3f::length(v1);
		return {v1.x / length, v1.y / length, v1.z / length };
	}
};

Vec4f::operator Vec3f() const
{
	return {x, y, z};
}

Vec3f::operator Vec4f() const
{
	return {x, y, z, 0};
}


