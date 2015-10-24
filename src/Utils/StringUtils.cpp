// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Utils/StringUtils.h"

using namespace Raycer;

bool StringUtils::endsWith(const std::string& input, const std::string& end)
{
	return input.rfind(end) == (input.size() - end.size());
}

std::string StringUtils::readFileToString(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);

	if (!file.good())
		throw new std::runtime_error(tfm::format("Could not open file: %s", filePath));

	auto size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	file.read(&buffer[0], size);

	return std::string(&buffer[0], size);
}

bool StringUtils::readUntilSpace(const std::string& input, uint64_t& startIndex, std::string& result)
{
	bool nonSpaceWasFound = false;

	// find first non-space character
	for (; startIndex < input.size(); ++startIndex)
	{
		char c = input[startIndex];

		if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
		{
			nonSpaceWasFound = true;
			break;
		}
	}

	if (!nonSpaceWasFound)
		return false;

	uint64_t endIndex = startIndex;

	// find next space character
	for (; endIndex < input.size(); ++endIndex)
	{
		char c = input[endIndex];

		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			break;
	}

	if (startIndex == endIndex)
		return false;

	result = input.substr(startIndex, endIndex - startIndex);
	startIndex = endIndex;

	return true;
}

double StringUtils::parseDouble(const std::string& input)
{
	const char* p = input.c_str();

	double sign = 1.0;
	double accumulator = 0.0;

	if (*p == '-')
	{
		sign = -1.0;
		++p;
	}

	while (*p >= '0' && *p <= '9')
	{
		accumulator = accumulator * 10 + *p - '0';
		++p;
	}

	if (*p == '.')
	{
		double k = 0.1;
		p++;

		while (*p >= '0' && *p <= '9')
		{
			accumulator += (*p - '0') * k;
			k *= 0.1;
			++p;
		}
	}

	return sign * accumulator;
}

std::string StringUtils::humanizeNumber(double value, bool usePowerOfTwo)
{
	const char* prefixes[] = { "", "k", "M", "G", "T", "P", "E", "Z", "Y" };
	const double divider = usePowerOfTwo ? 1024.0 : 1000.0;

	for (uint64_t i = 0; i < 9; ++i)
	{
		if (value < divider)
			return tfm::format("%.2f %s", value, prefixes[i]);
		else
			value /= divider;
	}

	return tfm::format("%.2f %s", value, "Y");
}
