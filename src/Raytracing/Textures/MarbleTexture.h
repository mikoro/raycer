// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Utils/PerlinNoise.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;
	class Vector3;

	class MarbleTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormalData(const Vector2& texcoord, const Vector3& position, TextureNormalDataType& type) const;

		int seed = 1;
		Color marbleColor = Color(255, 252, 240);
		Color streakColor = Color(0, 33, 71);
		double density = 10.0;
		double swirliness = 15.0;
		double transparency = 2.0;

	private:

		PerlinNoise perlinNoise;
	};
}
