#include <vector>
#include <string>
#include <sstream>
#include "ResourceManager.h"
#include "fstream"


bool ResourceManager::loadMesh(const std::string& filePath, Mesh &mesh)
{
	mesh.vertices.push_back(Vec3f());
	std::ifstream stream(filePath);

	if (!stream.is_open())
	{
		//stream.close();
		return false;
	}

	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);

		std::stringstream s;
		s << line;
		char junk;

		if (line[0] == 'v')
		{
			Vec3f vec;
			s >> junk >> vec.x >> vec.y >> vec.z;
			mesh.vertices.push_back(vec);
		}

		if (line[0] == 'f')
		{
			Vec3i vec;
			s >> junk >> vec.x >> vec.y >> vec.z;
			mesh.faces.push_back(vec);
		}
	}

	mesh.calculateBounds();
	stream.close();
	return true;
}
