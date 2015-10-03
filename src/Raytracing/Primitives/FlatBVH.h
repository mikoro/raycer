// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <random>
#include <vector>

#include "cereal/cereal.hpp"

#include "Raytracing/Primitives/Primitive.h"
#include "Raytracing/AABB.h"

namespace Raycer
{
	enum class BVHAxisSelection { LARGEST, RANDOM };
	enum class BVHAxisSplit { MIDDLE, MEDIAN, RANDOM };

	struct BVHBuildInfo
	{
		size_t maxLeafSize = 5;
		bool useSAH = true;
		size_t regularSAHSplits = 0;
		BVHAxisSelection axisSelection = BVHAxisSelection::LARGEST;
		BVHAxisSplit axisSplit = BVHAxisSplit::MEDIAN;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(maxLeafSize),
				CEREAL_NVP(useSAH),
				CEREAL_NVP(regularSAHSplits),
				CEREAL_NVP(axisSelection),
				CEREAL_NVP(axisSplit));
		}
	};

	struct FlatBVHNode
	{
		AABB aabb;
		int64_t rightOffset;
		size_t startOffset;
		size_t primitiveCount;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(CEREAL_NVP(aabb),
				CEREAL_NVP(rightOffset),
				CEREAL_NVP(startOffset),
				CEREAL_NVP(primitiveCount));
		}
	};

	struct FlatBVHBuildEntry
	{
		size_t start;
		size_t end;
		int64_t parent;
	};

	class Scene;
	class Ray;
	struct Intersection;
	class Vector3;
	class EulerAngle;

	class FlatBVH : public Primitive
	{
	public:

		friend class Scene;
		friend class CLScene;

		void initialize(const Scene& scene) override;
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections) override;
		AABB getAABB() const override;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate) override;

		void build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo);
		void rebuild(const Scene& scene);

		bool hasBeenBuilt = false;
		std::vector<FlatBVHNode> flatNodes;
		std::vector<size_t> orderedPrimitiveIds;

		std::vector<Primitive*> orderedPrimitives;

	private:

		void calculateSplit(uint& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry, std::mt19937& generator);
		void calculateSAHSplit(uint& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry);
		double calculateSAHScore(uint axis, double splitPoint, const AABB& nodeAABB, const FlatBVHBuildEntry& buildEntry);
		double calculateMedianPoint(uint axis, const FlatBVHBuildEntry& buildEntry);

		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(hasBeenBuilt),
				CEREAL_NVP(flatNodes),
				CEREAL_NVP(orderedPrimitiveIds));
		}
	};
}
