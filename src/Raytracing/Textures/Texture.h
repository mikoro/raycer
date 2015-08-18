// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;
	class Vector2;
	class Vector3;

	enum class TextureNormalType { BUMP, GRADIENT, NORMAL };

	class Texture
	{
	public:

		virtual ~Texture() {}

		virtual void initialize() = 0;

		virtual Color getColor(const Vector2& texcoord, const Vector3& position) const = 0;
		virtual double getValue(const Vector2& texcoord, const Vector3& position) const = 0;
		virtual Vector3 getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const = 0;

		int id = 0;
		double intensity = 1.0;
	};
}
