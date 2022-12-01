#include "Day01.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

std::string GetResultForStream(std::ifstream& fileStream)
{
	std::vector<int> valList{};
	int currentMax = std::numeric_limits<int>::lowest();
	int currentMaxGroup = std::numeric_limits<int>::lowest();
	int currentVal = 0;
	int currentGroup = 0;

	const auto addEntry = [&currentVal](const std::string& strVal)
	{
		currentVal += std::stoi(strVal);
	};

	const auto commitEntry = [&valList, &currentVal, &currentMax, &currentMaxGroup, &currentGroup]
	{
		//std::cout << "Committing group " << currentGroup + 1 << " with value " << currentVal << "\n";
		if (currentMax < currentVal)
		{
			currentMax = currentVal;
			currentMaxGroup = currentGroup;
		}
		valList.push_back(currentVal);
		currentVal = 0;
		++currentGroup;

	};

	std::string line;
	while (std::getline(fileStream, line))
	{
		if (line.empty())
		{
			commitEntry();
		}
		else
		{
			addEntry(line);
		}
	}
	// Ensure the final entry is committed.
	commitEntry();

	std::stringstream resultStr{};
	resultStr << currentMax << " calories found in elf " << currentMaxGroup + 1 << "\n";

	{
		auto valCopy = valList;
		std::ranges::sort(valCopy, std::ranges::greater{});

		int topThreeTotal = 0;
		for (int i = 0; i < 3 && i < valCopy.size(); i++)
		{
			topThreeTotal += valCopy[i];
		}
		resultStr << "Top three elves account for " << topThreeTotal << " calories\n";
	}

	return resultStr.str();
}
std::string Day01::GetResultStr()
{
	std::filesystem::path filePath = std::filesystem::current_path() / "Day01" / "input";
	std::ifstream fileStream{ filePath };
	return GetResultForStream(fileStream);
};
