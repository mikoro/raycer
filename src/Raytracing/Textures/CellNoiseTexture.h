// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Raytracing/Textures/Texture.h"
#include "Utils/CellNoise.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	class Vector2;

	class CellNoiseTexture : public Texture
	{
	public:

		void initialize();

		Color getColor(const Vector2& texcoord, const Vector3& position) const;
		double getValue(const Vector2& texcoord, const Vector3& position) const;
		Vector3 getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const;

		int seed = 1;
		Vector3 scale = Vector3(10.0, 10.0, 10.0);
		Color baseColor = Color(1.0, 1.0, 1.0);
		CellNoiseDistanceType distanceType = CellNoiseDistanceType::EUCLIDEAN;
		CellNoiseCombineType combineType = CellNoiseCombineType::D1;
		int density = 1;

	private:

		CellNoise cellNoise;
	};
}
