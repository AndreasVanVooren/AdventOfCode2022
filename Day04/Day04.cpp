#include "Day04.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <ranges>
#include <format>
#include <set>
#include "../Utils.h"

std::wstring Day04::GetResultStr()
{
	std::wstringstream resultStream{};
	int overlapTotal{};
	int overlapFullyTotal{};
	const auto reset = [&]
	{
		overlapTotal = 0;
		overlapFullyTotal = 0;
	};
	const auto output = [&](const std::wstring& debug)
	{
		resultStream << std::format(L"[{}] Found {} overlapping groups, where {} fully overlap\n", debug, overlapTotal, overlapFullyTotal);
	};
	const auto processLambda = [&](const std::wstring& line)
	{
		if (line.empty()) return;
		const auto [pairL, pairR] = SplitString(line, L",");
		const auto [firstL, lastL] = SplitString(pairL, L"-");
		const auto [firstR, lastR] = SplitString(pairR, L"-");
		const int 
			  firstLId = std::stoi(firstL)
			, lastLId = std::stoi(lastL)
			, firstRId = std::stoi(firstR)
			, lastRId = std::stoi(lastR);

		if ((firstLId >= firstRId && lastLId <= lastRId) || firstRId >= firstLId && lastRId <= lastLId)
		{
			//std::wcout << std::format(L"Full Overlap detected between [{}] and [{}]\n", pairL, pairR);
			++overlapFullyTotal;
			++overlapTotal;
		}
		else if ((firstLId >= firstRId && firstLId <= lastRId) || (lastLId >= firstRId && lastLId <= lastRId))
		{
			//std::wcout << std::format(L"Partial Overlap detected between [{}] and [{}]\n", pairL, pairR);
			++overlapTotal;
		}
		else
		{
			//std::wcout << std::format(L"No overlap between [{}] and [{}]\n", pairL, pairR);
		}
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	output(L"Test");

	if (overlapTotal != 4) __debugbreak();
	if (overlapFullyTotal != 2) __debugbreak();

	reset();
	ForEachLineInInputFile(processLambda);
	output(L"Final");

	return resultStream.str();
};

std::wstring Day04::GetIdStr()
{
	return L"Day04";
};
