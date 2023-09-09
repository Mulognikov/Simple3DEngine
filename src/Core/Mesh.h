#pragma once

#include <vector>
#include "../Math/Vec3f.h"
#include "Triangle.h"

class Mesh
{
public:
	Vec3f *vertices;
	Triangle *triangles;

	uint32_t verticesCount;
	uint32_t trianglesCount;

	Vec3f boundsCenter = Vec3f(0, 0, 0);
	float boundsRadius = 0;

	void calculateBounds();
};
