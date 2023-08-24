#pragma once
#include "../Math/Vec3f.h"

class Transform
{
public:
	Vec3f position = Vec3f(0, 0, 0);
	Vec3f rotation = Vec3f(0, 0, 0);
	Vec3f scale = Vec3f(1,1,1);

	Vec3f inline getRight()
	{

	}
};

