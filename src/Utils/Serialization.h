// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Raytracing/Scene.h"

#include "Math/AxisAngle.h"
#include "Math/Color.h"
#include "Math/EulerAngle.h"
#include "Math/Matrix4x4.h"
#include "Math/MovingAverage.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace Raycer
{
	// ---------------------------------------------
	// SCENE
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, Scene& b)
	{
		a(cereal::make_nvp("camera", b.camera),
			cereal::make_nvp("misc", b.misc),
			cereal::make_nvp("fog", b.fog),
			cereal::make_nvp("raytracing", b.raytracing),
			cereal::make_nvp("multisampling", b.multisampling),
			cereal::make_nvp("toneMapping", b.toneMapping),
			cereal::make_nvp("rootBVH", b.rootBVH),
			cereal::make_nvp("textures", b.textures),
			cereal::make_nvp("materials", b.materials),
			cereal::make_nvp("lights", b.lights),
			cereal::make_nvp("primitives", b.primitives));
	}

	template<class Archive>
	void serialize(Archive& a, Camera& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("orientation", b.orientation),
			cereal::make_nvp("projectionType", b.projectionType),
			cereal::make_nvp("fov", b.fov),
			cereal::make_nvp("orthoSize", b.orthoSize),
			cereal::make_nvp("fishEyeAngle", b.fishEyeAngle),
			cereal::make_nvp("dofSamples", b.dofSamples),
			cereal::make_nvp("timeSamples", b.timeSamples),
			cereal::make_nvp("apertureSize", b.apertureSize),
			cereal::make_nvp("focalLenght", b.focalLenght));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Misc& b)
	{
		a(cereal::make_nvp("backgroundColor", b.backgroundColor));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Fog& b)
	{
		a(cereal::make_nvp("enabled", b.enabled),
			cereal::make_nvp("color", b.color),
			cereal::make_nvp("distance", b.distance),
			cereal::make_nvp("steepness", b.steepness),
			cereal::make_nvp("heightDispersion", b.heightDispersion),
			cereal::make_nvp("height", b.height),
			cereal::make_nvp("heightSteepness", b.heightSteepness));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Raytracing& b)
	{
		a(cereal::make_nvp("maxIterations", b.maxIterations),
			cereal::make_nvp("startOffset", b.startOffset));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Multisampling& b)
	{
		a(cereal::make_nvp("enabled", b.enabled),
			cereal::make_nvp("type", b.type),
			cereal::make_nvp("samples", b.samples));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::ToneMapping& b)
	{
		a(cereal::make_nvp("enabled", b.enabled),
			cereal::make_nvp("type", b.type),
			cereal::make_nvp("gamma", b.gamma));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::RootBVH& b)
	{
		a(cereal::make_nvp("enabled", b.enabled),
			cereal::make_nvp("buildInfo", b.buildInfo));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Textures& b)
	{
		a(cereal::make_nvp("colorTextures", b.colorTextures),
			cereal::make_nvp("checkerTextures", b.checkerTextures),
			cereal::make_nvp("imageTextures", b.imageTextures),
			cereal::make_nvp("perlinNoiseTextures", b.perlinNoiseTextures),
			cereal::make_nvp("valueNoiseTextures", b.valueNoiseTextures),
			cereal::make_nvp("cellNoiseTextures", b.cellNoiseTextures),
			cereal::make_nvp("marbleTextures", b.marbleTextures),
			cereal::make_nvp("woodTextures", b.woodTextures),
			cereal::make_nvp("fireTextures", b.fireTextures),
			cereal::make_nvp("atmosphereTextures", b.atmosphereTextures),
			cereal::make_nvp("voronoiTextures", b.voronoiTextures));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Lights& b)
	{
		a(cereal::make_nvp("ambientLight", b.ambientLight),
			cereal::make_nvp("directionalLights", b.directionalLights),
			cereal::make_nvp("pointLights", b.pointLights),
			cereal::make_nvp("spotLights", b.spotLights));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Primitives& b)
	{
		a(cereal::make_nvp("planes", b.planes),
			cereal::make_nvp("spheres", b.spheres),
			cereal::make_nvp("boxes", b.boxes),
			cereal::make_nvp("triangles", b.triangles),
			cereal::make_nvp("meshes", b.meshes));
	}

	// ---------------------------------------------
	// LIGHTS
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, AmbientLight& b)
	{
		a(cereal::make_nvp("color", b.color),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("ambientOcclusion", b.ambientOcclusion),
			cereal::make_nvp("samples", b.samples),
			cereal::make_nvp("distribution", b.distribution));
	}

	template<class Archive>
	void serialize(Archive& a, DirectionalLight& b)
	{
		a(cereal::make_nvp("color", b.color),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("direction", b.direction));
	}

	template<class Archive>
	void serialize(Archive& a, PointLight& b)
	{
		a(cereal::make_nvp("color", b.color),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("position", b.position),
			cereal::make_nvp("distance", b.distance),
			cereal::make_nvp("attenuation", b.attenuation));
	}

	template<class Archive>
	void serialize(Archive& a, SpotLight& b)
	{
		a(cereal::make_nvp("color", b.color),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("position", b.position),
			cereal::make_nvp("direction", b.direction),
			cereal::make_nvp("distance", b.distance),
			cereal::make_nvp("distanceAttenuation", b.distanceAttenuation),
			cereal::make_nvp("sideAttenuation", b.sideAttenuation),
			cereal::make_nvp("angle", b.angle));
	}

	// ---------------------------------------------
	// PRIMITIVES
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, Plane& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("normal", b.normal),
			cereal::make_nvp("materialId", b.materialId));
	}

	template<class Archive>
	void serialize(Archive& a, Sphere& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("radius", b.radius),
			cereal::make_nvp("materialId", b.materialId));
	}

	template<class Archive>
	void serialize(Archive& a, Box& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("extent", b.extent),
			cereal::make_nvp("materialId", b.materialId));
	}

	template<class Archive>
	void serialize(Archive& a, Triangle& b)
	{
		a(cereal::make_nvp("vertices", b.vertices),
			cereal::make_nvp("normals", b.normals),
			cereal::make_nvp("texcoords", b.texcoords),
			cereal::make_nvp("materialId", b.materialId));
	}

	template<class Archive>
	void serialize(Archive& a, Mesh& b)
	{
		a(cereal::make_nvp("meshFilePath", b.meshFilePath),
			cereal::make_nvp("position", b.position),
			cereal::make_nvp("scale", b.scale),
			cereal::make_nvp("orientation", b.orientation),
			cereal::make_nvp("materialId", b.materialId),
			cereal::make_nvp("enableBVH", b.enableBVH),
			cereal::make_nvp("bvhBuildInfo", b.bvhBuildInfo));
	}
	
	// ---------------------------------------------
	// TEXTURES
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, ColorTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("color", b.color));
	}

	template<class Archive>
	void serialize(Archive& a, CheckerTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("color1", b.color1),
			cereal::make_nvp("color2", b.color2));
	}

	template<class Archive>
	void serialize(Archive& a, ImageTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("imageFilePath", b.imageFilePath));
	}

	template<class Archive>
	void serialize(Archive& a, PerlinNoiseTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("scale", b.scale),
			cereal::make_nvp("baseColor", b.baseColor),
			cereal::make_nvp("isFbm", b.isFbm),
			cereal::make_nvp("octaves", b.octaves),
			cereal::make_nvp("lacunarity", b.lacunarity),
			cereal::make_nvp("persistence", b.persistence));
	}

	template<class Archive>
	void serialize(Archive& a, ValueNoiseTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("scale", b.scale),
			cereal::make_nvp("baseColor", b.baseColor),
			cereal::make_nvp("isFbm", b.isFbm),
			cereal::make_nvp("octaves", b.octaves),
			cereal::make_nvp("lacunarity", b.lacunarity),
			cereal::make_nvp("persistence", b.persistence));
	}

	template<class Archive>
	void serialize(Archive& a, CellNoiseTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("scale", b.scale),
			cereal::make_nvp("baseColor", b.baseColor),
			cereal::make_nvp("distanceType", b.distanceType),
			cereal::make_nvp("combineType", b.combineType),
			cereal::make_nvp("density", b.density));
	}

	template<class Archive>
	void serialize(Archive& a, MarbleTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("marbleColor", b.marbleColor),
			cereal::make_nvp("streakColor", b.streakColor),
			cereal::make_nvp("density", b.density),
			cereal::make_nvp("swirliness", b.swirliness),
			cereal::make_nvp("transparency", b.transparency));
	}

	template<class Archive>
	void serialize(Archive& a, WoodTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("woodColor", b.woodColor),
			cereal::make_nvp("density", b.density),
			cereal::make_nvp("bumpiness", b.bumpiness));
	}

	template<class Archive>
	void serialize(Archive& a, FireTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed));
	}

	template<class Archive>
	void serialize(Archive& a, AtmosphereTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("topColor1", b.topColor1),
			cereal::make_nvp("topColor2", b.topColor2),
			cereal::make_nvp("middleColor", b.middleColor),
			cereal::make_nvp("horizonColor", b.horizonColor),
			cereal::make_nvp("cloudColor", b.cloudColor),
			cereal::make_nvp("sunColor", b.sunColor),
			cereal::make_nvp("sunOrientation", b.sunOrientation),
			cereal::make_nvp("sunSize", b.sunSize),
			cereal::make_nvp("hasClouds", b.hasClouds),
			cereal::make_nvp("hasSun", b.hasSun));
	}

	template<class Archive>
	void serialize(Archive& a, VoronoiTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("seed", b.seed),
			cereal::make_nvp("density", b.density),
			cereal::make_nvp("scale", b.scale),
			cereal::make_nvp("distanceType", b.distanceType),
			cereal::make_nvp("useRandomColors", b.useRandomColors),
			cereal::make_nvp("randomColorCount", b.randomColorCount),
			cereal::make_nvp("colors", b.colors));
	}

	// ---------------------------------------------
	// MATH
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, AxisAngle& b)
	{
		a(cereal::make_nvp("axis", b.axis),
			cereal::make_nvp("angle", b.angle));
	}

	template<class Archive>
	void serialize(Archive& a, Color& b)
	{
		a(cereal::make_nvp("r", b.r),
			cereal::make_nvp("g", b.g),
			cereal::make_nvp("b", b.b),
			cereal::make_nvp("a", b.a));
	}

	template<class Archive>
	void serialize(Archive& a, EulerAngle& b)
	{
		a(cereal::make_nvp("yaw", b.yaw),
			cereal::make_nvp("pitch", b.pitch),
			cereal::make_nvp("roll", b.roll));
	}

	template<class Archive>
	void serialize(Archive& a, Matrix4x4& b)
	{
		a(cereal::make_nvp("m", b.m));
	}

	template<class Archive>
	void serialize(Archive& a, Quaternion& b)
	{
		a(cereal::make_nvp("w", b.w),
			cereal::make_nvp("x", b.x),
			cereal::make_nvp("y", b.y),
			cereal::make_nvp("z", b.z));
	}

	template<class Archive>
	void serialize(Archive& a, Vector2& b)
	{
		a(cereal::make_nvp("x", b.x),
			cereal::make_nvp("y", b.y));
	}

	template<class Archive>
	void serialize(Archive& a, Vector3& b)
	{
		a(cereal::make_nvp("x", b.x),
			cereal::make_nvp("y", b.y),
			cereal::make_nvp("z", b.z));
	}

	// ---------------------------------------------
	// MATERIAL
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, Material& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("textureId", b.textureId),
			cereal::make_nvp("texcoordScale", b.texcoordScale),
			cereal::make_nvp("skipLighting", b.skipLighting),
			cereal::make_nvp("nonShadowing", b.nonShadowing),
			cereal::make_nvp("ambientness", b.ambientness),
			cereal::make_nvp("diffuseness", b.diffuseness),
			cereal::make_nvp("specularity", b.specularity),
			cereal::make_nvp("shininess", b.shininess),
			cereal::make_nvp("fresnel", b.fresnel),
			cereal::make_nvp("reflectance", b.reflectance),
			cereal::make_nvp("transmittance", b.transmittance),
			cereal::make_nvp("refractiveIndex", b.refractiveIndex),
			cereal::make_nvp("attenuate", b.attenuate),
			cereal::make_nvp("attenuation", b.attenuation),
			cereal::make_nvp("attenuationColor", b.attenuationColor));
	}

	// ---------------------------------------------
	// BVHBUILDINFO
	// ---------------------------------------------

	template<class Archive>
	void serialize(Archive& a, BVHBuildInfo& b)
	{
		a(cereal::make_nvp("maxLeafSize", b.maxLeafSize),
			cereal::make_nvp("useSAH", b.useSAH),
			cereal::make_nvp("regularSAHSplits", b.regularSAHSplits),
			cereal::make_nvp("axisSelection", b.axisSelection),
			cereal::make_nvp("axisSplit", b.axisSplit));
	}
}
