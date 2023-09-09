#include <vector>
#include <string>
#include <sstream>
#include "ResourceManager.h"
#include "fstream"


bool ResourceManager::loadMesh(const std::string& filePath, Mesh &mesh)
{
	std::vector<Vec3f> vertices;
	std::vector<Triangle> triangles;

	vertices.emplace_back(Vec3f());
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
			vertices.emplace_back(vec);
		}

		if (line[0] == 'f')
		{
			Triangle t{};
			s >> junk >> t.face[0] >> t.face[1] >> t.face[2];
			triangles.emplace_back(t);
		}
	}

	mesh.verticesCount = vertices.size();
	mesh.trianglesCount = triangles.size();

	mesh.vertices = new Vec3f[mesh.verticesCount];
	mesh.triangles = new Triangle[mesh.trianglesCount];

	std::copy(&vertices[0], &vertices[0] + mesh.verticesCount, mesh.vertices);
	std::copy(&triangles[0], &triangles[0] + mesh.trianglesCount, mesh.triangles);

	//mesh.calculateBounds();
	stream.close();
	return true;
}
