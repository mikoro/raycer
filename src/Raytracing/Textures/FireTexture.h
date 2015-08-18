// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Utils/PerlinNoise.h"
#include "Utils/ColorGradient.h"

namespace Raycer
{
	class Color;
	class Vector2;
	class Vector3;

	class FireTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const;

		int seed = 1;

	private:

		PerlinNoise perlinNoise;
		ColorGradient colorGradient;
	};
}
