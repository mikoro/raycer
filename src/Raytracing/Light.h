// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"
#include "Math/Color.h"

namespace Raycer
{
	class Light
	{
	public:

		Light();
		
		Vector3 position = Vector3(0, 0, 0);
		Vector3 direction = Vector3(1, 0, 0);
		Color color = Color::WHITE;
	};
}
