#include "Mesh.h"

void Mesh::calculateBounds()
{
	for(auto vertex : vertices)
	{
		boundsCenter = boundsCenter + vertex;
	}

	boundsCenter = boundsCenter / vertices.size();

	for(auto vertex : vertices)
	{
		Vec3f distance = vertex - boundsCenter;
		float length = Vec3f::length(distance);
		if (length > boundsRadius) boundsRadius = length;
	}
}
