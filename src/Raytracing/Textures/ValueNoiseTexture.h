// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Utils/ValueNoise.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;

	class ValueNoiseTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const;

		int seed = 1;
		Vector3 scale = Vector3(1.0, 1.0, 1.0);
		Color baseColor = Color(1.0, 1.0, 1.0);
		bool isFbm = false;
		int octaves = 4;
		double lacunarity = 2.0;
		double persistence = 0.5;

	private:

		ValueNoise valueNoise;
	};
}
