// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>
#include <iostream>
#include <regex>

#include "Utils/IniReader.h"
#include "Utils/Log.h"

using namespace Raycer;

namespace
{
	std::regex commentRegex("^\\s*[#;].*");
	std::regex sectionRegex("^\\s*\\[(\\S+)\\].*");
	std::regex valueRegex("^\\s*(\\S+)\\s*=\\s*(\\S+).*");
}

IniReader::IniReader(BaseLog& baseLog)
{
	log = baseLog.getNamedLog("IniReader");
}

void IniReader::readFile(const std::string& fileName)
{
	log->logInfo("Parsing values from %s", fileName);

	std::ifstream fileStream(fileName);
	std::string line;
	std::string sectionName = "unknown";

	while (std::getline(fileStream, line))
	{
		std::smatch match;

		if (std::regex_match(line, match, commentRegex))
			continue;
		
		if (std::regex_match(line, match, sectionRegex))
		{
			sectionName = match[1];
			continue;
		}

		if (std::regex_match(line, match, valueRegex))
			sections[sectionName][match[1]] = match[2];
	}
}
