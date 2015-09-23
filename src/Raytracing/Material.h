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
		
		bool skipLighting = false;
		bool nonShadowing = false;
		bool normalInterpolation = true;
		bool backfaceCulling = false;
		bool invertNormal = false;
		bool enableCSG = false;

		Vector2 texcoordScale = Vector2(1.0, 1.0);

		Texture* ambientMapTexture = nullptr;
		int ambientMapTextureId = 0;
		Color ambientReflectance = Color(1.0, 1.0, 1.0);

		Texture* diffuseMapTexture = nullptr;
		int diffuseMapTextureId = 0;
		Color diffuseReflectance = Color(1.0, 1.0, 1.0);

		Texture* specularMapTexture = nullptr;
		int specularMapTextureId = 0;
		Color specularReflectance = Color(0.0, 0.0, 0.0);
		double shininess = 2.0;

		Texture* rayReflectanceMapTexture = nullptr;
		int rayReflectanceMapTextureId = 0;
		double rayReflectance = 0.0;
		bool fresnelReflection = false;

		Texture* rayTransmittanceMapTexture = nullptr;
		int rayTransmittanceMapTextureId = 0;
		double rayTransmittance = 0.0;
		double refractiveIndex = 1.0;
		bool enableAttenuation = false;
		double attenuationFactor = 1.0;
		Color attenuationColor = Color(0.0, 0.0, 0.0);

		Texture* normalMapTexture = nullptr;
		int normalMapTextureId = 0;
		
		Texture* heightMapTexture = nullptr;
		int heightMapTextureId = 0;
	};
}
