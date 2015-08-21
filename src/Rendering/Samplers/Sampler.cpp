// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>

#include "Rendering/Samplers/Sampler.h"
#include "Rendering/Samplers/RandomSampler.h"
#include "Rendering/Samplers/RegularSampler.h"
#include "Rendering/Samplers/JitteredSampler.h"
#include "Rendering/Samplers/CMJSampler.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Raytracing/ONB.h"

using namespace Raycer;

Vector2 Sampler::mapToDisk(const Vector2& point)
{
	Vector2 result;

	// square to disk polar mapping
	double theta = 2.0 * M_PI * point.x;
	double r = sqrt(point.y);

	result.x = r * cos(theta);
	result.y = r * sin(theta);

	return result;
}

Vector3 Sampler::mapToHemisphere(const ONB& onb, double distribution, const Vector2& point)
{
	// square to hemisphere mapping with cosine distribution
	double phi = 2.0 * M_PI * point.x;
	double cos_phi = cos(phi);
	double sin_phi = sin(phi);
	double cos_theta = pow(1.0 - point.y, 1.0 / (distribution + 1.0));
	double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	double u = sin_theta * cos_phi;
	double v = sin_theta * sin_phi;
	double w = cos_theta;

	return u * onb.u + v * onb.v + w * onb.w;
}

Sampler* Sampler::getSampler(SamplerType type)
{
	switch (type)
	{
		case SamplerType::RANDOM: return new RandomSampler(); break;
		case SamplerType::REGULAR: return new RegularSampler(); break;
		case SamplerType::JITTERED: return new JitteredSampler(); break;
		case SamplerType::CMJ: return new CMJSampler(); break;
		default: throw std::runtime_error("Invalid sampler type");
	}
}