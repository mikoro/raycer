// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include "Math/Vector3.h"

namespace Raycer
{
	class Camera
	{
	public:

		Camera();
		
		Vector3 position = Vector3(0, 0, 0);
	};
}
