// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>
#include <sstream>
#include <map>

#include "Utils/PlyReader.h"
#include "App.h"
#include "Utils/Log.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Raytracing/Primitives/Triangle.h"

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
		int count = 0;
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
			case PlyType::INT8: inputStream.read((char*)&tempInt8, sizeof(int8_t)); returnValue = (double)tempInt8; break;
			case PlyType::UINT8: inputStream.read((char*)&tempUint8, sizeof(uint8_t)); returnValue = (double)tempUint8; break;
			case PlyType::INT16: inputStream.read((char*)&tempInt16, sizeof(int16_t)); returnValue = (double)tempInt16; break;
			case PlyType::UINT16: inputStream.read((char*)&tempUint16, sizeof(uint16_t)); returnValue = (double)tempUint16; break;
			case PlyType::INT32: inputStream.read((char*)&tempInt32, sizeof(int32_t)); returnValue = (double)tempInt32; break;
			case PlyType::UINT32: inputStream.read((char*)&tempUint32, sizeof(uint32_t)); returnValue = (double)tempUint32; break;
			case PlyType::FLOAT: inputStream.read((char*)&tempFloat, sizeof(float)); returnValue = (double)tempFloat; break;
			case PlyType::DOUBLE: inputStream.read((char*)&tempDouble, sizeof(double)); returnValue = (double)tempDouble; break;
			default: returnValue = 0.0;
		}

		return returnValue;
	}

	int readBinaryValueAsInt(std::ifstream& inputStream, PlyType type)
	{
		int8_t tempInt8;
		uint8_t tempUint8;
		int16_t tempInt16;
		uint16_t tempUint16;
		int32_t tempInt32;
		uint32_t tempUint32;
		float tempFloat;
		double tempDouble;

		int returnValue;

		switch (type)
		{
			case PlyType::INT8: inputStream.read((char*)&tempInt8, sizeof(int8_t)); returnValue = (int)tempInt8; break;
			case PlyType::UINT8: inputStream.read((char*)&tempUint8, sizeof(uint8_t)); returnValue = (int)tempUint8; break;
			case PlyType::INT16: inputStream.read((char*)&tempInt16, sizeof(int16_t)); returnValue = (int)tempInt16; break;
			case PlyType::UINT16: inputStream.read((char*)&tempUint16, sizeof(uint16_t)); returnValue = (int)tempUint16; break;
			case PlyType::INT32: inputStream.read((char*)&tempInt32, sizeof(int32_t)); returnValue = (int)tempInt32; break;
			case PlyType::UINT32: inputStream.read((char*)&tempUint32, sizeof(uint32_t)); returnValue = (int)tempUint32; break;
			case PlyType::FLOAT: inputStream.read((char*)&tempFloat, sizeof(float)); returnValue = (int)tempFloat; break;
			case PlyType::DOUBLE: inputStream.read((char*)&tempDouble, sizeof(double)); returnValue = (int)tempDouble; break;
			default: returnValue = 0;
		}

		return returnValue;
	}

	void readHeader(std::ifstream& inputStream, PlyHeader& header)
	{
		std::string line;
		std::getline(inputStream, line);
		int elementIndex = -1;

		if (line != "ply")
			throw std::runtime_error("Not a PLY file");

		while (std::getline(inputStream, line))
		{
			std::string part;
			std::istringstream iss(line);
			iss >> part;

			if (part == "format")
			{
				iss >> part;

				if (part == "ascii")
					header.isAscii = true;
				else if (part == "binary_little_endian" || part == "binary_big_endian")
					header.isAscii = false;
				else
					throw std::runtime_error("Unknown PLY format");

				iss >> part;

				if (part != "1.0")
					throw std::runtime_error("Unknown PLY version");
			}
			else if (part == "element")
			{
				PlyElement element;
				iss >> element.name;
				iss >> element.count;
				header.elements.push_back(element);
				elementIndex++;
			}
			else if (part == "property")
			{
				PlyProperty property;
				iss >> part;
				property.dataType = convertTextToType(part);

				if (property.dataType == PlyType::NONE)
					throw std::runtime_error("Unknown PLY type");

				if (property.dataType == PlyType::LIST)
				{
					iss >> part;
					property.listLengthType = convertTextToType(part);

					if (property.listLengthType == PlyType::NONE)
						throw std::runtime_error("Unknown PLY type");

					iss >> part;
					property.listDataType = convertTextToType(part);

					if (property.listDataType == PlyType::NONE)
						throw std::runtime_error("Unknown PLY type");
				}

				iss >> property.name;
				header.elements.at(elementIndex).properties.push_back(property);
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

	void readAsciiData(std::ifstream& inputStream, const PlyHeader& header, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<std::vector<int>>& faces)
	{
		for (const PlyElement& element : header.elements)
		{
			if (element.name == "vertex")
			{
				for (int i = 0; i < element.count; ++i)
				{
					std::string line;
					std::getline(inputStream, line);
					std::istringstream iss(line);
					std::map<std::string, double> propertyValues;

					for (int j = 0; j < (int)element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// ignore and skip lists
						if (property.dataType == PlyType::LIST)
						{
							int listLength;
							iss >> listLength;
							double dummy;

							for (int k = 0; k < listLength; ++k)
								iss >> dummy;

							continue;
						}

						double propertyValue;
						iss >> propertyValue;
						propertyValues[property.name] = propertyValue;
					}

					vertices.push_back(Vector3(propertyValues["x"], propertyValues["y"], propertyValues["z"]));

					if (header.hasTexcoords)
						texcoords.push_back(Vector2(propertyValues["s"], propertyValues["t"]));

					if (header.hasNormals)
						normals.push_back(Vector3(propertyValues["nx"], propertyValues["ny"], propertyValues["nz"]));
				}
			}

			if (element.name == "face")
			{
				for (int i = 0; i < element.count; ++i)
				{
					std::string line;
					std::getline(inputStream, line);
					std::istringstream iss(line);

					for (int j = 0; j < (int)element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// only process lists named vertex_indices
						if (property.dataType == PlyType::LIST && property.name == "vertex_indices")
						{
							std::vector<int> indices;

							int listLength;
							iss >> listLength;

							for (int k = 0; k < listLength; ++k)
							{
								int index;
								iss >> index;
								indices.push_back(index);
							}

							faces.push_back(indices);
						}
						// ignore and skip other lists
						else if (property.dataType == PlyType::LIST)
						{
							int listLength;
							iss >> listLength;
							double dummy;

							for (int k = 0; k < listLength; ++k)
								iss >> dummy;
						}
						// ignore other properties
						else
						{
							double dummy;
							iss >> dummy;
						}
					}
				}
			}
		}
	}

	void readBinaryData(std::ifstream& inputStream, const PlyHeader& header, std::vector<Vector3>& vertices, std::vector<Vector2>& texcoords, std::vector<Vector3>& normals, std::vector<std::vector<int>>& faces)
	{
		for (const PlyElement& element : header.elements)
		{
			if (element.name == "vertex")
			{
				for (int i = 0; i < element.count; ++i)
				{
					std::map<std::string, double> propertyValues;

					for (int j = 0; j < (int)element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// ignore and skip lists
						if (property.dataType == PlyType::LIST)
						{
							int listLength = readBinaryValueAsInt(inputStream, property.listLengthType);

							for (int k = 0; k < listLength; ++k)
								readBinaryValueAsDouble(inputStream, property.listDataType);

							continue;
						}

						propertyValues[property.name] = readBinaryValueAsDouble(inputStream, property.dataType);
					}

					vertices.push_back(Vector3(propertyValues["x"], propertyValues["y"], propertyValues["z"]));

					if (header.hasTexcoords)
						texcoords.push_back(Vector2(propertyValues["s"], propertyValues["t"]));

					if (header.hasNormals)
						normals.push_back(Vector3(propertyValues["nx"], propertyValues["ny"], propertyValues["nz"]));
				}
			}

			if (element.name == "face")
			{
				for (int i = 0; i < element.count; ++i)
				{
					for (int j = 0; j < (int)element.properties.size(); ++j)
					{
						const PlyProperty& property = element.properties.at(j);

						// only process lists named vertex_indices
						if (property.dataType == PlyType::LIST && property.name == "vertex_indices")
						{
							std::vector<int> indices;
							int listLength = readBinaryValueAsInt(inputStream, property.listLengthType);

							for (int k = 0; k < listLength; ++k)
								indices.push_back(readBinaryValueAsInt(inputStream, property.listDataType));

							faces.push_back(indices);
						}
						// ignore and skip other lists
						else if (property.dataType == PlyType::LIST)
						{
							int listLength = readBinaryValueAsInt(inputStream, property.listLengthType);

							for (int k = 0; k < listLength; ++k)
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

	std::vector<Triangle> unpackAndTriangulate(const PlyHeader& header, const std::vector<Vector3>& vertices, const std::vector<Vector2>& texcoords, const std::vector<Vector3>& normals, const std::vector<std::vector<int>>& faces)
	{
		std::vector<Triangle> triangles;

		for (const std::vector<int>& face : faces)
		{
			if (face.size() < 3)
				throw std::runtime_error("Too few vertices in a face");

			// triangulate
			for (int i = 2; i < (int)face.size(); ++i)
			{
				Triangle triangle;

				triangle.vertices[0] = vertices[face[0]];
				triangle.vertices[1] = vertices[face[i - 1]];
				triangle.vertices[2] = vertices[face[i]];

				if (header.hasTexcoords)
				{
					triangle.texcoords[0] = texcoords[face[0]];
					triangle.texcoords[1] = texcoords[face[i - 1]];
					triangle.texcoords[2] = texcoords[face[i]];
				}

				// calculate triangle normal CCW
				Vector3 v0tov1 = triangle.vertices[1] - triangle.vertices[0];
				Vector3 v0tov2 = triangle.vertices[2] - triangle.vertices[0];
				Vector3 normal = v0tov1.cross(v0tov2).normalized();

				triangle.normal = normal;

				if (header.hasNormals)
				{
					triangle.normals[0] = normals[face[0]];
					triangle.normals[1] = normals[face[i - 1]];
					triangle.normals[2] = normals[face[i]];
				}
				else
					triangle.normals[0] = triangle.normals[1] = triangle.normals[2] = normal;

				triangles.push_back(triangle);
			}
		}

		return triangles;
	}
}

std::vector<Triangle> PlyReader::readFile(const std::string& fileName)
{
	Log& log = App::getLog();

	log.logInfo("Parsing PLY file %s", fileName);

	std::ifstream inputStream(fileName, std::ios::in | std::ios::binary);

	if (!inputStream.good())
		throw std::runtime_error("Could not open the PLY file");

	PlyHeader header;
	readHeader(inputStream, header);
	checkHeader(header);

	std::vector<Vector3> vertices;
	std::vector<Vector2> texcoords;
	std::vector<Vector3> normals;
	std::vector<std::vector<int>> faces;

	if (header.isAscii)
		readAsciiData(inputStream, header, vertices, texcoords, normals, faces);
	else
		readBinaryData(inputStream, header, vertices, texcoords, normals, faces);

	return unpackAndTriangulate(header, vertices, texcoords, normals, faces);
}
