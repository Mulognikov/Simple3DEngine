#pragma once

#include <vector>
#include "../Math/Vec3f.h"
#include "../Math/Vec3i.h"

class Mesh
{
public:
	std::vector<Vec3f> vertices;
	std::vector<Vec3i> faces;
};
