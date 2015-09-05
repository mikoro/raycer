// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>
#include <stdexcept>

#include "tinyformat/tinyformat.h"

#include "OpenCL/CLScene.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

namespace
{
	void readVector2(cl_float2& destination, const Vector2& source)
	{
		destination.s[0] = (cl_float)source.x;
		destination.s[1] = (cl_float)source.y;
	}

	void readVector3(cl_float4& destination, const Vector3& source)
	{
		destination.s[0] = (cl_float)source.x;
		destination.s[1] = (cl_float)source.y;
		destination.s[2] = (cl_float)source.z;
		destination.s[3] = 0.0f;
	}

	void readColor(cl_float4& destination, const Color& source)
	{
		destination.s[0] = (cl_float)source.r;
		destination.s[1] = (cl_float)source.g;
		destination.s[2] = (cl_float)source.b;
		destination.s[3] = (cl_float)source.a;
	}

	void readEulerAngle(cl_float4& destination, const EulerAngle& source)
	{
		destination.s[0] = (cl_float)source.pitch;
		destination.s[1] = (cl_float)source.yaw;
		destination.s[2] = (cl_float)source.roll;
		destination.s[3] = 0.0f;
	}
}

void CLScene::readScene(const Scene& scene)
{
	readVector3(camera.position, scene.camera.cameraState.position);
	readVector3(camera.forward, scene.camera.cameraState.forward);
	readVector3(camera.right, scene.camera.cameraState.right);
	readVector3(camera.up, scene.camera.cameraState.up);
	readVector3(camera.imagePlaneCenter, scene.camera.cameraState.imagePlaneCenter);
	readVector3(camera.translateInTime, scene.camera.translateInTime);
	readEulerAngle(camera.rotateInTime, scene.camera.rotateInTime);
	camera.projectionType = (cl_int)scene.camera.projectionType;
	camera.isTimeVariant = (cl_int)scene.camera.isTimeVariant;
	camera.hasMoved = (cl_int)scene.camera.cameraHasMoved;
	camera.fov = (cl_float)scene.camera.fov;
	camera.orthoSize = (cl_float)scene.camera.orthoSize;
	camera.fishEyeAngle = (cl_float)scene.camera.fishEyeAngle;
	camera.apertureSize = (cl_float)scene.camera.apertureSize;
	camera.focalDistance = (cl_float)scene.camera.focalDistance;
	camera.aspectRatio = (cl_float)scene.camera.aspectRatio;
	camera.imagePlaneWidth = (cl_float)scene.camera.imagePlaneWidth;
	camera.imagePlaneHeight = (cl_float)scene.camera.imagePlaneHeight;
	camera.imagePlaneDistance = (cl_float)scene.camera.imagePlaneDistance;

	readColor(raytracer.backgroundColor, scene.raytracer.backgroundColor);
	readColor(raytracer.offLensColor, scene.raytracer.offLensColor);
	raytracer.maxRayIterations = (cl_int)scene.raytracer.maxRayIterations;
	raytracer.rayStartOffset = (cl_float)scene.raytracer.rayStartOffset;
	raytracer.multiSamples = (cl_int)scene.raytracer.multiSamples;
	raytracer.timeSamples = (cl_int)scene.raytracer.timeSamples;
	raytracer.cameraSamples = (cl_int)scene.raytracer.cameraSamples;

	toneMapper.type = (cl_int)scene.toneMapper.type;
	toneMapper.applyGamma = (cl_int)scene.toneMapper.applyGamma;
	toneMapper.shouldClamp = (cl_int)scene.toneMapper.shouldClamp;
	toneMapper.gamma = (cl_float)scene.toneMapper.gamma;
	toneMapper.exposure = (cl_float)scene.toneMapper.exposure;
	toneMapper.key = (cl_float)scene.toneMapper.key;
	toneMapper.maxLuminance = (cl_float)scene.toneMapper.maxLuminance;

	readColor(simpleFog.color, scene.simpleFog.color);
	simpleFog.enabled = (cl_int)scene.simpleFog.enabled;
	simpleFog.distance = (cl_float)scene.simpleFog.distance;
	simpleFog.steepness = (cl_float)scene.simpleFog.steepness;
	simpleFog.heightDispersion = (cl_int)scene.simpleFog.heightDispersion;
	simpleFog.height = (cl_float)scene.simpleFog.height;
	simpleFog.heightSteepness = (cl_float)scene.simpleFog.heightSteepness;

	readColor(ambientLight.color, scene.lights.ambientLight.color);
	ambientLight.intensity = (cl_float)scene.lights.ambientLight.intensity;
	ambientLight.enableOcclusion = (cl_int)scene.lights.ambientLight.enableOcclusion;
	ambientLight.maxDistance = (cl_float)scene.lights.ambientLight.maxDistance;
	ambientLight.samplerType = (cl_int)scene.lights.ambientLight.samplerType;
	ambientLight.samples = (cl_int)scene.lights.ambientLight.samples;
	ambientLight.distribution = (cl_float)scene.lights.ambientLight.distribution;

	materials.clear();
	directionalLights.clear();
	pointLights.clear();
	spotLights.clear();
	planes.clear();
	spheres.clear();
	boxes.clear();
	triangles.clear();

	for (const Material& material : scene.materials)
	{
		OpenCL::Material clMaterial;

		ColorTexture* colorTexture = dynamic_cast<ColorTexture*>(material.colorTexture);

		if (colorTexture != nullptr)
			readColor(clMaterial.color, colorTexture->color);
		else
			readColor(clMaterial.color, Color::WHITE);

		readColor(clMaterial.ambientReflectance, material.ambientReflectance);
		readColor(clMaterial.diffuseReflectance, material.diffuseReflectance);
		readColor(clMaterial.specularReflectance, material.specularReflectance);
		readColor(clMaterial.attenuationColor, material.attenuationColor);
		readVector2(clMaterial.texcoordScale, material.texcoordScale);
		clMaterial.colorIntensity = (cl_float)material.colorTexture->intensity;
		clMaterial.shininess = (cl_float)material.shininess;
		clMaterial.skipLighting = (cl_int)material.skipLighting;
		clMaterial.nonShadowing = (cl_int)material.nonShadowing;
		clMaterial.normalInterpolation = (cl_int)material.normalInterpolation;
		clMaterial.backfaceCulling = (cl_int)material.backfaceCulling;
		clMaterial.invertNormal = (cl_int)material.invertNormal;
		clMaterial.hasTexture = (cl_int)0;
		clMaterial.textureIndex = (cl_int)0;
		clMaterial.rayReflectance = (cl_float)material.rayReflectance;
		clMaterial.rayTransmittance = (cl_float)material.rayTransmittance;
		clMaterial.refractiveIndex = (cl_float)material.refractiveIndex;
		clMaterial.isFresnel = (cl_int)material.isFresnel;
		clMaterial.enableAttenuation = (cl_int)material.enableAttenuation;
		clMaterial.attenuation = (cl_float)material.attenuation;
		clMaterial.id = (cl_int)material.id;

		materials.push_back(clMaterial);
	}

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		OpenCL::DirectionalLight clLight;

		readColor(clLight.color, light.color);
		readVector3(clLight.direction, light.direction);
		clLight.intensity = (cl_float)light.intensity;

		directionalLights.push_back(clLight);
	}

	for (const PointLight& light : scene.lights.pointLights)
	{
		OpenCL::PointLight clLight;

		readColor(clLight.color, light.color);
		readVector3(clLight.position, light.position);
		clLight.intensity = (cl_float)light.intensity;
		clLight.distance = (cl_float)light.distance;
		clLight.attenuation = (cl_float)light.attenuation;
		clLight.softShadows = (cl_int)light.softShadows;
		clLight.radius = (cl_float)light.radius;
		clLight.samplerType = (cl_int)light.samplerType;
		clLight.samples = (cl_int)light.samples;

		pointLights.push_back(clLight);
	}

	for (const SpotLight& light : scene.lights.spotLights)
	{
		OpenCL::SpotLight clLight;

		readColor(clLight.color, light.color);
		readVector3(clLight.position, light.position);
		readVector3(clLight.direction, light.direction);
		clLight.intensity = (cl_float)light.intensity;
		clLight.distance = (cl_float)light.distance;
		clLight.attenuation = (cl_float)light.attenuation;
		clLight.sideAttenuation = (cl_float)light.sideAttenuation;
		clLight.angle = (cl_float)light.angle;
		clLight.softShadows = (cl_int)light.softShadows;
		clLight.radius = (cl_float)light.radius;
		clLight.samplerType = (cl_int)light.samplerType;
		clLight.samples = (cl_int)light.samples;

		spotLights.push_back(clLight);
	}

	for (const Plane& plane : scene.primitives.planes)
	{
		OpenCL::Plane clPlane;

		readVector3(clPlane.position, plane.position);
		readVector3(clPlane.normal, plane.normal);
		readVector3(clPlane.uAxis, plane.uAxis);
		readVector3(clPlane.vAxis, plane.vAxis);
		clPlane.invisible = (cl_int)plane.invisible;
		clPlane.materialIndex = (cl_int)findMaterialIndex(plane.materialId);

		planes.push_back(clPlane);
	}

	for (const Sphere& sphere : scene.primitives.spheres)
	{
		OpenCL::Sphere clSphere;

		readVector3(clSphere.position, sphere.position);
		clSphere.radius = (cl_float)sphere.radius;
		clSphere.invisible = (cl_int)sphere.invisible;
		clSphere.materialIndex = (cl_int)findMaterialIndex(sphere.materialId);

		spheres.push_back(clSphere);
	}

	for (const Box& box : scene.primitives.boxes)
	{
		OpenCL::Box clBox;

		readVector3(clBox.position, box.position);
		readVector3(clBox.extent, box.extent);
		clBox.invisible = (cl_int)box.invisible;
		clBox.materialIndex = (cl_int)findMaterialIndex(box.materialId);

		boxes.push_back(clBox);
	}

	for (const Triangle& triangle : scene.primitives.triangles)
	{
		OpenCL::Triangle clTriangle;

		readVector3(clTriangle.vertices[0], triangle.vertices[0]);
		readVector3(clTriangle.vertices[1], triangle.vertices[1]);
		readVector3(clTriangle.vertices[2], triangle.vertices[2]);
		readVector3(clTriangle.normals[0], triangle.normals[0]);
		readVector3(clTriangle.normals[1], triangle.normals[1]);
		readVector3(clTriangle.normals[2], triangle.normals[2]);
		readVector2(clTriangle.texcoords[0], triangle.texcoords[0]);
		readVector2(clTriangle.texcoords[1], triangle.texcoords[1]);
		readVector2(clTriangle.texcoords[2], triangle.texcoords[2]);
		readVector3(clTriangle.normal, triangle.normal);
		readVector3(clTriangle.tangent, triangle.tangent);
		readVector3(clTriangle.bitangent, triangle.bitangent);
		clTriangle.invisible = (cl_int)triangle.invisible;
		clTriangle.materialIndex = (cl_int)findMaterialIndex(triangle.materialId);

		triangles.push_back(clTriangle);
	}

	state.directionalLightCount = (cl_int)directionalLights.size();
	state.pointLightCount = (cl_int)pointLights.size();
	state.spotLightCount = (cl_int)spotLights.size();
	state.planeCount = (cl_int)planes.size();
	state.sphereCount = (cl_int)spheres.size();
	state.boxCount = (cl_int)boxes.size();
	state.triangleCount = (cl_int)triangles.size();
}

int CLScene::findMaterialIndex(int materialId)
{
	auto result = std::find_if(materials.begin(), materials.end(), [materialId](const OpenCL::Material& material) { return material.id == materialId; });

	if (result == materials.end())
		throw std::runtime_error(tfm::format("Could not find OpenCL material index for material id (%d)", materialId));

	return (int)(result - materials.begin());
}
