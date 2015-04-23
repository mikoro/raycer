// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

namespace Raycer
{
	class Color;
	class Vector3;
	class Vector2;

	class Texture
	{
		public:

			virtual void initialize() = 0;
			virtual Color getColor(const Vector3& position, const Vector2& texcoord) const = 0;

			int id = 0;
	};
}
