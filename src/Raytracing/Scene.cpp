// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

#include "Raytracing/Scene.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

void Scene::loadFromString(const std::string& text)
{
	App::getLog().logInfo("Loading scene from string data");

	std::stringstream ss(text);
	cereal::JSONInputArchive archive(ss);
	archive(*this);
}

void Scene::loadFromFile(const std::string& fileName)
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
	textureList.clear();
	textureMap.clear();
	materialMap.clear();
	primitiveList.clear();

	for (ColorTexture& colorTexture : colorTextures)
		textureList.push_back(&colorTexture);

	for (CheckerboardTexture& checkerboardTexture : checkerboardTextures)
		textureList.push_back(&checkerboardTexture);

	for (ImageTexture& imageTexture : imageTextures)
		textureList.push_back(&imageTexture);

	for (WoodTexture& woodTexture : woodTextures)
		textureList.push_back(&woodTexture);

	for (MarbleTexture& marbleTexture : marbleTextures)
		textureList.push_back(&marbleTexture);

	for (Texture* texture : textureList)
	{
		texture->initialize();
		textureMap[texture->id] = texture;
	}

	for (Material& material : materials)
		materialMap[material.id] = &material;

	for (Sphere& sphere : spheres)
		primitiveList.push_back(&sphere);

	for (Plane& plane : planes)
		primitiveList.push_back(&plane);

	for (Mesh& mesh : meshes)
		primitiveList.push_back(&mesh);

	for (Primitive* primitive : primitiveList)
		primitive->initialize();

	camera.initialize();
}

Scene Scene::createTestScene()
{
	Scene scene;

	scene.camera.position = Vector3(0.0, 3.0, 0.0);
	scene.camera.orientation = EulerAngle(0.0, -20.0, 0.0);
	scene.camera.fov = 75.0;

	scene.multisamples = 1;
	scene.maxReflections = 1;

	scene.fogEnabled = false;
	scene.fogDistance = 40.0;
	scene.fogSteepness = 8.0;
	scene.fogColor = Color(1.0, 1.0, 1.0);

	ColorTexture t0;
	t0.id = 0;
	t0.color = Color(0.5, 0.5, 0.5);

	ColorTexture t1;
	t1.id = 1;
	t1.color = Color(0.5, 0.0, 0.0);

	ColorTexture t2;
	t2.id = 2;
	t2.color = Color(0.0, 0.5, 0.0);

	ColorTexture t3;
	t3.id = 3;
	t3.color = Color(0.0, 0.0, 0.5);

	scene.colorTextures.push_back(t0);
	scene.colorTextures.push_back(t1);
	scene.colorTextures.push_back(t2);
	scene.colorTextures.push_back(t3);

	CheckerboardTexture t4;
	t4.id = 4;
	t4.color1 = Color(0.0, 0.0, 0.0);
	t4.color2 = Color(1.0, 1.0, 1.0);

	scene.checkerboardTextures.push_back(t4);

	ImageTexture t5;
	t5.id = 5;
	t5.imageFilePath = "data/images/grass.jpg";

	scene.imageTextures.push_back(t5);

	WoodTexture t6;
	t6.id = 6;

	scene.woodTextures.push_back(t6);

	MarbleTexture t7;
	t7.id = 7;

	scene.marbleTextures.push_back(t7);
	
	// p1
	Material m0;
	m0.id = 0;
	m0.textureId = 0;
	m0.diffuseness = 1.0;
	m0.specularity = 0.5;
	m0.shininess = 2.0;
	m0.reflectivity = 0.0;

	// s1
	Material m1;
	m1.id = 1;
	m1.textureId = 6;
	m1.diffuseness = 1.0;
	m1.specularity = 1.0;
	m1.shininess = 1.0;
	m1.reflectivity = 0.0;

	// s2
	Material m2;
	m2.id = 2;
	m2.textureId = 7;
	m2.diffuseness = 1.0;
	m2.specularity = 1.0;
	m2.shininess = 64.0;
	m2.reflectivity = 0.0;

	// s3
	Material m3;
	m3.id = 3;
	m3.textureId = 3;
	m3.diffuseness = 1.0;
	m3.specularity = 1.0;
	m3.shininess = 8.0;
	m3.reflectivity = 1.0;

	Material m4;
	m4.id = 4;
	m4.textureId = 3;
	m4.diffuseness = 1.0;
	m4.specularity = 1.0;
	m4.shininess = 8.0;
	m4.reflectivity = 1.0;

	scene.materials.push_back(m0);
	scene.materials.push_back(m1);
	scene.materials.push_back(m2);
	scene.materials.push_back(m3);
	scene.materials.push_back(m4);

	Plane p1;
	p1.materialId = 0;
	p1.position = Vector3(0.0, 0.0, 0.0);
	p1.normal = Vector3(0.0, 1.0, 0.0).normalized();
	p1.texcoordScale = Vector2(5.0, 5.0);

	scene.planes.push_back(p1);

	Sphere s1;
	s1.materialId = 1;
	s1.position = Vector3(0.0, 1.0, -5.0);
	s1.radius = 1.0;
	s1.texcoordScale = Vector2(0.05, 0.05);

	Sphere s2;
	s2.materialId = 2;
	s2.position = Vector3(2.5, 1.0, -5.0);
	s2.radius = 1.0;

	Sphere s3;
	s3.materialId = 3;
	s3.position = Vector3(-2.5, 1.0, -5.0);
	s3.radius = 1.0;

	scene.spheres.push_back(s1);
	scene.spheres.push_back(s2);
	//scene.spheres.push_back(s3);

	Light l1;
	l1.position = Vector3(0.0, 10.0, 5.0);
	l1.color = Color(1.0, 1.0, 1.0);
	l1.intensity = 1.0;

	scene.lights.push_back(l1);

	return scene;
}
