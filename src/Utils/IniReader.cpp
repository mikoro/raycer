// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <fstream>
#include <iostream>
#include <regex>

#include "Utils/IniReader.h"
#include "App.h"
#include "Utils/Log.h"

using namespace Raycer;

namespace
{
	const std::regex commentRegex("^\\s*[#;].*");
	const std::regex sectionRegex("^\\s*\\[(\\S+)\\].*");
	//const std::regex valueRegex("^\\s*(\\S+)\\s*=\\s*(\\S+).*"); // captures everything until first whitespace after =
	const std::regex valueRegex("^\\s*(\\S+)\\s*=\\s*(.+)"); // captures everything after =
}

void IniReader::readFile(const std::string& fileName)
{
	App::getLog().logInfo("Parsing ini file %s", fileName);

	std::ifstream fileStream(fileName);
	std::string line;
	std::string sectionName = "unknown";

	while (std::getline(fileStream, line))
	{
		std::smatch match;
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

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

std::string IniReader::getValue(const std::string& sectionName, const std::string& keyName)
{
	if (sections.count(sectionName) == 0 || sections[sectionName].count(keyName) == 0)
		throw std::runtime_error(tfm::format("Could not find %s::%s from the ini file", sectionName, keyName));

	return sections[sectionName][keyName];
}
