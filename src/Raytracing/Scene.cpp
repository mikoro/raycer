// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

#include "Raytracing/Scene.h"

using namespace Raycer;

Scene::Scene()
{
}

Scene::Scene(const std::string fileName)
{
	load(fileName);
}

void Scene::load(const std::string fileName)
{
	std::ifstream file(fileName);
	cereal::JSONInputArchive archive(file);
	archive(*this);
}

void Scene::saveAs(const std::string fileName) const
{
	std::ofstream file(fileName);
	cereal::JSONOutputArchive archive(file);
	archive(cereal::make_nvp("scene", *this));
}

void Scene::initialize()
{
	primitives.clear();

	for (Sphere& sphere : spheres)
		primitives.push_back(&sphere);

	for (Plane& plane : planes)
		primitives.push_back(&plane);

	for (Mesh& mesh : meshes)
		primitives.push_back(&mesh);
}
