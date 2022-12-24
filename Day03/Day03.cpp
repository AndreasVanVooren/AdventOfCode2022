#include "Day03.h"
#include <algorithm>
#include <vector>
#include <ranges>
#include <set>
#include "../Utils.h"

constexpr int ConvertToPriorityValue(wchar_t character) { return (character >= L'A' && character <= L'Z') ? (character - L'A' + 27) : ((character >= L'a' && character <= L'z') ? character - L'a' + 1 : 0); }

std::wstring Day03::GetResultStr()
{
	std::wstringstream resultStream{};
	int mismatchPrioTotal{};
	int groupPrioTotal{};
	std::vector<std::wstring> groupSacks{};
	constexpr int MAX_SACKS_PER_GROUP = 3;
	
	const auto reset = [&]
	{
		mismatchPrioTotal = 0;
		groupPrioTotal = 0;
		groupSacks = {};
	};
	const auto output = [&](const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] Mismatched priority total = " << mismatchPrioTotal << " | Group priority total = " << groupPrioTotal << "\n";
	};

	const auto uniques = [](const auto& src) -> std::vector<wchar_t>
	{
		std::set<wchar_t> intermediate{ src.begin(), src.end() };
		return { intermediate.begin(), intermediate.end() };
	};

	const auto commitGroupSacks = [&]
	{
		if (groupSacks.size() <= 0) return;
		std::vector<wchar_t> uniqueInGroup = uniques(groupSacks[0]);
		for (const std::wstring& str : groupSacks)
		{
			const std::vector<wchar_t> uniqueCopy = uniqueInGroup;
			const std::vector<wchar_t> intermediateVec = uniques(str);
			uniqueInGroup = {};
			std::ranges::set_intersection(uniqueCopy, intermediateVec, std::back_inserter(uniqueInGroup));
		}

		if (uniqueInGroup.size() > 1) __debugbreak();

		if (uniqueInGroup.size() > 0) groupPrioTotal += ConvertToPriorityValue(uniqueInGroup[0]);

		groupSacks = {};
	};

	const auto processLambda = [&](const std::wstring_view& line)
	{
		groupSacks.push_back(std::wstring{ line });
		const auto [lineLhs, lineRhs] = SplitString(line, line.length() / 2);
		if (lineLhs.length() != lineRhs.length()) __debugbreak();
		std::set<wchar_t> encountered;
		for (size_t i = 0; i < lineLhs.length(); ++i)
		{
			if (lineRhs.find(lineLhs[i]) != lineRhs.npos && !encountered.contains(lineLhs[i]))
			{ mismatchPrioTotal += ConvertToPriorityValue(lineLhs[i]); encountered.emplace(lineLhs[i]); }
		}
		if (groupSacks.size() >= MAX_SACKS_PER_GROUP) commitGroupSacks();
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	output(L"Test");

	if (mismatchPrioTotal != 157) __debugbreak();
	if (groupPrioTotal != 70) __debugbreak();

	reset();
	ForEachLineInInputFile(processLambda);
	output(L"Final");

	// Was too low
	if (mismatchPrioTotal <= 791) __debugbreak();

	return resultStream.str();
}

std::wstring Day03::GetIdStr()
{
	return L"Day03";
}
