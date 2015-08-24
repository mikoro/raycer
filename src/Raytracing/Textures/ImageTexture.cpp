// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "Raytracing/Textures/ImageTexture.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

using namespace Raycer;

void ImageTexture::initialize()
{
	image.load(imageFilePath);

	if (applyGamma)
		image.applyGamma(gamma);

	if (isBumpMap)
	{
		bumpMapX = Image(image.getWidth(), image.getHeight());
		bumpMapY = Image(image.getWidth(), image.getHeight());

		for (int y = 0; y < image.getHeight(); ++y)
		{
			for (int x = 0; x < image.getWidth(); ++x)
			{
				Color current = image.getPixel(x, y);

				if (x < image.getWidth() - 1)
				{
					Color right = image.getPixel(x + 1, y);
					Color rightDiff = right - current;
					bumpMapX.setPixel(x, y, rightDiff);
				}
				else
					bumpMapX.setPixel(x, y, bumpMapX.getPixel(x - 1, y));

				if (y < image.getHeight() - 1)
				{
					Color top = image.getPixel(x, y + 1);
					Color topDiff = top - current;
					bumpMapY.setPixel(x, y, topDiff);
				}
				else
					bumpMapY.setPixel(x, y, bumpMapY.getPixel(x, y - 1));
			}
		}
	}
}

Color ImageTexture::getColor(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	return image.getPixelBilinear(texcoord.x, texcoord.y);
}

double ImageTexture::getValue(const Vector2& texcoord, const Vector3& position) const
{
	(void)position;

	Color color = image.getPixelBilinear(texcoord.x, texcoord.y);

	return 0.2126 * pow(color.r, gamma) + 0.7152 * pow(color.g, gamma) + 0.0722 * pow(color.b, gamma);
}

Vector3 ImageTexture::getNormal(const Vector2& texcoord, const Vector3& position, TextureNormalType& type) const
{
	(void)position;

	Vector3 normal;
	type = TextureNormalType::BUMP;

	if (isBumpMap)
	{
		normal.x = -bumpMapX.getPixelBilinear(texcoord.x, texcoord.y).r;
		normal.y = -bumpMapY.getPixelBilinear(texcoord.x, texcoord.y).r;
	}
	
	return normal;
}
