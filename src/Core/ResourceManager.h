#pragma once
#include <iostream>
#include "Mesh.h"
#include "Texture.h"

class ResourceManager
{
public:
	static bool loadMesh(const std::string &filePath, Mesh &mesh);
	static bool loadTexturePPM(const std::string &filePath, Texture &texture);
private:
	ResourceManager() = default;
};

