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
		int maxLeafSize = 5;
		bool useSAH = true;
		int regularSAHSplits = 0;
		BVHAxisSelection axisSelection = BVHAxisSelection::LARGEST;
		BVHAxisSplit axisSplit = BVHAxisSplit::MEDIAN;

		template<class Archive>
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
		int rightOffset;
		int startOffset;
		int primitiveCount;

		template<class Archive>
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
		int start;
		int end;
		int parent;
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

		void initialize(const Scene& scene);
		bool intersect(const Ray& ray, Intersection& intersection, std::vector<Intersection>& intersections);
		AABB getAABB() const;
		void transform(const Vector3& scale, const EulerAngle& rotate, const Vector3& translate);

		void build(const std::vector<Primitive*>& primitives, const BVHBuildInfo& buildInfo, const Scene& scene);

		bool hasBeenBuilt = false;
		std::vector<FlatBVHNode> flatNodes;
		std::vector<int> orderedPrimitiveIds;

	private:

		void calculateSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry, std::mt19937& generator);
		void calculateSAHSplit(int& axis, double& splitPoint, const AABB& nodeAABB, const BVHBuildInfo& buildInfo, const FlatBVHBuildEntry& buildEntry);
		double calculateSAHScore(int axis, double splitPoint, const AABB& nodeAABB, const FlatBVHBuildEntry& buildEntry);
		double calculateMedianPoint(int axis, const FlatBVHBuildEntry& buildEntry);

		std::vector<Primitive*> orderedPrimitives;

		friend class cereal::access;

		template<class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("primitive", cereal::base_class<Primitive>(this)),
				CEREAL_NVP(hasBeenBuilt),
				CEREAL_NVP(flatNodes),
				CEREAL_NVP(orderedPrimitiveIds));
		}
	};
}
