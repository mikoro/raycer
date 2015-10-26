// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Film.h"
#include "Raytracing/Camera.h"
#include "Raytracing/Ray.h"

using namespace Raycer;

void Film::resize(uint64_t width_, uint64_t height_)
{
	width = width_;
	height = height_;

	filmPixels.resize(width * height);
	linearImage.resize(width, height);
	toneMappedImage.resize(width, height);
}

void Film::setSampler(SamplerType type, uint64_t sampleCountSqrt)
{
	for (auto& filmPixel : filmPixels)
	{
		filmPixel.sampler = Sampler::getSampler(type);
		filmPixel.sampler->generateSamples2D(sampleCountSqrt);
	}
}

void Film::setFilter(FilterType type)
{
	filter = Filter::getFilter(type);
}

void Film::setToneMapper(ToneMapperType type)
{
	toneMapper = ToneMapper::getToneMapper(type);
}

void Film::clear()
{
	for (auto& filmPixel : filmPixels)
	{
		filmPixel.cumulativeColor = Color(0.0, 0.0, 0.0, 0.0);
		filmPixel.filterWeightSum = 0.0;
	}
}

bool Film::getNextRay(uint64_t x, uint64_t y, const Camera& camera, Ray& ray, double time)
{
	FilmPixel& filmPixel = filmPixels[y * width + x];
	Vector2 sampleOffset;

	if (!filmPixel.sampler->getNextSample2D(sampleOffset))
		return false;

	Vector2 pixelCoordinate = Vector2(double(x), double(y));
	sampleOffset = (sampleOffset - Vector2(0.5, 0.5)) * 2.0 * filter->getRadius();

	camera.getRay(pixelCoordinate + sampleOffset, ray, time);
	ray.sampleOffset = sampleOffset;

	return true;
}

void Film::addSample(uint64_t x, uint64_t y, const Vector2& sampleOffset, const Color& color)
{
	FilmPixel& filmPixel = filmPixels[y * width + x];
	double filterWeight = filter->getWeight(sampleOffset);
	filmPixel.cumulativeColor += color * filterWeight;
	filmPixel.filterWeightSum += filterWeight;
}

void Film::generateOutputImage(const Scene& scene)
{
	for (uint64_t i = 0; i < filmPixels.size(); ++i)
		linearImage.setPixel(i, filmPixels[i].cumulativeColor / filmPixels[i].filterWeightSum);

	toneMapper->apply(scene, linearImage, toneMappedImage);
}

const Image& Film::getToneMappedImage() const
{
	return toneMappedImage;
}
