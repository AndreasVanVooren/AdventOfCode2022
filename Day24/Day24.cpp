#include "Day24.h"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <numeric>
#include <functional>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>
#include <map>
#include <regex>
#include <ranges>
#include "../Utils.h"
#include "../ScopeProfiler.h"

using namespace std;

namespace Day24Internals
{
	enum class TravelDirection
	{
		N,
		S,
		W,
		E,
	};

	struct Blizzard
	{
		coord Position{};
		TravelDirection Dir{};
	};

	struct LocationData
	{
		vector<size_t> blizzardsAtThisLocation{};
		bool bIsWall{};
	};

	wostream& operator<<(wostream& stream, const coord& c)
	{
		stream << L"["<<c.first<< L","<<c.second<< L"]";
		return stream;
	}

	coord WarpLocation(const grid<LocationData>& grizzy, const coord& pos, TravelDirection dir)
	{
		switch (dir)
		{
		case TravelDirection::W:
		{
			auto iter = prev(find_if(grizzy[pos.second].crbegin(), grizzy[pos.second].crend(), [](const LocationData& loc) { return !loc.bIsWall; }).base());
			auto newCoord = coord{ distance(grizzy[pos.second].cbegin(), iter), pos.second };
			return newCoord;
		}
		case TravelDirection::E:
		{
			auto iter = find_if(grizzy[pos.second].cbegin(), grizzy[pos.second].cend(), [](const LocationData& loc) { return !loc.bIsWall; });
			auto newCoord =  coord{ distance(grizzy[pos.second].cbegin(), iter), pos.second };
			return newCoord;
		}
		case TravelDirection::N:
		{
			auto iter = prev(find_if(grizzy.crbegin(), grizzy.crend(), [&](const vector<LocationData>& loc) {return !loc[pos.first].bIsWall; }).base());
			auto newCoord = coord{ pos.first, distance(grizzy.cbegin(), iter)};
			return newCoord;
		}
		case TravelDirection::S:
		{
			auto iter = find_if(grizzy.cbegin(), grizzy.cend(), [&](const vector<LocationData>& loc) {return !loc[pos.first].bIsWall; });
			auto newCoord = coord{ pos.first, distance(grizzy.cbegin(), iter)};
			return newCoord;
		}
		default:
			return pos;
		}
	}

	// Moves a blizzard on the grid by one movement.
	void MoveBlizzard(grid<LocationData>& grizzy, Blizzard& blizzy, size_t blindex)
	{
		coord targetLocation = blizzy.Position;
		switch (blizzy.Dir) {
		case TravelDirection::N: targetLocation = coord{blizzy.Position.first    , blizzy.Position.second - 1}; break;
		case TravelDirection::S: targetLocation = coord{blizzy.Position.first    , blizzy.Position.second + 1}; break;
		case TravelDirection::W: targetLocation = coord{blizzy.Position.first - 1, blizzy.Position.second    }; break;
		case TravelDirection::E: targetLocation = coord{blizzy.Position.first + 1, blizzy.Position.second    }; break;
		}

		// NOTE: While theoretically it would be possible for a blizzard to overlap with the begin/end-positions,
		// but the input doesn't appear to have any blizzards that would ever do that.
		if(grizzy[targetLocation.second][targetLocation.first].bIsWall)
		{
			targetLocation = WarpLocation(grizzy, targetLocation, blizzy.Dir);
		}

		erase(grizzy[blizzy.Position.second][blizzy.Position.first].blizzardsAtThisLocation, blindex);
		grizzy[targetLocation.second][targetLocation.first].blizzardsAtThisLocation.push_back(blindex);
		blizzy.Position = targetLocation;
	}

	bool IsOccupied(const grid<LocationData>& grizzy, const coord& cizzy)
	{
		return cizzy.first < 0 || cizzy.second < 0 || grizzy[cizzy.second][cizzy.first].bIsWall
		|| grizzy[cizzy.second][cizzy.first].blizzardsAtThisLocation.size() > 0;
	}

	size_t ProcessTravel(const grid<LocationData>& grizzy, const vector<Blizzard>& blizzy, const coord& elfPos, const coord& endPos, size_t recurseLayer = 0, size_t knownBest = 0)
	{
		if(elfPos == endPos)
		{
			return recurseLayer;
		}
		if(knownBest > 0 && recurseLayer > knownBest)
		{
			return 0;
		}
		if(recurseLayer > 10000)
		{
			return 0;
		}
		// First: Get a duplicate map which will be the next state
		auto nextGrizzy = grizzy;
		auto nextBlizzy = blizzy;

		// Move all the blizzards on the new grid
		for (size_t i = 0; i < nextBlizzy.size(); ++i) {
			MoveBlizzard(nextGrizzy, nextBlizzy[i], i);
		}

		// Priorities:
		// * In the general direction of the end goal
		// * Wait
		// * Away from
		std::vector<coord> targetCandidates{
			elfPos + coord{1,0}
			, elfPos + coord{0,1}
			, elfPos
			, elfPos + coord{-1,0}
			, elfPos + coord{0,-1}
		};
		erase_if(targetCandidates, [&](const coord& c){return IsOccupied(nextGrizzy, c);});
		
		size_t bestCandidateSize = knownBest;
		for (const auto& c : targetCandidates) {
			auto candidateSize = ProcessTravel(nextGrizzy, nextBlizzy, c, endPos, recurseLayer + 1, bestCandidateSize);
			if(candidateSize != 0)
			{
				if(bestCandidateSize != 0)
				{
					bestCandidateSize = min(candidateSize, bestCandidateSize);
				}
				else
				{
					bestCandidateSize = candidateSize;
				}
			}
		}

		return bestCandidateSize;
	}
}

using namespace Day24Internals;

std::wstring Day24::GetResultStr()
{
	wstringstream resultStream;

	grid<LocationData> daGriddy{};
	vector<Blizzard> blizzards{};
	coord elfPos = {};
	coord endPos = {};

	const auto reset = [&]
	{
		daGriddy = {};
		blizzards = {};
		elfPos = {};
		endPos = {};
	};
	const auto printGrid = [&]
	{
		for (const auto& vec : daGriddy) {
			for (const LocationData& loc : vec) {
				if(loc.bIsWall)
				{
					std::wcout << L'#';
				}
				else
				{
					if(loc.blizzardsAtThisLocation.empty())
					{
						std::wcout << L'.';
					}
					else if (loc.blizzardsAtThisLocation.size() > 1) {
						std::wcout << L'0' + min(loc.blizzardsAtThisLocation.size(), size_t{9});
					}
					else {
						switch(blizzards[loc.blizzardsAtThisLocation[0]].Dir)
						{
							case TravelDirection::N: wcout << L'^'; break;
							case TravelDirection::S: wcout << L'v'; break;
							case TravelDirection::W: wcout << L'<'; break;
							case TravelDirection::E: wcout << L'>'; break;
							default: wcout << L'o';
						}
					}
				}
			}
			std::wcout << L"\n";
		}
	};

	const auto parseLine = [&](const wstring_view& line)
	{
		daGriddy.emplace_back();
		for (const wchar_t& c : line)
		{
			LocationData newData{};
			coord blizzardCoord{daGriddy.back().size(), daGriddy.size() - 1};
			switch (c) {
			case L'#':
				newData.bIsWall = true;
				break;
			case L'>':
				newData.blizzardsAtThisLocation.push_back(blizzards.size());
				blizzards.emplace_back(blizzardCoord, TravelDirection::E);
				break;
			case L'<':
				newData.blizzardsAtThisLocation.push_back(blizzards.size());
				blizzards.emplace_back(blizzardCoord, TravelDirection::W);
				break;
			case L'^':
				newData.blizzardsAtThisLocation.push_back(blizzards.size());
				blizzards.emplace_back(blizzardCoord, TravelDirection::N);
				break;
			case L'v':
				newData.blizzardsAtThisLocation.push_back(blizzards.size());
				blizzards.emplace_back(blizzardCoord, TravelDirection::S);
				break;
			}

			daGriddy.back().push_back(newData);
		}
	};

	const auto finalize = [&]
	{
		// Place the first elf
		for (size_t i = 0; i < daGriddy.size(); ++i) {
			auto& vec = daGriddy[i];
			auto iter = ranges::find_if(vec, [](const LocationData& loc){ return loc.bIsWall == false && loc.blizzardsAtThisLocation.empty();});
			if(iter != vec.cend())
			{
				elfPos = {distance(vec.begin(), iter), i};
				break;
			}
		}
		for (size_t i = 0; i < daGriddy.size(); ++i) {
			auto realI = daGriddy.size() - (i + 1);
			auto& vec = daGriddy[daGriddy.size() - (i + 1)];
			auto iter = find_if(vec.crbegin(), vec.crend(), [](const LocationData& loc){ return loc.bIsWall == false && loc.blizzardsAtThisLocation.empty();});
			if(iter != vec.crend())
			{
				endPos = {distance(vec.cbegin(), prev(iter.base())), realI};
				break;
			}
		}
	};

	const auto processMoves = [&]
	{
		return ProcessTravel(daGriddy, blizzards, elfPos, endPos);
	};


	const auto output = [&](size_t result, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Best I can do is " << result << L"\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine, 0);
	finalize();
	printGrid();
	size_t result = processMoves();
	output(result, L"Test 0");

	reset();
	ForEachLineInTestInputFile(parseLine, 1);
	finalize();
	printGrid();
	result = processMoves();
	if(result != 18) __debugbreak();
	output(result, L"Test 1");

	return resultStream.str();
};
std::wstring Day24::GetIdStr()
{
	return L"Day24";
};
