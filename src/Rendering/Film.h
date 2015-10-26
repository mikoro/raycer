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
	class Scene;

	struct FilmPixel
	{
		Color cumulativeColor = Color(0.0, 0.0, 0.0, 0.0);
		double filterWeightSum = 0.0;
		std::unique_ptr<Sampler> sampler = nullptr;
	};

	class Film
	{
	public:

		void resize(uint64_t width, uint64_t height);
		void setSampler(SamplerType type, uint64_t sampleCountSqrt);
		void setFilter(FilterType type);
		void setToneMapper(ToneMapperType type);

		void clear();
		bool getNextRay(uint64_t x, uint64_t y, const Camera& camera, Ray& ray, double time);
		void addSample(uint64_t x, uint64_t y, const Vector2& sampleOffset, const Color& color);
		void generateOutputImage(const Scene& scene);

		const Image& getToneMappedImage() const;

	private:

		uint64_t width = 0;
		uint64_t height = 0;

		std::unique_ptr<Filter> filter = nullptr;
		std::unique_ptr<ToneMapper> toneMapper = nullptr;

		std::vector<FilmPixel> filmPixels;
		Image linearImage;
		Image toneMappedImage;
	};
}
