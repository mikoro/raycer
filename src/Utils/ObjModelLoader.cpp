// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <chrono>
#include <fstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "tinyformat/tinyformat.h"

#include "Utils/ObjModelLoader.h"
#include "App.h"
#include "Utils/Log.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Math/Matrix4x4.h"

using namespace Raycer;
using namespace boost::filesystem;

ModelLoaderResult ObjModelLoader::readFile(const ModelLoaderInfo& info)
{
	Log& log = App::getLog();

	log.logInfo("Reading OBJ file (%s)", info.modelFilePath);

	auto startTime = std::chrono::high_resolution_clock::now();

	std::ifstream file(info.modelFilePath);

	if (!file.good())
		throw std::runtime_error("Could not open the OBJ file");

	ModelLoaderResult result;

	result.allGroup.id = info.allGroupId;
	result.allGroup.invisible = true;

	result.addAllGroup = info.addAllGroup;
	result.addAllInstance = info.addAllInstance;
	result.addGroups = info.addGroups;
	result.addGroupInstances = info.addGroupInstances;

	if (info.addAllInstance)
		result.addAllGroup = true;

	if (info.addGroupInstances)
		result.addGroups = true;

	int currentId = info.idStartOffset;
	int currentMaterialId = info.defaultMaterialId;
	bool invisible = info.invisibleTriangles || info.addAllInstance || info.addGroupInstances;
	
	std::map<std::string, int> materialsMap;
	std::string objFileDirectory = boost::filesystem::absolute(info.modelFilePath).parent_path().string();

	Matrix4x4 scaling = Matrix4x4::scale(info.scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(info.rotate);
	Matrix4x4 translation = Matrix4x4::translate(info.translate);
	Matrix4x4 transformation = translation * rotation * scaling;
	Matrix4x4 transformationInvT = transformation.inverted().transposed();

	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	std::string line;

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string part;
		ss >> part;

		// new material file
		if (part == "mtllib" && !info.ignoreMaterials)
		{
			ss >> part;
			processMaterialFile(objFileDirectory, part, result, materialsMap, currentId);
			continue;
		}

		// new group
		if (part == "g")
		{
			result.groups.push_back(PrimitiveGroup());
			result.groups.back().id = ++currentId;
			result.groups.back().invisible = true;
			continue;
		}

		// select material
		if (part == "usemtl" && !info.ignoreMaterials)
		{
			ss >> part;

			if (!materialsMap.count(part))
				throw std::runtime_error(tfm::format("Could not find material named \"%s\"", part));

			currentMaterialId = materialsMap[part];
			continue;
		}

		// vertex
		if (part == "v")
		{
			Vector3 vertex;

			ss >> vertex.x;
			ss >> vertex.y;
			ss >> vertex.z;

			vertices.push_back(transformation.transformPosition(vertex));
			continue;
		}

		// normal
		if (part == "vn")
		{
			Vector3 normal;

			ss >> normal.x;
			ss >> normal.y;
			ss >> normal.z;

			normals.push_back(transformationInvT.transformDirection(normal));
			continue;
		}

		// texcoord
		if (part == "vt")
		{
			Vector2 texcoord;

			ss >> texcoord.x;
			ss >> texcoord.y;

			texcoords.push_back(texcoord);
			continue;
		}

		// face
		if (part == "f")
			processFace(ss, vertices, normals, texcoords, result, currentId, currentMaterialId, invisible);
	}

	file.close();

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	int milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("OBJ file reading finished (time: %d ms)", milliseconds);

	return result;
}

namespace
{
	std::string getAbsolutePath(const std::string& rootDirectory, const std::string& relativeFilePath)
	{
		path tempPath(rootDirectory);
		tempPath.append(relativeFilePath.begin(), relativeFilePath.end());
		std::string tempPathString = tempPath.string();
		std::replace(tempPathString.begin(), tempPathString.end(), '\\', '/');
		return tempPathString;
	}
}

void ObjModelLoader::processMaterialFile(const std::string& objFileDirectory, const std::string& mtlFilePath, ModelLoaderResult& result, std::map<std::string, int>& materialsMap, int& currentId)
{
	std::string absoluteMtlFilePath = getAbsolutePath(objFileDirectory, mtlFilePath);
	App::getLog().logInfo("Reading MTL file (%s)", absoluteMtlFilePath);
	std::ifstream file(absoluteMtlFilePath);

	if (!file.good())
		throw std::runtime_error("Could not open the MTL file");

	Material currentMaterial;
	bool materialPending = false;
	bool hasAmbientMap = false;
	bool hasDiffuseMap = false;
	bool hasSpecularMap = false;
	bool hasNormalMap = false;
	bool hasHeightMap = false;

	std::string line;

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string part;
		ss >> part;

		// new material
		if (part == "newmtl")
		{
			if (materialPending)
			{
				result.materials.push_back(currentMaterial);

				hasAmbientMap = false;
				hasDiffuseMap = false;
				hasSpecularMap = false;
				hasNormalMap = false;
				hasHeightMap = false;
			}

			materialPending = true;

			currentMaterial = Material();
			currentMaterial.id = ++currentId;

			ss >> part;
			materialsMap[part] = currentMaterial.id;

			continue;
		}

		// ambient reflectance
		if (part == "Ka")
		{
			ss >> currentMaterial.ambientReflectance.r;
			ss >> currentMaterial.ambientReflectance.g;
			ss >> currentMaterial.ambientReflectance.b;

			continue;
		}

		// diffuse reflectance
		if (part == "Kd")
		{
			ss >> currentMaterial.diffuseReflectance.r;
			ss >> currentMaterial.diffuseReflectance.g;
			ss >> currentMaterial.diffuseReflectance.b;

			continue;
		}

		// specular reflectance
		if (part == "Ks")
		{
			ss >> currentMaterial.specularReflectance.r;
			ss >> currentMaterial.specularReflectance.g;
			ss >> currentMaterial.specularReflectance.b;

			continue;
		}

		// shininess
		if (part == "Ns")
		{
			ss >> currentMaterial.shininess;
			continue;
		}

		// refractive index
		if (part == "Ni")
		{
			ss >> currentMaterial.refractiveIndex;
			continue;
		}

		// ambient map
		if (part == "map_Ka" && !hasAmbientMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.ambientMapTextureId = imageTexture.id;

			std::string imageFilePath;
			ss >> imageFilePath;
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, imageFilePath);

			result.textures.push_back(imageTexture);
			hasAmbientMap = true;

			continue;
		}

		// diffuse map
		if (part == "map_Kd" && !hasDiffuseMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.diffuseMapTextureId = imageTexture.id;

			std::string imageFilePath;
			ss >> imageFilePath;
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, imageFilePath);

			result.textures.push_back(imageTexture);
			hasDiffuseMap = true;

			continue;
		}

		// specular map
		if (part == "map_Ks" && !hasSpecularMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.specularMapTextureId = imageTexture.id;

			std::string imageFilePath;
			ss >> imageFilePath;
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, imageFilePath);

			result.textures.push_back(imageTexture);
			hasSpecularMap = true;

			continue;
		}

		// bump map
		if ((part == "map_bump" || part == "bump") && !hasNormalMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			imageTexture.isBumpMap = true;
			currentMaterial.normalMapTextureId = imageTexture.id;

			std::string imageFilePath;
			ss >> imageFilePath;
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, imageFilePath);

			result.textures.push_back(imageTexture);
			hasNormalMap = true;

			continue;
		}

		// normal map
		if (part == "map_normal" && !hasNormalMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			imageTexture.isNormalMap = true;
			currentMaterial.normalMapTextureId = imageTexture.id;

			std::string imageFilePath;
			ss >> imageFilePath;
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, imageFilePath);

			result.textures.push_back(imageTexture);
			hasNormalMap = true;

			continue;
		}

		// height map
		if (part == "map_height" && !hasHeightMap)
		{
			ImageTexture imageTexture;
			imageTexture.id = ++currentId;
			currentMaterial.heightMapTextureId = imageTexture.id;

			std::string imageFilePath;
			ss >> imageFilePath;
			imageTexture.imageFilePath = getAbsolutePath(objFileDirectory, imageFilePath);

			result.textures.push_back(imageTexture);
			hasHeightMap = true;

			continue;
		}
	}

	file.close();

	if (materialPending)
		result.materials.push_back(currentMaterial);
}

void ObjModelLoader::processFace(std::istringstream& ss, std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& texcoords, ModelLoaderResult& result, int& currentId, int currentMaterialId, bool invisible)
{
	std::vector<int> vertexIndices;
	std::vector<int> normalIndices;
	std::vector<int> texcoordIndices;

	std::string part;
	ss >> part;

	// determine what indices are available from the slash count
	int slashCount = (int)std::count(part.begin(), part.end(), '/');
	bool doubleSlash = (part.find("//") != std::string::npos);
	bool hasTexcoords = (slashCount > 0 && !doubleSlash);
	bool hasNormals = (slashCount > 1);

	do
	{
		// stringstream likes spaces
		std::replace(part.begin(), part.end(), '/', ' ');
		std::istringstream ssp(part);

		int vertexIndex;
		ssp >> vertexIndex;

		if (vertexIndex < 0)
			vertexIndex = (int)vertices.size() + vertexIndex;
		else
			vertexIndex--;

		if (vertexIndex < 0 || vertexIndex >= (int)vertices.size())
			throw std::runtime_error("Vertex index was out of bounds");

		vertexIndices.push_back(vertexIndex);

		if (hasTexcoords)
		{
			int texcoordIndex;
			ssp >> texcoordIndex;

			if (texcoordIndex < 0)
				texcoordIndex = (int)texcoords.size() + texcoordIndex;
			else
				texcoordIndex--;

			if (texcoordIndex < 0 || texcoordIndex >= (int)texcoords.size())
				throw std::runtime_error("Texcoord index was out of bounds");

			texcoordIndices.push_back(texcoordIndex);
		}

		if (hasNormals)
		{
			int normalIndex;
			ssp >> normalIndex;

			if (normalIndex < 0)
				normalIndex = (int)normals.size() + normalIndex;
			else
				normalIndex--;

			if (normalIndex < 0 || normalIndex >= (int)normals.size())
				throw std::runtime_error("Normal index was out of bounds");

			normalIndices.push_back(normalIndex);
		}

	} while (ss >> part);

	if (vertexIndices.size() < 3)
		throw std::runtime_error("Too few vertices in a face");

	// triangulate
	for (int i = 2; i < (int)vertexIndices.size(); ++i)
	{
		Triangle triangle;
		triangle.id = ++currentId;
		triangle.materialId = currentMaterialId;
		triangle.invisible = invisible;

		if (result.groups.size() > 0)
			result.groups.back().primitiveIds.push_back(triangle.id);

		result.allGroup.primitiveIds.push_back(triangle.id);

		triangle.vertices[0] = vertices[vertexIndices[0]];
		triangle.vertices[1] = vertices[vertexIndices[i - 1]];
		triangle.vertices[2] = vertices[vertexIndices[i]];

		if (hasNormals)
		{
			triangle.normals[0] = normals[normalIndices[0]];
			triangle.normals[1] = normals[normalIndices[i - 1]];
			triangle.normals[2] = normals[normalIndices[i]];
		}
		else
		{
			Vector3 v0tov1 = triangle.vertices[1] - triangle.vertices[0];
			Vector3 v0tov2 = triangle.vertices[2] - triangle.vertices[0];
			Vector3 normal = v0tov1.cross(v0tov2).normalized();

			triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = normal;
		}

		if (hasTexcoords)
		{
			triangle.texcoords[0] = texcoords[texcoordIndices[0]];
			triangle.texcoords[1] = texcoords[texcoordIndices[i - 1]];
			triangle.texcoords[2] = texcoords[texcoordIndices[i]];
		}

		result.triangles.push_back(triangle);
	}
}
