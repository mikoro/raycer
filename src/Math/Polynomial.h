// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <algorithm>
#include <complex>

namespace Raycer
{
	template <uint64_t N>
	class Polynomial
	{
	public:

		Polynomial();
		explicit Polynomial(const double* coefficients);
		Polynomial& operator=(const Polynomial &other) = delete;

		void setCoefficients(const double* coefficients);

		std::complex<double> evaluate(const std::complex<double>& x) const;

		const std::complex<double>* findAllRoots(uint64_t maxIterations = 64, double changeThreshold = 0.0001);
		const double* findAllPositiveRealRoots(uint64_t& count, uint64_t maxIterations = 64, double changeThreshold = 0.0001, double imagZeroThreshold = 0.0001);
		bool findSmallestPositiveRealRoot(double& result, uint64_t maxIterations = 64, double changeThreshold = 0.0001, double imagZeroThreshold = 0.0001);

	private:

		const uint64_t size = N;
		const uint64_t degree = N - 1;

		double coefficients[N];
		std::complex<double> roots[N - 1];
		std::complex<double> previousRoots[N - 1];
		double positiveRealRoots[N - 1];
	};
}

#include "Math/Polynomial.inl"
