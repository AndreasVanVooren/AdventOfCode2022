#include "Day01.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

std::wstring Day01::GetResultForStream(const std::filesystem::path& path)
{
	std::vector<int> valList{};
	int currentMax = std::numeric_limits<int>::lowest();
	int currentMaxGroup = std::numeric_limits<int>::lowest();
	int currentVal = 0;
	int currentGroup = 0;

	const auto addEntry = [&currentVal](const std::wstring& strVal)
	{
		currentVal += std::stoi(strVal);
	};

	const auto commitEntry = [&valList, &currentVal, &currentMax, &currentMaxGroup, &currentGroup]
	{
		//std::wcout << "Committing group " << currentGroup + 1 << " with value " << currentVal << "\n";
		if (currentMax < currentVal)
		{
			currentMax = currentVal;
			currentMaxGroup = currentGroup;
		}
		valList.push_back(currentVal);
		currentVal = 0;
		++currentGroup;

	};

	ForEachLineInFile(path, [&commitEntry, &addEntry](const std::wstring& line)
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
	);
	// Ensure the final entry is committed.
	commitEntry();

	std::wstringstream resultStr{};
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
std::wstring Day01::GetIdStr()
{
	return L"Day01";
}
std::wstring Day01::GetResultStr()
{
	std::wcout << GetResultForStream(GetTestInputPath());
	return GetResultForStream(GetInputPath());
};
