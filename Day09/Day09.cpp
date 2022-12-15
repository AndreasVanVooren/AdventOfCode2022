#include "Day09.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <ranges>
#include "../Utils.h"
#include "../ScopeProfiler.h"

using coord = std::pair<int, int>;

struct Knot
{
	coord Location;
	std::optional<std::vector<coord>> VisitedLocations;
	size_t Moves;
};

struct ReactToKnotMoveFunctor
{
	size_t knotIndex;
	std::vector<Knot>& knots;
	void operator()() const
	{
		const Knot& prevKnot = knots[knotIndex - 1];
		Knot& thisKnot = knots[knotIndex];
		const coord oldTailPos = thisKnot.Location;
		coord newTailPos = thisKnot.Location;
		const bool bLeftTooFar = (prevKnot.Location.first - thisKnot.Location.first) >= 2;
		const bool bRightTooFar = (thisKnot.Location.first - prevKnot.Location.first) >= 2;
		const bool bBotTooFar = (prevKnot.Location.second - thisKnot.Location.second) >= 2;
		const bool bTopTooFar = (thisKnot.Location.second - prevKnot.Location.second) >= 2;

		if (bLeftTooFar || bRightTooFar || bBotTooFar || bTopTooFar)
		{
			newTailPos = prevKnot.Location;
			if (bLeftTooFar)
			{
				--newTailPos.first;
			}
			else if (bRightTooFar)
			{
				++newTailPos.first;
			}

			if (bBotTooFar)
			{
				--newTailPos.second;
			}
			else if (bTopTooFar)
			{
				++newTailPos.second;
			}
		}

		if (newTailPos != oldTailPos)
		{
			//std::wcout << "Knot " << knotIndex << " moves to [" << newTailPos.first << "," << newTailPos.second << "]\n";
			thisKnot.Location = newTailPos;
			++thisKnot.Moves;

			if (thisKnot.VisitedLocations.has_value() && std::ranges::find(thisKnot.VisitedLocations.value(), thisKnot.Location) == thisKnot.VisitedLocations.value().end())
			{
				thisKnot.VisitedLocations.value().push_back(thisKnot.Location);
			}
			if ((knotIndex + 1) < knots.size())
			{
				ReactToKnotMoveFunctor{ knotIndex + 1, knots }();
			}
		}
	}
};

std::wstring Day09::GetResultStr()
{
	std::wstringstream resultStream{};
	std::vector<Knot> knotInfos{};
	const auto reset = [&]
	{
		constexpr const size_t KNOT_AMOUNT = 10;
		knotInfos = std::vector<Knot>(KNOT_AMOUNT);
		knotInfos.back().VisitedLocations = std::vector<coord>{ knotInfos.back().Location };
	};
	const auto output = [&](const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] Tail visited " << knotInfos.back().VisitedLocations.value().size() << L" locations\n";
	};
	const auto processOneMove = [&](const wchar_t cmd)
	{
		//ScopeProfiler prof{ "moveProcess" };
		Knot& thisKnot = knotInfos[0];
		coord newHeadPos = thisKnot.Location;
		switch (cmd)
		{
		case L'L':
			++thisKnot.Moves;
			--newHeadPos.first;
			break;
		case L'R':
			++thisKnot.Moves;
			++newHeadPos.first;
			break;
		case L'D':
			++thisKnot.Moves;
			--newHeadPos.second;
			break;
		case L'U':
			++thisKnot.Moves;
			++newHeadPos.second;
			break;
		default:
			break;
		}

		thisKnot.Location = newHeadPos;
		//std::wcout << L"Head knot moves to [" << thisKnot.Location.first << L"," << thisKnot.Location.second << L"]\n";
		//if (std::ranges::find(thisKnot.VisitedLocations, thisKnot.Location) == thisKnot.VisitedLocations.end())
		//{
		//	thisKnot.VisitedLocations.push_back(thisKnot.Location);
		//}
		ReactToKnotMoveFunctor{ 1, knotInfos }();
	};
	const auto processMove = [&](const wchar_t cmd, int amount)
	{
		//ScopeProfiler prof{ L"bulkMoveProcess" };
		for (int i = 0; i < amount; i++)
		{
			processOneMove(cmd);
		}
	};
	const auto processCmdList = [&](const std::wstring_view& line)
	{
		if (line.empty()) return;
		const auto [cmd, amountStr] = SplitString(line, L" ");
		processMove(cmd[0], std::stoi(amountStr));
	};

	reset();
	ForEachLineInTestInputFile(processCmdList, 0);
	output(L"Test 0");
	if (knotInfos.back().VisitedLocations.value().size() != 1) __debugbreak();


	reset();
	ForEachLineInTestInputFile(processCmdList, 1);
	output(L"Test 1");
	if (knotInfos.back().VisitedLocations.value().size() != 36) __debugbreak();

	reset();
	ForEachLineInInputFile(processCmdList);
	output(L"Final");
	if (knotInfos.back().VisitedLocations.value().size() == 2355) __debugbreak();
	if (knotInfos.back().VisitedLocations.value().size() == 2596) __debugbreak();

	return resultStream.str();
};
std::wstring Day09::GetIdStr()
{
	return L"Day09";
};
