// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

#include "CpuRaytracing/Scene.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

void Scene::load(const std::string& fileName)
{
	App::getLog().logInfo("Loading scene from %s", fileName);

	std::ifstream file(fileName);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file");

	cereal::JSONInputArchive archive(file);
	archive(*this);
}

void Scene::saveAs(const std::string& fileName) const
{
	App::getLog().logInfo("Saving scene to %s", fileName);

	std::ofstream file(fileName);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file");

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
