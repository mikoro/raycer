// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <random>

#include "Raytracing/Tracers/Tracer.h"
#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Filters/Filter.h"
#include "Rendering/ToneMappers/ToneMapper.h"

namespace Raycer
{
	class Color;
	class Scene;
	class Vector2;
	class Vector3;
	struct TracerState;
	class Ray;
	struct Intersection;
	class Material;
	struct Light;
	struct DirectionalLight;
	struct PointLight;

	class WhittedRaytracer : public Tracer
	{
	public:

		WhittedRaytracer();

		void run(TracerState& state, std::atomic<bool>& interrupted) override;

	private:

		WhittedRaytracer(const WhittedRaytracer& tracer);
		WhittedRaytracer& operator=(const WhittedRaytracer& tracer);

		Color generateMultiSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateTimeSamples(const Scene& scene, const Vector2& pixelCoordinate, const std::atomic<bool>& interrupted);
		Color generateCameraSamples(const Scene& scene, const Vector2& pixelCoordinate, double time, const std::atomic<bool>& interrupted);

		Color traceRay(const Scene& scene, const Ray& ray, Intersection& intersection, uint iteration, const std::atomic<bool>& interrupted);

		Color calculateLightColor(const Scene& scene, const Ray& ray, const Intersection& intersection, double ambientOcclusionAmount);
		Color calculatePhongShadingColor(const Vector3& normal, const Vector3& directionToLight, const Vector3& directionToCamera, const Light& light, const Color& diffuseReflectance, const Color& specularReflectance, double shininess);
		Color calculateSimpleFogColor(const Scene& scene, const Intersection& intersection, const Color& pixelColor);

		double calculateAmbientOcclusionAmount(const Scene& scene, const Intersection& intersection);
		double calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const DirectionalLight& light);
		double calculateShadowAmount(const Scene& scene, const Ray& ray, const Intersection& intersection, const PointLight& light);

		std::map<SamplerType, std::unique_ptr<Sampler>> samplers;
		std::map<FilterType, std::unique_ptr<Filter>> filters;
		std::map<ToneMapperType, std::unique_ptr<ToneMapper>> toneMappers;

		std::mt19937 generator;
		std::uniform_int_distribution<uint> randomPermutation;
	};
}
