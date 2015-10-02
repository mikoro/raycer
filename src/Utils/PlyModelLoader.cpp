// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/ModelLoader.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/StringUtils.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

/*

Following names are assumed:

Element "vertex":
x
y
z
nx (optional)
ny (optional)
nz (optional)
s (optional)
t (optional)

Element "face":
vertex_indices (list, 3 or more vertex indices, polygons are triangulated)

*/

using namespace Raycer;

namespace
{
	enum class PlyType
	{
		INT8,
		UINT8,
		INT16,
		UINT16,
		INT32,
		UINT32,
		FLOAT,
		DOUBLE,
		LIST,
		NONE
	};

	struct PlyProperty
	{
		PlyType dataType = PlyType::NONE;
		std::string name;
		PlyType listLengthType = PlyType::NONE;
		PlyType listDataType = PlyType::NONE;
	};

	struct PlyElement
	{
		std::string name;
		size_t count = 0;
		std::vector<PlyProperty> properties;
	};

	struct PlyHeader
	{
		bool isAscii = true;
		bool hasTexcoords = false;
		bool hasNormals = false;
		std::vector<PlyElement> elements;
	};

	PlyType convertTextToType(const std::string& text)
	{
		if (text == "char" || text == "int8")
			return PlyType::INT8;
		else if (text == "uchar" || text == "uint8")
			return PlyType::UINT8;
		else if (text == "short" || text == "int16")
			return PlyType::INT16;
		else if (text == "ushort" || text == "uint16")
			return PlyType::UINT16;
		else if (text == "int" || text == "int32")
			return PlyType::INT32;
		else if (text == "uint" || text == "uint32")
			return PlyType::UINT32;
		else if (text == "float" || text == "float32")
			return PlyType::FLOAT;
		else if (text == "double" || text == "float64")
			return PlyType::DOUBLE;
		else if (text == "list")
			return PlyType::LIST;
		else
			return PlyType::NONE;
	}

	double readBinaryValueAsDouble(std::ifstream& inputStream, PlyType type)
	{
		int8_t tempInt8;
		uint8_t tempUint8;
		int16_t tempInt16;
		uint16_t tempUint16;
		int32_t tempInt32;
		uint32_t tempUint32;
		float tempFloat;
		double tempDouble;

		double returnValue;

		switch (type)
		{
			case PlyType::INT8: inputStream.read(reinterpret_cast<char*>(&tempInt8), sizeof(int8_t)); returnValue = double(tempInt8); break;
			case PlyType::UINT8: inputStream.read(reinterpret_cast<char*>(&tempUint8), sizeof(uint8_t)); returnValue = double(tempUint8); break;
			case PlyType::INT16: inputStream.read(reinterpret_cast<char*>(&tempInt16), sizeof(int16_t)); returnValue = double(tempInt16); break;
			case PlyType::UINT16: inputStream.read(reinterpret_cast<char*>(&tempUint16), sizeof(uint16_t)); returnValue = double(tempUint16); break;
			case PlyType::INT32: inputStream.read(reinterpret_cast<char*>(&tempInt32), sizeof(int32_t)); returnValue = double(tempInt32); break;
			case PlyType::UINT32: inputStream.read(reinterpret_cast<char*>(&tempUint32), sizeof(uint32_t)); returnValue = double(tempUint32); break;
			case PlyType::FLOAT: inputStream.read(reinterpret_cast<char*>(&tempFloat), sizeof(float)); returnValue = double(tempFloat); break;
			case PlyType::DOUBLE: inputStream.read(reinterpret_cast<char*>(&tempDouble), sizeof(double)); returnValue = double(tempDouble); break;
			default: returnValue = 0.0;
		}

		return returnValue;
	}

	size_t readBinaryValueAsInt(std::ifstream& inputStream, PlyType type)
	{
		int8_t tempInt8;
		uint8_t tempUint8;
		int16_t tempInt16;
		uint16_t tempUint16;
		int32_t tempInt32;
		uint32_t tempUint32;
		float tempFloat;
		double tempDouble;

		size_t returnValue;

		switch (type)
		{
			case PlyType::INT8: inputStream.read(reinterpret_cast<char*>(&tempInt8), sizeof(int8_t)); returnValue = size_t(tempInt8); break;
			case PlyType::UINT8: inputStream.read(reinterpret_cast<char*>(&tempUint8), sizeof(uint8_t)); returnValue = size_t(tempUint8); break;
			case PlyType::INT16: inputStream.read(reinterpret_cast<char*>(&tempInt16), sizeof(int16_t)); returnValue = size_t(tempInt16); break;
			case PlyType::UINT16: inputStream.read(reinterpret_cast<char*>(&tempUint16), sizeof(uint16_t)); returnValue = size_t(tempUint16); break;
			case PlyType::INT32: inputStream.read(reinterpret_cast<char*>(&tempInt32), sizeof(int32_t)); returnValue = size_t(tempInt32); break;
			case PlyType::UINT32: inputStream.read(reinterpret_cast<char*>(&tempUint32), sizeof(uint32_t)); returnValue = size_t(tempUint32); break;
			case PlyType::FLOAT: inputStream.read(reinterpret_cast<char*>(&tempFloat), sizeof(float)); returnValue = size_t(tempFloat); break;
			case PlyType::DOUBLE: inputStream.read(reinterpret_cast<char*>(&tempDouble), sizeof(double)); returnValue = size_t(tempDouble); break;
			default: returnValue = 0;
		}

		return returnValue;
	}

	void readHeader(std::ifstream& inputStream, PlyHeader& header)
	{
		std::string line;
		std::getline(inputStream, line);

		if (line.find("ply") != 0)
			throw std::runtime_error("Not a PLY file");

		while (std::getline(inputStream, line))
		{
			size_t lineIndex = 0;
			std::string part;
			StringUtils::readUntilSpace(line, lineIndex, part);

			if (part.size() == 0)
				continue;

			if (part == "format")
			{
				StringUtils::readUntilSpace(line, lineIndex, part);

				if (part == "ascii")
					header.isAscii = true;
				else if (part == "binary_little_endian" || part == "binary_big_endian")
					header.isAscii = false;
				else
					throw std::runtime_error("Unknown PLY format");

				StringUtils::readUntilSpace(line, lineIndex, part);

				if (part != "1.0")
					throw std::runtime_error("Unknown PLY version");
			}
			else if (part == "element")
			{
				PlyElement element;
				StringUtils::readUntilSpace(line, lineIndex, element.name);
				StringUtils::readUntilSpace(line, lineIndex, part);
				element.count = strtol(part.c_str(), nullptr, 10);;
				header.elements.push_back(element);
			}
			else if (part == "property")
			{
				PlyProperty property;
				StringUtils::readUntilSpace(line, lineIndex, part);
				property.dataType = convertTextToType(part);

				if (property.dataType == PlyType::NONE)
					throw std::runtime_error("Unknown PLY type");

				if (property.dataType == PlyType::LIST)
				{
					StringUtils::readUntilSpace(line, lineIndex, part);
					property.listLengthType = convertTextToType(part);

					if (property.listLengthType == PlyType::NONE)
						throw std::runtime_error("Unknown PLY type");

					StringUtils::readUntilSpace(line, lineIndex, part);
					property.listDataType = convertTextToType(part);

					if (property.listDataType == PlyType::NONE)
						throw std::runtime_error("Unknown PLY type");
				}

				StringUtils::readUntilSpace(line, lineIndex, property.name);
				header.elements.back().properties.push_back(property);
			}
			else if (part == "end_header")
				return;
		}

		throw std::runtime_error("Invalid PLY file");
	}

	bool findPropertyName(const PlyElement& element, const std::string& name)
	{
		for (const PlyProperty& property : element.properties)
		{
			if (property.name == name)
				return true;
		}

		return false;
	}

	void checkHeader(PlyHeader& header)
	{
		bool hasVertexElement = false;
		bool hasFaceElement = false;

		for (const PlyElement& element : header.elements)
		{
			if (element.name == "vertex")
			{
				hasVertexElement = true;

				if (!findPropertyName(element, "x") || !findPropertyName(element, "y") || !findPropertyName(element, "z"))
					throw std::runtime_error("PLY file doesn't have correct vertex coordinates");

				if (findPropertyName(element, "s") && findPropertyName(element, "t"))
					header.hasTexcoords = true;

				if (findPropertyName(element, "nx") && findPropertyName(element, "ny") && findPropertyName(element, "nz"))
					header.hasNormals = true;
			}

			if (element.name == "face")
			{
				hasFaceElement = true;

				if (!findPropertyName(element, "vertex_indices"))
					throw std::runtime_error("PLY file doesn't have correct vertex indices");
			}
		}

		if (!hasVertexElement)
			throw std::runtime_error("PLY file doesn't have a vertex element");

		if (!hasFaceElement)
			throw std::runtime_error("PLY file doesn't have a face element");
	}

	void readAsciiData(std::ifstream& inputStream, const PlyHeader& header, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<std::vector<size_t>>& faces, const Matrix4x4& transformation, const Matrix4x4& transformationInvT)
	{
		for (const PlyElement& element : header.elements)
		{
			if (element.name == "vertex")
			{
				for (size_t i = 0; i < element.count; ++i)
				{
					std::string line;
					size_t lineIndex = 0;
					std::string part;
					std::getline(inputStream, line);
					std::map<std::string, double> propertyValues;
					
					for (size_t j = 0; j < element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// ignore and skip lists
						if (property.dataType == PlyType::LIST)
						{
							StringUtils::readUntilSpace(line, lineIndex, part);
							size_t listLength = strtol(part.c_str(), nullptr, 10);

							for (size_t k = 0; k < listLength; ++k)
								StringUtils::readUntilSpace(line, lineIndex, part);

							continue;
						}

						StringUtils::readUntilSpace(line, lineIndex, part);
						propertyValues[property.name] = StringUtils::parseDouble(part);
					}

					Vector3 vertex = Vector3(propertyValues["x"], propertyValues["y"], propertyValues["z"]);
					vertices.push_back(transformation.transformPosition(vertex));

					if (header.hasTexcoords)
						texcoords.push_back(Vector2(propertyValues["s"], propertyValues["t"]));

					if (header.hasNormals)
					{
						Vector3 normal = Vector3(propertyValues["nx"], propertyValues["ny"], propertyValues["nz"]);
						normals.push_back(transformationInvT.transformDirection(normal).normalized());
					}
				}
			}

			if (element.name == "face")
			{
				for (size_t i = 0; i < element.count; ++i)
				{
					std::string line;
					size_t lineIndex = 0;
					std::string part;
					std::getline(inputStream, line);

					for (size_t j = 0; j < element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// only process lists named vertex_indices
						if (property.dataType == PlyType::LIST && property.name == "vertex_indices")
						{
							std::vector<size_t> indices;

							StringUtils::readUntilSpace(line, lineIndex, part);
							size_t listLength = strtol(part.c_str(), nullptr, 10);

							for (size_t k = 0; k < listLength; ++k)
							{
								StringUtils::readUntilSpace(line, lineIndex, part);
								size_t index = strtol(part.c_str(), nullptr, 10);
								indices.push_back(index);
							}

							faces.push_back(indices);
						}
						// ignore and skip other lists
						else if (property.dataType == PlyType::LIST)
						{
							StringUtils::readUntilSpace(line, lineIndex, part);
							size_t listLength = strtol(part.c_str(), nullptr, 10);

							for (size_t k = 0; k < listLength; ++k)
								StringUtils::readUntilSpace(line, lineIndex, part);
						}
						// ignore other properties
						else
							StringUtils::readUntilSpace(line, lineIndex, part);
					}
				}
			}
		}
	}

	void readBinaryData(std::ifstream& inputStream, const PlyHeader& header, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<std::vector<size_t>>& faces, const Matrix4x4& transformation, const Matrix4x4& transformationInvT)
	{
		for (const PlyElement& element : header.elements)
		{
			if (element.name == "vertex")
			{
				for (size_t i = 0; i < element.count; ++i)
				{
					std::map<std::string, double> propertyValues;

					for (size_t j = 0; j < element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// ignore and skip lists
						if (property.dataType == PlyType::LIST)
						{
							size_t listLength = readBinaryValueAsInt(inputStream, property.listLengthType);

							for (size_t k = 0; k < listLength; ++k)
								readBinaryValueAsDouble(inputStream, property.listDataType);

							continue;
						}

						propertyValues[property.name] = readBinaryValueAsDouble(inputStream, property.dataType);
					}

					Vector3 vertex = Vector3(propertyValues["x"], propertyValues["y"], propertyValues["z"]);
					vertices.push_back(transformation.transformPosition(vertex));

					if (header.hasTexcoords)
						texcoords.push_back(Vector2(propertyValues["s"], propertyValues["t"]));

					if (header.hasNormals)
					{
						Vector3 normal = Vector3(propertyValues["nx"], propertyValues["ny"], propertyValues["nz"]);
						normals.push_back(transformationInvT.transformDirection(normal).normalized());
					}
				}
			}

			if (element.name == "face")
			{
				for (size_t i = 0; i < element.count; ++i)
				{
					for (size_t j = 0; j < element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// only process lists named vertex_indices
						if (property.dataType == PlyType::LIST && property.name == "vertex_indices")
						{
							std::vector<size_t> indices;
							size_t listLength = readBinaryValueAsInt(inputStream, property.listLengthType);

							for (size_t k = 0; k < listLength; ++k)
								indices.push_back(readBinaryValueAsInt(inputStream, property.listDataType));

							faces.push_back(indices);
						}
						// ignore and skip other lists
						else if (property.dataType == PlyType::LIST)
						{
							size_t listLength = readBinaryValueAsInt(inputStream, property.listLengthType);

							for (size_t k = 0; k < listLength; ++k)
								readBinaryValueAsInt(inputStream, property.listDataType);
						}
						// ignore other properties
						else
							readBinaryValueAsInt(inputStream, property.dataType);
					}
				}
			}
		}
	}

	void unpackAndTriangulate(const PlyHeader& header, const std::vector<Vector3>& vertices, const std::vector<Vector2>& texcoords, const std::vector<Vector3>& normals, const std::vector<std::vector<size_t>>& faces, const ModelLoaderInfo& info, ModelLoaderResult& result, PrimitiveGroup& combinedGroup)
	{
		Log& log = App::getLog();

		int currentId = info.idStartOffset;

		for (const std::vector<size_t>& face : faces)
		{
			if (face.size() < 3)
			{
				log.logWarning("Too few vertices (%s) in a face", face.size());
				continue;
			}

			// triangulate
			for (size_t i = 2; i < face.size(); ++i)
			{
				Triangle triangle;
				triangle.id = ++currentId;
				triangle.materialId = info.defaultMaterialId;
				triangle.invisible = info.invisibleTriangles;

				if (info.enableCombinedGroup)
					combinedGroup.primitiveIds.push_back(triangle.id);

				triangle.vertices[0] = vertices[face[0]];
				triangle.vertices[1] = vertices[face[i - 1]];
				triangle.vertices[2] = vertices[face[i]];

				if (header.hasNormals)
				{
					triangle.normals[0] = normals[face[0]];
					triangle.normals[1] = normals[face[i - 1]];
					triangle.normals[2] = normals[face[i]];
				}
				else
				{
					Vector3 v0tov1 = triangle.vertices[1] - triangle.vertices[0];
					Vector3 v0tov2 = triangle.vertices[2] - triangle.vertices[0];
					Vector3 normal = v0tov1.cross(v0tov2).normalized();

					triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = normal;
				}

				if (header.hasTexcoords)
				{
					triangle.texcoords[0] = texcoords[face[0]];
					triangle.texcoords[1] = texcoords[face[i - 1]];
					triangle.texcoords[2] = texcoords[face[i]];
				}

				result.triangles.push_back(triangle);
			}
		}
	}
}

ModelLoaderResult ModelLoader::readPlyFile(const ModelLoaderInfo& info)
{
	Log& log = App::getLog();

	log.logInfo("Reading PLY file (%s)", info.modelFilePath);

	auto startTime = std::chrono::high_resolution_clock::now();

	std::ifstream inputStream(info.modelFilePath, std::ios::in | std::ios::binary);

	if (!inputStream.good())
		throw std::runtime_error("Could not open the PLY file");

	PlyHeader header;
	readHeader(inputStream, header);
	checkHeader(header);

	std::vector<Vector3> vertices;
	std::vector<Vector2> texcoords;
	std::vector<Vector3> normals;
	std::vector<std::vector<size_t>> faces;

	Matrix4x4 scaling = Matrix4x4::scale(info.scale);
	Matrix4x4 rotation = Matrix4x4::rotateXYZ(info.rotate);
	Matrix4x4 translation = Matrix4x4::translate(info.translate);
	Matrix4x4 transformation = translation * rotation * scaling;
	Matrix4x4 transformationInvT = transformation.inverted().transposed();

	if (header.isAscii)
		readAsciiData(inputStream, header, vertices, texcoords, normals, faces, transformation, transformationInvT);
	else
		readBinaryData(inputStream, header, vertices, texcoords, normals, faces, transformation, transformationInvT);

	PrimitiveGroup combinedGroup;
	combinedGroup.id = info.combinedGroupId;
	combinedGroup.invisible = info.invisibleCombinedGroup;

	Instance combinedGroupInstance;
	combinedGroupInstance.id = info.combinedGroupInstanceId;
	combinedGroupInstance.primitiveId = combinedGroup.id;

	ModelLoaderResult result;
	unpackAndTriangulate(header, vertices, texcoords, normals, faces, info, result, combinedGroup);

	if (info.enableCombinedGroup)
	{
		result.groups.push_back(combinedGroup);

		if (info.enableCombinedGroupInstance)
			result.instances.push_back(combinedGroupInstance);
	}

	auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();

	log.logInfo("PLY file reading finished (time: %d ms, groups: %s, triangles: %s, materials: %s, textures: %s)", milliseconds, result.groups.size(), result.triangles.size(), result.materials.size(), result.textures.size());

	return result;
}
