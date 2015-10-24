// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <memory>
#include <vector>

#include "Math/Color.h"
#include "Samplers/Sampler.h"
#include "Filters/Filter.h"
#include "ToneMappers/ToneMapper.h"
#include "Rendering/Image.h"

namespace Raycer
{
	class Camera;
	class Ray;

	struct FilmPixel
	{
		Color cumulativeColor;
		double filterWeightSum = 0.0;
		uint64_t samplesLeft = 1;
		uint64_t sampleX = 0;
		uint64_t sampleY = 0;
	};

	class Film
	{
	public:

		Film();
		Film(uint64_t width, uint64_t height);

		void resize(uint64_t width, uint64_t height);
		void clear();
		void resetSampleCounts();
		bool getRay(uint64_t x, uint64_t y, const Camera& camera, Ray& ray);
		void applyToneMap();

		void setSampleCount(uint64_t sampleCountSqrt);
		void setSampler(std::unique_ptr<Sampler> sampler);
		void setFilter(std::unique_ptr<Filter> filter);
		void setToneMapper(std::unique_ptr<ToneMapper> toneMapper);

		const std::vector<FilmPixel>& getFilmPixels() const;
		const Image& getOutputImage() const;

	private:

		uint64_t width = 0;
		uint64_t height = 0;
		uint64_t sampleCountSqrt = 1;

		std::unique_ptr<Sampler> sampler = nullptr;
		std::unique_ptr<Filter> filter = nullptr;
		std::unique_ptr<ToneMapper> toneMapper = nullptr;

		std::vector<FilmPixel> filmPixels;
		Image outputImage;
	};
}
