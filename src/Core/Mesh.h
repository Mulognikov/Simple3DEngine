#pragma once

#include <vector>
#include "../Math/Vec3f.h"
#include "../Math/Vec3i.h"

class Mesh
{
public:
	std::vector<Vec3f> vertices;
	std::vector<Vec3i> faces;

	Vec3f boundsCenter = Vec3f(0, 0, 0);
	float boundsRadius = 0;

	void calculateBounds();
};
