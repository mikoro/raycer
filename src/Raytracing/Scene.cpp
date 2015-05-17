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
		// TODO: will not compile
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

Scene Scene::createTestScene1()
{
	Scene scene;

	scene.tracer.maxIterations = 3;

	scene.multisampler.type = MultisampleType::NONE;
	scene.multisampler.multisamples = 4;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 3.0, 6.0);
	scene.camera.orientation = EulerAngle(0.0, -20.0, 0.0);
	scene.camera.fov = 75.0;

	// FLOOR //

	/*
	CheckerTexture floorTexture;
	floorTexture.id = 0;
	floorTexture.color1 = Color(0.1, 0.1, 0.1);
	floorTexture.color2 = Color(0.3, 0.3, 0.3);
	*/

	/*
	PerlinNoiseTexture floorTexture;
	floorTexture.id = 0;
	floorTexture.intensity = 0.5;
	floorTexture.seed = 1;
	floorTexture.scale = Vector3(1.0, 1.0, 1.0);
	floorTexture.baseColor = Color(1.0, 1.0, 1.0);
	floorTexture.isFbm = false;
	floorTexture.octaves = 4;
	floorTexture.lacunarity = 2.0;
	floorTexture.persistence = 0.5;
	*/

	CellNoiseTexture floorTexture;
	floorTexture.id = 0;
	floorTexture.intensity = 0.5;
	floorTexture.seed = 1;
	floorTexture.scale = Vector3(10.0, 10.0, 10.0);
	floorTexture.baseColor = Color(1.0, 1.0, 1.0);
	floorTexture.distanceType = CellNoiseDistanceType::EUCLIDEAN;
	floorTexture.combineType = CellNoiseCombineType::D1_MINUS_D2;
	floorTexture.density = 1;

	Material floorMaterial;
	floorMaterial.id = 0;
	floorMaterial.textureId = floorTexture.id;
	floorMaterial.ambientness = 1.0;
	floorMaterial.diffuseness = 1.0;
	floorMaterial.specularity = 1.0;
	floorMaterial.shininess = 2.0;
	floorMaterial.reflectance = 1.0;
	floorMaterial.transmittance = 0.0;
	floorMaterial.refractiveIndex = 1.0;

	Plane floorPlane;
	floorPlane.materialId = floorMaterial.id;
	floorPlane.position = Vector3(0.0, 0.0, 0.0);
	floorPlane.normal = Vector3(0.0, 1.0, 0.0).normalized();
	floorPlane.texcoordScale = Vector2(5.0, 5.0);

	scene.textures.cellNoiseTextures.push_back(floorTexture);
	scene.materials.push_back(floorMaterial);
	scene.primitives.planes.push_back(floorPlane);

	// SPHERE 1 //

	ColorTexture sphere1Texture;
	sphere1Texture.id = 1;
	sphere1Texture.color = Color(1.0, 0.0, 0.0);
	sphere1Texture.intensity = 0.3;

	Material sphere1Material;
	sphere1Material.id = 1;
	sphere1Material.textureId = sphere1Texture.id;
	sphere1Material.ambientness = 1.0;
	sphere1Material.diffuseness = 1.0;
	sphere1Material.specularity = 1.0;
	sphere1Material.shininess = 32.0;
	sphere1Material.reflectance = 0.5;
	sphere1Material.transmittance = 0.0;
	sphere1Material.refractiveIndex = 1.0;
	sphere1Material.isFresnel = false;

	Sphere sphere1;
	sphere1.materialId = sphere1Material.id;
	sphere1.texcoordScale = Vector2(1.0, 1.0);
	sphere1.position = Vector3(0.0, 1.0, 0.0);
	sphere1.radius = 1.0;
	
	scene.textures.colorTextures.push_back(sphere1Texture);
	scene.materials.push_back(sphere1Material);
	scene.primitives.spheres.push_back(sphere1);

	// SPHERE 2 //

	ColorTexture sphere2Texture;
	sphere2Texture.id = 2;
	sphere2Texture.color = Color(1.0, 1.0, 1.0);
	sphere2Texture.intensity = 1.0;

	Material sphere2Material;
	sphere2Material.id = 2;
	sphere2Material.textureId = sphere2Texture.id;
	sphere2Material.ambientness = 0.0;
	sphere2Material.diffuseness = 0.0;
	sphere2Material.specularity = 0.5;
	sphere2Material.shininess = 32.0;
	sphere2Material.reflectance = 1.0;
	sphere2Material.transmittance = 1.0;
	sphere2Material.refractiveIndex = 1.5;
	sphere2Material.isFresnel = true;

	Sphere sphere2;
	sphere2.materialId = sphere2Material.id;
	sphere2.texcoordScale = Vector2(1.0, 1.0);
	sphere2.position = Vector3(-3.0, 1.0, 0.0);
	sphere2.radius = 1.0;

	scene.textures.colorTextures.push_back(sphere2Texture);
	scene.materials.push_back(sphere2Material);
	scene.primitives.spheres.push_back(sphere2);

	// LIGHTS //

	AmbientLight ambientLight1;
	ambientLight1.color = Color(1.0, 1.0, 1.0);
	ambientLight1.intensity = 0.1;

	scene.lights.ambientLights.push_back(ambientLight1);

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(192, 191, 173);
	directionalLight1.direction = EulerAngle(45.0, -45.0, 0).getDirectionVector();
	directionalLight1.intensity = 1.5;

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}

Scene Scene::createTestScene2()
{
	Scene scene;

	scene.tracer.maxIterations = 3;

	scene.multisampler.type = MultisampleType::NONE;
	scene.multisampler.multisamples = 4;

	// CAMERA //

	scene.camera.position = Vector3(0.0, 3.0, 6.0);
	scene.camera.orientation = EulerAngle(0.0, -20.0, 0.0);
	scene.camera.fov = 75.0;

	// PLANE 1 //

	CheckerTexture plane1Texture;
	plane1Texture.id = 0;
	plane1Texture.color1 = Color(0.0, 0.0, 0.4);
	plane1Texture.color2 = Color(0.0, 0.0, 0.2);

	Material plane1Material;
	plane1Material.id = 0;
	plane1Material.textureId = plane1Texture.id;
	plane1Material.ambientness = 1.0;
	plane1Material.diffuseness = 1.0;
	plane1Material.specularity = 0.0;
	plane1Material.shininess = 0.0;
	plane1Material.reflectance = 0.0;
	plane1Material.transmittance = 0.0;
	plane1Material.refractiveIndex = 1.0;
	plane1Material.isFresnel = false;

	Plane plane1;
	plane1.materialId = plane1Material.id;
	plane1.position = Vector3(0.0, -1.0, 0.0);
	plane1.normal = Vector3(0.0, 1.0, 0.0).normalized();
	plane1.texcoordScale = Vector2(1.0, 1.0);

	scene.textures.checkerTextures.push_back(plane1Texture);
	scene.materials.push_back(plane1Material);
	scene.primitives.planes.push_back(plane1);

	// PLANE 2 //

	ColorTexture plane2Texture;
	plane2Texture.id = 1;
	plane2Texture.color = Color(1.0, 1.0, 1.0);
	plane2Texture.intensity = 1.0;

	Material plane2Material;
	plane2Material.id = 1;
	plane2Material.textureId = plane2Texture.id;
	plane2Material.ambientness = 0.0;
	plane2Material.diffuseness = 0.0;
	plane2Material.specularity = 0.0;
	plane2Material.shininess = 16.0;
	plane2Material.reflectance = 1.0;
	plane2Material.transmittance = 1.0;
	plane2Material.refractiveIndex = 1.5;
	plane2Material.isFresnel = true;

	Plane plane2;
	plane2.materialId = plane2Material.id;
	plane2.position = Vector3(0.0, 0.0, 0.0);
	plane2.normal = Vector3(0.0, 1.0, 0.0).normalized();
	plane2.texcoordScale = Vector2(1.0, 1.0);

	scene.textures.colorTextures.push_back(plane2Texture);
	scene.materials.push_back(plane2Material);
	scene.primitives.planes.push_back(plane2);

	// LIGHTS //

	AmbientLight ambientLight1;
	ambientLight1.color = Color(1.0, 1.0, 1.0);
	ambientLight1.intensity = 0.5;

	scene.lights.ambientLights.push_back(ambientLight1);

	DirectionalLight directionalLight1;
	directionalLight1.color = Color(192, 191, 173);
	directionalLight1.direction = EulerAngle(45.0, -45.0, 0).getDirectionVector();
	directionalLight1.intensity = 1.0;

	scene.lights.directionalLights.push_back(directionalLight1);

	return scene;
}
