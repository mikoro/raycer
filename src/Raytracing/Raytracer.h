// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <random>

#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Filters/Filter.h"
#include "Rendering/ToneMappers/ToneMapper.h"

namespace Raycer
{
	class Color;
	class Scene;
	class Vector2;
	class Vector3;
	struct RaytracerState;
	class Ray;
	struct Intersection;
	struct Material;
	struct Light;
	struct DirectionalLight;
	struct PointLight;

	class Raytracer
	{
	public:

		Raytracer();

		void run(RaytracerState& state, std::atomic<bool>& interrupted);

	private:

		Raytracer(const Raytracer& raytracer);
		Raytracer& operator=(const Raytracer& raytracer);

		Color generateMultiSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateDofSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateTimeSamples(const Scene& scene, Ray& ray, const std::atomic<bool>& interrupted);

		Color raytrace(const Scene& scene, const Ray& ray, Intersection& intersection, int iteration, const std::atomic<bool>& interrupted);

		Color calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusionAmount);
		Color calculatePhongShadingColor(const Vector3& normal, const Vector3& directionToLight, const Vector3& directionToCamera, const Light& light, const Material* material);
		Color calculateSimpleFogColor(const Scene& scene, const Intersection& intersection, const Color& pixelColor);

		double calculateAmbientOcclusionAmount(const Scene& scene, const Intersection& intersection);
		double calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const DirectionalLight& light);
		double calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const PointLight& light);

		std::map<SamplerType, std::shared_ptr<Sampler>> samplers;
		std::map<FilterType, std::shared_ptr<Filter>> filters;
		std::map<ToneMapperType, std::shared_ptr<ToneMapper>> toneMappers;

		std::mt19937 generator;
		std::uniform_int_distribution<int> randomDist;
	};
}
