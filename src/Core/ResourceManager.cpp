#include <vector>
#include <string>
#include <sstream>
#include "ResourceManager.h"
#include "fstream"


bool ResourceManager::loadMesh(const std::string& filePath, Mesh &mesh)
{
	std::vector<Vec3f> vertices;
	std::vector<Vec3f> uvs;
	std::vector<Triangle> triangles;

	vertices.emplace_back();
	uvs.emplace_back();
	std::ifstream stream(filePath);

	if (!stream.is_open()) return false;

	while (!stream.eof())
	{
		std::string line;
		std::getline(stream, line);

		std::stringstream s;
		s << line;
		char junk;
		std::string type = line.substr(0,2);

		if (type == "v ")
		{
			Vec3f vec;
			s >> junk >> vec.x >> vec.y >> vec.z;
			vertices.emplace_back(vec);
		}

		if (type == "vt")
		{
			Vec3f uv;
			s >> junk >> junk >> uv.x >> uv.y;
			uvs.emplace_back(uv);
		}

		if (type == "f " && uvs.size() > 1)
		{
			Triangle t{};
			std::string values[3];
			s >> junk >> values[0] >> values[1]  >> values[2];

			for (int i = 0; i < 3; i++)
			{
				std::stringstream valueStream(values[i]);
				std::string f, uv;
				std::getline(valueStream, f, '/');
				std::getline(valueStream, uv, '/');
				t.face[i] = std::stoi(f);
				t.uv[i] = std::stoi(uv);
			}

			triangles.emplace_back(t);
		}
		else if (type == "f ")
		{
			Triangle t{};
			s >> junk >> t.face[0] >> t.face[1] >> t.face[2];
			triangles.emplace_back(t);
		}
	}

	mesh.verticesCount = vertices.size();
	mesh.textureVerticesCount = uvs.size();
	mesh.trianglesCount = triangles.size();

	mesh.vertices = new Vec3f[mesh.verticesCount];
	mesh.textureVertices = new Vec3f[mesh.textureVerticesCount];
	mesh.triangles = new Triangle[mesh.trianglesCount];


	std::copy(&vertices[0], &vertices[0] + mesh.verticesCount, mesh.vertices);
	std::copy(&triangles[0], &triangles[0] + mesh.trianglesCount, mesh.triangles);

	if (uvs.size() > 1) std::copy(&uvs[0], &uvs[0] + mesh.textureVerticesCount, mesh.textureVertices);

	//mesh.calculateBounds();
	stream.close();
	return true;
}

bool ResourceManager::loadTexturePPM(const std::string &filePath, Texture &texture)
{
	std::ifstream stream(filePath, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(stream), {});

	uint16_t index = 0;
	while (buffer[index] != '\n') index++;

	index++;

	std::string textureSizeString;

	while (buffer[index] != '\n')
	{
		textureSizeString += buffer[index];
		index++;
	}

	std::stringstream strStream;
	strStream << textureSizeString;
	strStream >> texture.width >> texture.height;
	texture.color = new uint8_t[texture.width * texture.height];

	index++;
	while (buffer[index] != '\n') index++;
	index++;
	int textureIndex = 0;

	for (int i = index; i < buffer.size(); i += 3)
	{
		uint8_t r = buffer[i];
		uint8_t g = buffer[i + 1];
		uint8_t b = buffer[i + 2];

		r /= 36;
		g /= 36;
		b /= 85;

		uint8_t color = r;
		color = color << 3;
		color = color | g;
		color = color << 2;
		color = color | b;

		texture.color[textureIndex] = color;
		textureIndex++;
	}

	stream.close();
	return true;
}
