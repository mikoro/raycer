// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>

#include "cereal/types/vector.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/archives/binary.hpp"

#include "Raytracing/Scene.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Utils/Serialization.h"

using namespace Raycer;

void Scene::loadFromFile(const std::string& fileName)
{
	App::getLog().logInfo("Loading scene from %s", fileName);

	std::ifstream file(fileName, std::ios::binary);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file");

	if (StringUtils::endsWith(fileName, ".json"))
	{
		cereal::JSONInputArchive archive(file);
		archive(*this);
	}
	else if (StringUtils::endsWith(fileName, ".xml"))
	{
		cereal::XMLInputArchive archive(file);
		archive(*this);
	}
	else if (StringUtils::endsWith(fileName, ".bin"))
	{
		// TODO: will not compile
		//cereal::BinaryInputArchive archive(file);
		//archive(*this);
	}
	else
		throw std::runtime_error("Unsupported scene file format");
}

void Scene::loadFromJsonString(const std::string& text)
{
	App::getLog().logInfo("Loading scene from JSON string");

	std::stringstream ss(text);
	cereal::JSONInputArchive archive(ss);
	archive(*this);
}

void Scene::loadFromXmlString(const std::string& text)
{
	App::getLog().logInfo("Loading scene from XML string");

	std::stringstream ss(text);
	cereal::XMLInputArchive archive(ss);
	archive(*this);
}


void Scene::saveToFile(const std::string& fileName) const
{
	App::getLog().logInfo("Saving scene to %s", fileName);

	std::ofstream file(fileName, std::ios::binary);

	if (!file.good())
		throw std::runtime_error("Could not open the scene file");

	if (StringUtils::endsWith(fileName, ".json"))
	{
		cereal::JSONOutputArchive archive(file);
		archive(cereal::make_nvp("scene", *this));
	}
	else if (StringUtils::endsWith(fileName, ".xml"))
	{
		cereal::XMLOutputArchive archive(file);
		archive(cereal::make_nvp("scene", *this));
	}
	else if (StringUtils::endsWith(fileName, ".bin"))
	{
		//cereal::BinaryOutputArchive archive(file);
		//archive(cereal::make_nvp("scene", *this));
	}
	else
		throw std::runtime_error("Unsupported scene file format");
}

std::string Scene::saveToJsonString() const
{
	App::getLog().logInfo("Saving the scene to JSON string");

	std::stringstream ss;
	cereal::JSONOutputArchive archive(ss);
	archive(*this);

	return ss.str();
}

std::string Scene::saveToXmlString() const
{
	App::getLog().logInfo("Saving the scene to XML string");

	std::stringstream ss;
	cereal::XMLOutputArchive archive(ss);
	archive(*this);

	return ss.str();
}

void Scene::initialize()
{
	texturesList.clear();
	texturesMap.clear();
	materialsMap.clear();
	primitivesList.clear();

	for (ColorTexture& texture : textures.colorTextures)
		texturesList.push_back(&texture);

	for (CheckerTexture& texture : textures.checkerTextures)
		texturesList.push_back(&texture);

	for (ImageTexture& texture : textures.imageTextures)
		texturesList.push_back(&texture);

	for (PerlinNoiseTexture& texture : textures.perlinNoiseTextures)
		texturesList.push_back(&texture);

	for (CellNoiseTexture& texture : textures.cellNoiseTextures)
		texturesList.push_back(&texture);

	for (MarbleTexture& texture : textures.marbleTextures)
		texturesList.push_back(&texture);

	for (WoodTexture& texture : textures.woodTextures)
		texturesList.push_back(&texture);

	for (FireTexture& texture : textures.fireTextures)
		texturesList.push_back(&texture);

	for (Texture* texture : texturesList)
	{
		texture->initialize();
		texturesMap[texture->id] = texture;
	}

	for (Material& material : materials)
		materialsMap[material.id] = &material;

	for (Plane& plane : primitives.planes)
		primitivesList.push_back(&plane);

	for (Sphere& sphere : primitives.spheres)
		primitivesList.push_back(&sphere);

	for (Mesh& mesh : primitives.meshes)
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

	scene.camera.position = Vector3(0.0, 6.0, 8.0);
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

	scene.textures.colorTextures.push_back(t0);
	scene.textures.colorTextures.push_back(t1);
	scene.textures.colorTextures.push_back(t2);
	scene.textures.colorTextures.push_back(t3);

	CheckerTexture t4;
	t4.id = 4;

	scene.textures.checkerTextures.push_back(t4);

	ImageTexture t5;
	t5.id = 5;
	t5.imageFilePath = "data/images/grass.jpg";

	scene.textures.imageTextures.push_back(t5);

	WoodTexture t6;
	t6.id = 6;

	scene.textures.woodTextures.push_back(t6);

	MarbleTexture t7;
	t7.id = 7;

	scene.textures.marbleTextures.push_back(t7);

	FireTexture t8;
	t8.id = 8;

	scene.textures.fireTextures.push_back(t8);
	
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
	m1.textureId = 6;
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
	m3.textureId = 8;
	m3.diffuseness = 1.0;
	m3.specularity = 0.0;
	m3.shininess = 0.0;
	m3.reflectivity = 0.0;

	// tr1
	Material m4;
	m4.id = 4;
	m4.textureId = 1;
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

	scene.primitives.planes.push_back(p1);

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

	//scene.primitives.spheres.push_back(s1);
	//scene.primitives.spheres.push_back(s2);
	//scene.primitives.spheres.push_back(s3);

	Mesh mh1;
	mh1.materialId = 4;
	mh1.position = Vector3(0.0, 2.0, 0.0);
	mh1.scale = Vector3(1.0, 1.0, 1.0);
	mh1.orientation = EulerAngle(0.0, 0.0, 0.0);
	mh1.meshFilePath = "data/meshes/monkey1.obj";

	scene.primitives.meshes.push_back(mh1);

	AmbientLight l1;
	l1.color = Color::WHITE;
	l1.intensity = 0.1;

	scene.lights.ambientLights.push_back(l1);

	DirectionalLight l2;
	l2.color = Color(192, 191, 173);
	l2.direction = EulerAngle(45.0, -45.0, 0).getDirectionVector();
	l2.intensity = 1.0;

	scene.lights.directionalLights.push_back(l2);

	return scene;
}
