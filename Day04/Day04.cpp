#include "Day04.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <ranges>
#include <format>
#include <set>
#include "../Utils.h"

std::string Day04::GetResultStr()
{
	std::stringstream resultStream{};
	int overlapTotal{};
	int overlapFullyTotal{};
	const auto reset = [&]
	{
		overlapTotal = 0;
		overlapFullyTotal = 0;
	};
	const auto output = [&](const std::string& debug)
	{
		resultStream << std::format("[{}] Found {} overlapping groups, where {} fully overlap\n", debug, overlapTotal, overlapFullyTotal);
	};
	const auto processLambda = [&](const std::string& line)
	{
		if (line.empty()) return;
		const auto [pairL, pairR] = SplitString(line, ",");
		const auto [firstL, lastL] = SplitString(pairL, "-");
		const auto [firstR, lastR] = SplitString(pairR, "-");
		const int 
			  firstLId = std::stoi(firstL)
			, lastLId = std::stoi(lastL)
			, firstRId = std::stoi(firstR)
			, lastRId = std::stoi(lastR);

		if ((firstLId >= firstRId && lastLId <= lastRId) || firstRId >= firstLId && lastRId <= lastLId)
		{
			std::cout << std::format("Full Overlap detected between [{}] and [{}]\n", pairL, pairR);
			++overlapFullyTotal;
			++overlapTotal;
		}
		else if ((firstLId >= firstRId && firstLId <= lastRId) || (lastLId >= firstRId && lastLId <= lastRId))
		{
			std::cout << std::format("Partial Overlap detected between [{}] and [{}]\n", pairL, pairR);
			++overlapTotal;
		}
		else
		{
			std::cout << std::format("No overlap between [{}] and [{}]\n", pairL, pairR);
		}
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	output("Test");

	if (overlapTotal != 4) __debugbreak();
	if (overlapFullyTotal != 2) __debugbreak();

	reset();
	ForEachLineInInputFile(processLambda);
	output("Final");

	return resultStream.str();
};

std::string Day04::GetIdStr()
{
	return "Day04";
};
