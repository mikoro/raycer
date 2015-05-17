// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <algorithm>

#include "Raytracing/Raytracer.h"
#include "Raytracing/RaytracerState.h"
#include "Raytracing/Scene.h"
#include "Raytracing/Ray.h"
#include "Raytracing/Pixel.h"
#include "Raytracing/Intersection.h"
#include "Raytracing/Material.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

Raytracer::Raytracer()
{
	std::random_device rd;
	mt.seed(rd());
	random = std::uniform_real_distribution<double>(0.0, 1.0);
}

void Raytracer::run(RaytracerState& state, std::atomic<bool>& interrupted)
{
	Image& image = *state.image;
	Scene& scene = *state.scene;

	int rayCount = 0;
	int previousRayCount = 0;

	#pragma omp parallel for schedule(dynamic, 1000) reduction(+:rayCount) firstprivate(previousRayCount)
	for (int pixelIndex = 0; pixelIndex < state.pixelCount; ++pixelIndex)
	{
		if (interrupted)
			continue;

		int pixelOffsetIndex = pixelIndex + state.pixelOffset;
		double x = (double)(pixelOffsetIndex % state.sceneWidth);
		double y = (double)(pixelOffsetIndex / state.sceneWidth);

		Pixel pixel = shootRays(scene, x, y, rayCount, interrupted);
		
		if (scene.fog.enabled)
			pixel.color = scene.fog.calculate(scene, pixel);

		image.setPixel(pixelIndex, pixel.color);

		// progress reporting to another thread
		if ((pixelIndex + 1) % 100 == 0)
		{
			state.pixelsProcessed += 100;
			state.raysProcessed += (rayCount - previousRayCount);
			previousRayCount = rayCount;
		}
	}

	state.raysProcessed = rayCount;

	if (!interrupted)
		state.pixelsProcessed = state.pixelCount;
}

Pixel Raytracer::shootRays(Scene& scene, double x, double y, int& rayCount, std::atomic<bool>& interrupted)
{
	Pixel pixel;

	if (scene.multisampler.type == MultisampleType::NONE)
	{
		double rx = x + 0.5;
		double ry = y + 0.5;

		Ray ray = scene.camera.getRay(rx, ry);
		traceRay(scene, ray, rayCount, interrupted);

		if (ray.intersection.wasFound)
		{
			pixel.color = ray.color;
			pixel.position = ray.intersection.position;
			pixel.distance = ray.intersection.distance;
		}
		
		return pixel;
	}
	
	int multisamples = scene.multisampler.multisamples;
	int multisampleCount = 0;
	
	if (scene.multisampler.type == MultisampleType::UNIFORM || scene.multisampler.type == MultisampleType::REGULAR || scene.multisampler.type == MultisampleType::JITTER)
	{
		for (int i = 0; i < multisamples; ++i)
		{
			for (int j = 0; j < multisamples; ++j)
			{
				double rx = 0.0;
				double ry = 0.0;

				if (scene.multisampler.type == MultisampleType::UNIFORM)
				{
					rx = x + random(mt);
					ry = y + random(mt);
				}
				else if (scene.multisampler.type == MultisampleType::REGULAR)
				{
					rx = x + ((double)j + 0.5) / (double)multisamples;
					ry = y + ((double)i + 0.5) / (double)multisamples;
				}
				else if (scene.multisampler.type == MultisampleType::JITTER)
				{
					rx = x + ((double)j + random(mt)) / (double)multisamples;
					ry = y + ((double)i + random(mt)) / (double)multisamples;
				}

				Ray ray = scene.camera.getRay(rx, ry);
				traceRay(scene, ray, rayCount, interrupted);

				if (ray.intersection.wasFound)
				{
					pixel.color += ray.color;
					pixel.position += ray.intersection.position;
					pixel.distance += ray.intersection.distance;
					multisampleCount++;
				}
			}
		}
	}
	else if (scene.multisampler.type == MultisampleType::POISSON)
	{
	}

	pixel.color /= (double)multisampleCount;
	pixel.position /= (double)multisampleCount;
	pixel.distance /= (double)multisampleCount;

	return pixel;
}

/*

D = ray direction (unit)
N = intersection surface normal (unit)
R = reflected ray direction (unit)
T = transmitted (refracted) ray direction (unit)
P = intersection position
n1 = from refractive index
n2 = to refractive index
n = relative refractive index
c1 = ray-surface cosine, > 0: ray comes from outside,  < 0: ray comes from inside
c2 = < 0: total internal reflection, no transmitted ray
rf = fresnel reflectance factor
tf = fresnel transmittance factor
rf0 = fresnel reflectance at normal incidence
d = distance traveled inside

*/

void Raytracer::traceRay(Scene& scene, Ray& ray, int& rayCount, std::atomic<bool>& interrupted)
{
	if (interrupted)
		return;

	++rayCount;

	for (const Primitive* primitive : scene.primitivesList)
		primitive->intersect(ray);

	if (!ray.intersection.wasFound)
		return;

	Material* material = scene.materialsMap[ray.intersection.materialId];
	Texture* texture = scene.texturesMap[material->textureId];

	Color textureColor = texture->getColor(ray.intersection.position, ray.intersection.texcoord) * texture->intensity;

	if (material->isStatic)
	{
		ray.color = textureColor;
		return;
	}

	Vector3& D = ray.direction;
	Vector3& N = ray.intersection.normal;
	Vector3& P = ray.intersection.position;

	Color reflectionColor;
	Color refractionColor;

	double c1 = -D.dot(N);
	bool isFromOutside = (c1 >= 0.0);
	double n1 = isFromOutside ? 1.0 : material->refractiveIndex;
	double n2 = isFromOutside ? material->refractiveIndex : 1.0;
	double rf = 1.0;
	double tf = 1.0;

	if (material->isFresnel)
	{
		double rf0 = (n2 - n1) / (n2 + n1);
		rf0 = rf0 * rf0;
		rf = rf0 + (1.0 - rf0) * pow(1.0 - fabs(c1), 5.0);
		tf = 1.0 - rf;
	}

	double reflectance = material->reflectance * rf;
	double transmittance = material->transmittance * tf;

	// calculate and trace reflected ray
	if (reflectance > 0.0 && ray.iterations < scene.tracer.maxIterations)
	{
		Vector3 R = D + 2.0 * c1 * N;
		R.normalize();

		Ray reflectedRay;
		reflectedRay.origin = P + R * scene.tracer.rayStartOffset;
		reflectedRay.direction = R;
		reflectedRay.iterations = ray.iterations + 1;

		traceRay(scene, reflectedRay, rayCount, interrupted);

		if (ray.intersection.wasFound)
			reflectionColor = reflectedRay.color * reflectance;
	}

	// calculate and trace refracted ray
	if (transmittance > 0.0 && ray.iterations < scene.tracer.maxIterations)
	{
		double n = n1 / n2;
		double c2 = 1.0 - (n * n) * (1.0 - c1 * c1);

		// no total internal reflection
		if (c2 > 0.0)
		{
			Vector3 T = D * n + (c1 * n - sqrt(c2)) * N;
			T.normalize();

			Ray refractedRay;
			refractedRay.origin = P + T * scene.tracer.rayStartOffset;
			refractedRay.direction = T;
			refractedRay.iterations = ray.iterations + 1;

			traceRay(scene, refractedRay, rayCount, interrupted);

			if (ray.intersection.wasFound)
				refractionColor = refractedRay.color * transmittance;
		}
	}

	Color lightColor = calculateLighting(scene, ray, interrupted);
	ray.color = (reflectionColor + refractionColor + lightColor) * textureColor;
}

/*

P = intersection position
N = intersection surface normal (unit)
V = direction to viewer (unit)
L = direction to light (unit)
R = reflected light direction (unit)
d1 = diffuse light relative amount, only when > 0
d2 = specular light relative amount, only when > 0 and d1 > 0

*/

Color Raytracer::calculateLighting(Scene& scene, Ray& ray, std::atomic<bool>& interrupted)
{
	(void)interrupted;

	Material* material = scene.materialsMap[ray.intersection.materialId];

	Color lightColor;

	Vector3& P = ray.intersection.position;
	Vector3& N = ray.intersection.normal;
	Vector3 V = -ray.direction;

	for (const AmbientLight& ambientLight : scene.lights.ambientLights)
		lightColor += ambientLight.color * ambientLight.intensity * material->ambientness;

	for (const DirectionalLight& light : scene.lights.directionalLights)
	{
		Vector3 L = -light.direction;

		Ray shadowRay;
		shadowRay.origin = P + L * scene.tracer.rayStartOffset;
		shadowRay.direction = L;

		for (const Primitive* primitive : scene.primitivesList)
		{
			if (!primitive->nonShadowing)
				primitive->intersect(shadowRay);
		}

		if (shadowRay.intersection.wasFound)
			continue;

		// diffuse amount
		double d1 = L.dot(N);

		if (d1 <= 0.0)
			continue;

		lightColor += light.color * light.intensity * d1 * material->diffuseness;

		if (material->specularity == 0.0)
			continue;

		// reflected light direction
		Vector3 R = (2.0 * d1 * N) - L;
		R.normalize();

		// specular amount
		double d2 = R.dot(V);

		if (d2 > 0.0)
			lightColor += light.color * light.intensity * pow(d2, material->shininess) * material->specularity;
	}

	return lightColor;
}
