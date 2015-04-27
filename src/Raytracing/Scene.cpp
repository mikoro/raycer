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
	texturesList.clear();
	texturesMap.clear();
	materialsMap.clear();
	primitivesList.clear();

	for (ColorTexture& colorTexture : colorTextures)
		texturesList.push_back(&colorTexture);

	for (CheckerTexture& checkerTexture : checkerTextures)
		texturesList.push_back(&checkerTexture);

	for (ImageTexture& imageTexture : imageTextures)
		texturesList.push_back(&imageTexture);

	for (WoodTexture& woodTexture : woodTextures)
		texturesList.push_back(&woodTexture);

	for (MarbleTexture& marbleTexture : marbleTextures)
		texturesList.push_back(&marbleTexture);

	for (Texture* texture : texturesList)
	{
		texture->initialize();
		texturesMap[texture->id] = texture;
	}

	for (Material& material : materials)
		materialsMap[material.id] = &material;

	for (Sphere& sphere : spheres)
		primitivesList.push_back(&sphere);

	for (Plane& plane : planes)
		primitivesList.push_back(&plane);

	for (Mesh& mesh : meshes)
		primitivesList.push_back(&mesh);

	for (Primitive* primitive : primitivesList)
		primitive->initialize();

	camera.initialize();
	validate();
}

void Scene::validate()
{
	for (Primitive* primitive : primitivesList)
	{
		if (materialsMap.count(primitive->materialId))
		{
			Material* material = materialsMap[primitive->materialId];

			if (!texturesMap.count(material->textureId))
				throw std::runtime_error("A material has an invalid texture id");
		}
		else
			throw std::runtime_error("A primitive has an invalid material id");
	}
}

Scene Scene::createTestScene()
{
	Scene scene;

	scene.tracer.maxReflections = 1;

	scene.multisampler.type = MultisampleType::NONE;
	scene.multisampler.multisamples = 4;

	scene.toneMapper.type = ToneMapType::GAMMA;
	scene.toneMapper.gamma = 1.0 / 2.2;

	scene.camera.position = Vector3(0.0, 3.0, 0.0);
	scene.camera.orientation = EulerAngle(0.0, -20.0, 0.0);
	scene.camera.fov = 75.0;

	scene.fog.enabled = false;
	scene.fog.distance = 40.0;
	scene.fog.steepness = 8.0;
	scene.fog.color = Color(1.0, 1.0, 1.0);

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

	CheckerTexture t4;
	t4.id = 4;

	scene.checkerTextures.push_back(t4);

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
	m0.textureId = 4;
	m0.diffuseness = 1.0;
	m0.specularity = 0.0;
	m0.shininess = 0.0;
	m0.reflectivity = 0.0;

	// s1
	Material m1;
	m1.id = 1;
	m1.textureId = 4;
	m1.diffuseness = 1.0;
	m1.specularity = 0.0;
	m1.shininess = 2.0;
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
	m4.textureId = 0;
	m4.diffuseness = 1.0;
	m4.specularity = 0.0;
	m4.shininess = 0.0;
	m4.reflectivity = 0.0;

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

	//scene.spheres.push_back(s1);
	//scene.spheres.push_back(s2);
	//scene.spheres.push_back(s3);

	Light l1;
	l1.position = Vector3(2.0, 10.0, 5.0);
	l1.color = Color(1.0, 1.0, 1.0);
	l1.intensity = 1.0;

	scene.lights.push_back(l1);

	return scene;
}