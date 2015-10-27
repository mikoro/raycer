// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Rendering/Film.h"
#include "Rendering/ToneMappers/PassthroughToneMapper.h"
#include "Rendering/ToneMappers/LinearToneMapper.h"
#include "Rendering/ToneMappers/SimpleToneMapper.h"
#include "Rendering/ToneMappers/ReinhardToneMapper.h"
#include "Raytracing/Scene.h"

using namespace Raycer;

Film::Film()
{
	toneMappers[ToneMapperType::PASSTHROUGH] = std::make_unique<PassthroughToneMapper>();
	toneMappers[ToneMapperType::LINEAR] = std::make_unique<LinearToneMapper>();
	toneMappers[ToneMapperType::SIMPLE] = std::make_unique<SimpleToneMapper>();
	toneMappers[ToneMapperType::REINHARD] = std::make_unique<ReinhardToneMapper>();
}

void Film::resize(uint64_t width_, uint64_t height_)
{
	width = width_;
	height = height_;

	filmPixels.resize(width * height);
	linearImage.resize(width, height);
	toneMappedImage.resize(width, height);

	clear();
}

void Film::resize(uint64_t length)
{
	resize(length, 1);
}

void Film::clear()
{
	for (auto& filmPixel : filmPixels)
	{
		filmPixel.cumulativeColor = Color(0.0, 0.0, 0.0, 0.0);
		filmPixel.filterWeightSum = 0.0;
	}
}

void Film::addSample(uint64_t x, uint64_t y, const Color& color, double filterWeight)
{
	FilmPixel& filmPixel = filmPixels[y * width + x];
	filmPixel.cumulativeColor += color * filterWeight;
	filmPixel.filterWeightSum += filterWeight;
}

void Film::generateToneMappedImage(const Scene& scene)
{
	for (uint64_t i = 0; i < filmPixels.size(); ++i)
		linearImage.setPixel(i, filmPixels[i].cumulativeColor / filmPixels[i].filterWeightSum);

	ToneMapper* toneMapper = toneMappers[scene.toneMapper.type].get();
	toneMapper->apply(scene, linearImage, toneMappedImage);
}

void Film::setToneMappedImage(const Image& other)
{
	toneMappedImage = other;
}

const Image& Film::getToneMappedImage() const
{
	return toneMappedImage;
}

uint64_t Film::getWidth() const
{
	return width;
}

uint64_t Film::getHeight() const
{
	return height;
}
