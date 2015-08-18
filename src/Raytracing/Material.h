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
		int colorTextureId = 0;
		int normalMapTextureId = 0;
		int displacementMapTextureId = 0;

		Vector2 texcoordScale = Vector2(1.0, 1.0);

		Color ambientReflectance = Color(1.0, 1.0, 1.0);
		Color diffuseReflectance = Color(1.0, 1.0, 1.0);
		Color specularReflectance = Color(0.0, 0.0, 0.0);
		double shininess = 0.0;

		bool skipLighting = false;
		bool nonShadowing = false;

		double rayReflectance = 0.0;
		double rayTransmittance = 0.0;
		double refractiveIndex = 1.0;
		bool isFresnel = false;

		bool enableAttenuation = false;
		double attenuation = 1.0;
		Color attenuationColor = Color(0.0, 0.0, 0.0);

		Texture* colorTexture = nullptr;
		Texture* normalMapTexture = nullptr;
		Texture* displacementMapTexture = nullptr;
	};
}
