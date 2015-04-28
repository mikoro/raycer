// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <string>
#include <vector>

#include "Math/Color.h"
#include "Rendering/Image.h"
#include "Utils/PerlinNoise.h"
#include "Utils/CellNoise.h"

namespace Raycer
{
	class Vector2;
	class Vector3;

	class Texture
	{
	public:

		virtual void initialize() = 0;
		virtual Color getColor(const Vector3& position, const Vector2& texcoord) const = 0;
		virtual double getValue(const Vector3& position, const Vector2& texcoord) const = 0;

		int id = 0;
		double intensity = 1.0;
	};

	class ColorTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		Color color;
	};

	class CheckerTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		Color color1 = Color(0.3, 0.3, 0.3);
		Color color2 = Color(0.1, 0.1, 0.1);
	};
	
	class ImageTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		std::string imageFilePath;

	private:

		Image image;
	};

	class PerlinNoiseTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		PerlinNoise perlinNoise;
	};

	class CellNoiseTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		CellNoise cellNoise;
	};

	class MarbleTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		PerlinNoise perlinNoise;
	};

	class WoodTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		PerlinNoise perlinNoise;
	};

	class FireTexture : public Texture
	{
	public:

		void initialize();
		Color getColor(const Vector3& position, const Vector2& texcoord) const;
		double getValue(const Vector3& position, const Vector2& texcoord) const;

		PerlinNoise perlinNoise;
	};
}
