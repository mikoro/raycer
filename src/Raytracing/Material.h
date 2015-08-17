// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Color.h"
#include "Math/Vector2.h"

namespace Raycer
{
	class Texture;

	struct Material
	{
		int id = 0;
		int textureId = 0;
		int bumpMapTextureId = 0;
		int displacementMapTextureId = 0;

		Texture* texture = nullptr;
		Texture* bumpMapTexture = nullptr;
		Texture* displacementMapTexture = nullptr;

		Vector2 texcoordScale = Vector2(1.0, 1.0);

		bool skipLighting = false;
		bool nonShadowing = false;

		double ambientness = 1.0;
		double diffuseness = 1.0;
		double specularity = 0.0;
		double shininess = 0.0;

		bool fresnel = false;
		double reflectance = 0.0;
		double transmittance = 0.0;
		double refractiveIndex = 1.0;

		bool attenuate = false;
		double attenuation = 1.0;
		Color attenuationColor;
	};
}
