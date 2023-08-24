#pragma once
#include <iostream>
#include "Mesh.h"

class ResourceManager
{
public:
	static bool loadMesh(const std::string& filePath, Mesh &mesh);
private:
	ResourceManager() = default;
};

