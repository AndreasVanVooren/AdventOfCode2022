#include "Day03.h"
#include <algorithm>
#include <vector>
#include <ranges>
#include <format>
#include <set>
#include "../Utils.h"

constexpr int ConvertToPriorityValue(char character) { return (character >= 'A' && character <= 'Z') ? (character - 'A' + 27) : ((character >= 'a' && character <= 'z') ? character - 'a' + 1 : 0); }

std::string Day03::GetResultStr()
{
	std::stringstream resultStream{};
	int mismatchPrioTotal{};
	int groupPrioTotal{};
	std::vector<std::string> groupSacks{};
	constexpr int MAX_SACKS_PER_GROUP = 3;
	
	const auto reset = [&]
	{
		mismatchPrioTotal = 0;
		groupPrioTotal = 0;
		groupSacks = {};
	};
	const auto output = [&](const std::string& debug)
	{
		resultStream << std::format("[{}] Mismatched priority total = {} | Group priority total = {}\n", debug, mismatchPrioTotal, groupPrioTotal);
	};

	const auto uniques = [](const auto& src) -> std::vector<char>
	{
		std::set<char> intermediate{ src.begin(), src.end() };
		return { intermediate.begin(), intermediate.end() };
	};

	const auto commitGroupSacks = [&]
	{
		if (groupSacks.size() <= 0) return;
		std::vector<char> uniqueInGroup = uniques(groupSacks[0]);
		for (const std::string& str : groupSacks)
		{
			const std::vector<char> uniqueCopy = uniqueInGroup;
			const std::vector<char> intermediateVec = uniques(str);
			uniqueInGroup = {};
			std::ranges::set_intersection(uniqueCopy, intermediateVec, std::back_inserter(uniqueInGroup));
		}

		if (uniqueInGroup.size() > 1) __debugbreak();

		if (uniqueInGroup.size() > 0) groupPrioTotal += ConvertToPriorityValue(uniqueInGroup[0]);

		groupSacks = {};
	};

	const auto processLambda = [&](const std::string& line)
	{
		groupSacks.push_back(line);
		const auto [lineLhs, lineRhs] = SplitString(line, line.length() / 2);
		if (lineLhs.length() != lineRhs.length()) __debugbreak();
		std::set<char> encountered;
		for (size_t i = 0; i < lineLhs.length(); ++i)
		{
			if (lineRhs.find(lineLhs[i]) != lineRhs.npos && !encountered.contains(lineLhs[i]))
			{ mismatchPrioTotal += ConvertToPriorityValue(lineLhs[i]); encountered.emplace(lineLhs[i]); }
		}
		if (groupSacks.size() >= MAX_SACKS_PER_GROUP) commitGroupSacks();
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	output("Test");

	if (mismatchPrioTotal != 157) __debugbreak();
	if (groupPrioTotal != 70) __debugbreak();

	reset();
	ForEachLineInInputFile(processLambda);
	output("Final");

	// Was too low
	if (mismatchPrioTotal <= 791) __debugbreak();

	return resultStream.str();
}

std::string Day03::GetIdStr()
{
	return "Day03";
}
