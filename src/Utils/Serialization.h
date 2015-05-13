// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "cereal/cereal.hpp"

#include "Math/AxisAngle.h"
#include "Math/Color.h"
#include "Math/EulerAngle.h"
#include "Math/Matrix4x4.h"
#include "Math/MovingAverage.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

#include "Raytracing/Camera.h"
#include "Raytracing/Fog.h"
#include "Raytracing/Lights.h"
#include "Raytracing/Material.h"
#include "Raytracing/Scene.h"

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/Primitives/Plane.h"
#include "Raytracing/Primitives/Sphere.h"
#include "Raytracing/Primitives/Triangle.h"
#include "Raytracing/Primitives/Mesh.h"

#include "Raytracing/Textures/Texture.h"
#include "Raytracing/Textures/ColorTexture.h"
#include "Raytracing/Textures/CheckerTexture.h"
#include "Raytracing/Textures/ImageTexture.h"
#include "Raytracing/Textures/PerlinNoiseTexture.h"
#include "Raytracing/Textures/CellNoiseTexture.h"
#include "Raytracing/Textures/MarbleTexture.h"
#include "Raytracing/Textures/WoodTexture.h"
#include "Raytracing/Textures/FireTexture.h"

namespace Raycer
{
	/* MATH */

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

	/* CAMERA */

	template<class Archive>
	void serialize(Archive& a, Camera& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("orientation", b.orientation),
			cereal::make_nvp("fov", b.fov));
	}

	/* FOG */

	template<class Archive>
	void serialize(Archive& a, Fog& b)
	{
		a(cereal::make_nvp("enabled", b.enabled),
			cereal::make_nvp("distance", b.distance),
			cereal::make_nvp("steepness", b.steepness),
			cereal::make_nvp("color", b.color));
	}

	/* LIGHTS */

	template<class Archive>
	void serialize(Archive& a, AmbientLight& b)
	{
		a(cereal::make_nvp("color", b.color),
			cereal::make_nvp("intensity", b.intensity));
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
			cereal::make_nvp("radius", b.radius));
	}

	template<class Archive>
	void serialize(Archive& a, SpotLight& b)
	{
		a(cereal::make_nvp("color", b.color),
			cereal::make_nvp("intensity", b.intensity),
			cereal::make_nvp("position", b.position),
			cereal::make_nvp("direction", b.direction),
			cereal::make_nvp("angle", b.angle),
			cereal::make_nvp("radius", b.radius));
	}

	/* MATERIAL */

	template<class Archive>
	void serialize(Archive& a, Material& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("textureId", b.textureId),
			cereal::make_nvp("diffuseness", b.diffuseness),
			cereal::make_nvp("specularity", b.specularity),
			cereal::make_nvp("shininess", b.shininess),
			cereal::make_nvp("reflectivity", b.reflectivity));
	}

	/* SCENE */

	template<class Archive>
	void serialize(Archive& a, Scene& b)
	{
		a(cereal::make_nvp("tracer", b.tracer),
			cereal::make_nvp("multisampler", b.multisampler),
			cereal::make_nvp("toneMapper", b.toneMapper),
			cereal::make_nvp("camera", b.camera),
			cereal::make_nvp("fog", b.fog),
			cereal::make_nvp("textures", b.textures),
			cereal::make_nvp("materials", b.materials),
			cereal::make_nvp("lights", b.lights),
			cereal::make_nvp("primitives", b.primitives));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Tracer& b)
	{
		a(cereal::make_nvp("maxIterations", b.maxIterations));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Multisampler& b)
	{
		a(cereal::make_nvp("type", b.type),
			cereal::make_nvp("multisamples", b.multisamples));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::ToneMapper& b)
	{
		a(cereal::make_nvp("type", b.type),
			cereal::make_nvp("gamma", b.gamma));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Textures& b)
	{
		a(cereal::make_nvp("colorTextures", b.colorTextures),
			cereal::make_nvp("checkerTextures", b.checkerTextures),
			cereal::make_nvp("imageTextures", b.imageTextures),
			cereal::make_nvp("perlinNoiseTextures", b.perlinNoiseTextures),
			cereal::make_nvp("cellNoiseTextures", b.cellNoiseTextures),
			cereal::make_nvp("marbleTextures", b.marbleTextures),
			cereal::make_nvp("woodTextures", b.woodTextures),
			cereal::make_nvp("fireTextures", b.fireTextures));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Lights& b)
	{
		a(cereal::make_nvp("ambientLights", b.ambientLights),
			cereal::make_nvp("directionalLights", b.directionalLights),
			cereal::make_nvp("pointLights", b.pointLights),
			cereal::make_nvp("spotLights", b.spotLights));
	}

	template<class Archive>
	void serialize(Archive& a, Scene::Primitives& b)
	{
		a(cereal::make_nvp("planes", b.planes),
			cereal::make_nvp("spheres", b.spheres),
			cereal::make_nvp("meshes", b.meshes));
	}

	/* PRIMITIVES */

	template<class Archive>
	void serialize(Archive& a, Plane& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("normal", b.normal),
			cereal::make_nvp("materialId", b.materialId),
			cereal::make_nvp("texcoordScale", b.texcoordScale));
	}

	template<class Archive>
	void serialize(Archive& a, Sphere& b)
	{
		a(cereal::make_nvp("position", b.position),
			cereal::make_nvp("radius", b.radius),
			cereal::make_nvp("materialId", b.materialId),
			cereal::make_nvp("texcoordScale", b.texcoordScale));
	}

	template<class Archive>
	void serialize(Archive& a, Triangle& b)
	{
		a(cereal::make_nvp("vertices", b.vertices),
			cereal::make_nvp("normals", b.normals),
			cereal::make_nvp("texcoords", b.texcoords));
	}

	template<class Archive>
	void serialize(Archive& a, Mesh& b)
	{
		a(cereal::make_nvp("meshFilePath", b.meshFilePath),
			cereal::make_nvp("position", b.position),
			cereal::make_nvp("orientation", b.orientation),
			cereal::make_nvp("materialId", b.materialId),
			cereal::make_nvp("texcoordScale", b.texcoordScale));
	}

	/* TEXTURES */

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
			cereal::make_nvp("intensity", b.intensity));
	}

	template<class Archive>
	void serialize(Archive& a, CellNoiseTexture& b)
	{
		a(cereal::make_nvp("id", b.id),
			cereal::make_nvp("intensity", b.intensity));
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
}
