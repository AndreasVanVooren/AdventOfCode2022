#include "Day22.h"

#include <algorithm>
#include <filesystem>
#include <numeric>
#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include <map>
#include <regex>
#include <ranges>
#include "../Utils.h"
#include "../ScopeProfiler.h"

using namespace std;

namespace Day22Internals
{
	enum class Material
	{
		// The illustrious darkness that surrounds us all, lying in wait for its next prey.
		Void,
		// Place you can go
		Air,
		// Place you can not go
		Wall,
	};
	enum class Facing
	{
		Right,
		Down,
		Left,
		Up,
		MAX,
	};
	enum class TurnDirection
	{
		CW,
		CCW,
	};

	constexpr Facing TurnCCW(Facing input)
	{
		switch (input)
		{
		case Facing::Left:
			return Facing::Down;
			break;
		case Facing::Right:
			return Facing::Up;
			break;
		case Facing::Up:
			return Facing::Left;
			break;
		case Facing::Down:
			return Facing::Right;
			break;
		}
		return input;
	}
	constexpr Facing TurnCW(Facing input)
	{
		switch (input)
		{
		case Facing::Left: return Facing::Up;
		case Facing::Right: return Facing::Down;
		case Facing::Up: return Facing::Right;
		case Facing::Down: return Facing::Left;
		}
		return input;
	}
	constexpr Facing Turn(Facing input, TurnDirection turn)
	{
		switch (turn)
		{
		case TurnDirection::CW: return TurnCW(input);
		case TurnDirection::CCW: return TurnCCW(input);
		}
		return input;
	}

	struct CubeEdge
	{
		int NextFace;
		Facing NextFacing;
	};

	struct CubeRegion
	{
		coord StartPos;
		CubeEdge Edges[static_cast<int>(Facing::MAX)];
	};
}

using namespace Day22Internals;

std::wstring Day22::GetResultStr()
{
	wstringstream resultStream{};
	coord currentPos{};
	Facing currentFacing{};
	grid<Material> daGriddy{};
	size_t maxSizeFound{};
	vector<int> walks{};
	vector<TurnDirection> turns{};
	vector<CubeRegion> regions;

	const auto reset = [&]
	{
		currentPos = {};
		currentFacing = Facing::Right;
		daGriddy = {};
		maxSizeFound = {};
		walks = {};
		turns = {};
	};
	const auto parseLine = [&](const wstring_view& line)
	{
		if (line.empty()) return;

		if (line[0] >= L'0' && line[0] <= '9')
		{
			// This is the password, parse the password now.
			int currentNum = 0;
			for (const auto& c : line)
			{
				switch (c)
				{
				case L'0':
				case L'1':
				case L'2':
				case L'3':
				case L'4':
				case L'5':
				case L'6':
				case L'7':
				case L'8':
				case L'9':
					currentNum *= 10;
					currentNum += c - L'0';
					break;
				case L'L':
				case L'R':
					walks.push_back(currentNum);
					currentNum = 0;
					if (c == L'L')
					{
						turns.push_back(TurnDirection::CCW);
					}
					else
					{
						turns.push_back(TurnDirection::CW);
					}
					break;
				}
			}
			if (currentNum != 0)
			{
				walks.push_back(currentNum);
			}
		}
		else
		{
			daGriddy.emplace_back();
			maxSizeFound = max(maxSizeFound, line.length());
			daGriddy.back().reserve(maxSizeFound);
			for (const auto& c : line)
			{
				Material toAdd = Material::Void;
				switch (c)
				{
				case L'.': toAdd = Material::Air; break;
				case L'#': toAdd = Material::Wall; break;
				}
				daGriddy.back().emplace_back(toAdd);
			}
			// Fill the rest with void.
			for (size_t i = 0; i < (maxSizeFound - line.length()); i++)
			{
				daGriddy.back().emplace_back(Material::Void);
			}
		}
	};
	const auto finalize = [&](size_t cubeSize)
	{
		// Just making sure the grid is box-shaped. Otherwise I get triggered.
		for (auto& row : daGriddy)
		{
			size_t originalSize = row.size();
			if (maxSizeFound <= originalSize) continue;
			for (size_t i = 0; i < (maxSizeFound - originalSize); i++)
			{
				row.emplace_back(Material::Void);
			}
		}
		auto iter = ranges::find(daGriddy[0], Material::Air);
		currentPos = { distance(daGriddy[0].begin(), iter), 0 };

		for (size_t rowIndex = 0; rowIndex < daGriddy.size(); rowIndex += cubeSize)
		{
			for (size_t colIndex = 0; colIndex < daGriddy[rowIndex].size(); colIndex += cubeSize)
			{
				if (daGriddy[rowIndex][colIndex] != Material::Void)
				{
					regions.push_back(CubeRegion{ {colIndex, rowIndex} });
				}
			}
		}
	};
	const auto findWarpPointSimple = [&daGriddy](const coord& pos, Facing facing)
	{
		switch (facing)
		{
		case Facing::Left:
		{
			auto iter = prev(find_if(daGriddy[pos.second].crbegin(), daGriddy[pos.second].crend(), [](const Material& mat) {return mat != Material::Void; }).base());
			return coord{ distance(daGriddy[pos.second].cbegin(), iter), pos.second };
		}
		case Facing::Right:
		{
			auto iter = find_if(daGriddy[pos.second].cbegin(), daGriddy[pos.second].cend(), [](const Material& mat) {return mat != Material::Void; });
			return coord{ distance(daGriddy[pos.second].cbegin(), iter), pos.second };
		}
		case Facing::Up:
		{
			auto iter = prev(find_if(daGriddy.crbegin(), daGriddy.crend(), [&](const vector<Material>& mat) {return mat[pos.first] != Material::Void; }).base());
			return coord{ pos.first, distance(daGriddy.cbegin(), iter)};
		}
		case Facing::Down:
		{
			auto iter = find_if(daGriddy.cbegin(), daGriddy.cend(), [&](const vector<Material>& mat) {return mat[pos.first] != Material::Void; });
			return coord{ pos.first, distance(daGriddy.cbegin(), iter)};
		}
		default:
			return pos;
		}
	};
	const auto findWarpPointCubeMapped = [&daGriddy](const coord& pos, Facing facing)
	{
		switch (facing)
		{
		case Facing::Left:
		{
			auto iter = prev(find_if(daGriddy[pos.second].crbegin(), daGriddy[pos.second].crend(), [](const Material& mat) {return mat != Material::Void; }).base());
			return coord{ distance(daGriddy[pos.second].cbegin(), iter), pos.second };
		}
		case Facing::Right:
		{
			auto iter = find_if(daGriddy[pos.second].cbegin(), daGriddy[pos.second].cend(), [](const Material& mat) {return mat != Material::Void; });
			return coord{ distance(daGriddy[pos.second].cbegin(), iter), pos.second };
		}
		case Facing::Up:
		{
			auto iter = prev(find_if(daGriddy.crbegin(), daGriddy.crend(), [&](const vector<Material>& mat) {return mat[pos.first] != Material::Void; }).base());
			return coord{ pos.first, distance(daGriddy.cbegin(), iter) };
		}
		case Facing::Down:
		{
			auto iter = find_if(daGriddy.cbegin(), daGriddy.cend(), [&](const vector<Material>& mat) {return mat[pos.first] != Material::Void; });
			return coord{ pos.first, distance(daGriddy.cbegin(), iter) };
		}
		default:
			return pos;
		}
	};
	const auto walkOne = [&]()
	{
		coord targetPos = currentPos;
		switch (currentFacing)
		{
		case Facing::Left:
			targetPos = { currentPos.first - 1, currentPos.second };
			break;
		case Facing::Right:
			targetPos = { currentPos.first + 1, currentPos.second };
			break;
		case Facing::Up:
			targetPos = { currentPos.first, currentPos.second - 1 };
			break;
		case Facing::Down:
			targetPos = { currentPos.first, currentPos.second + 1 };
			break;
		}
		if (targetPos.second < 0 || targetPos.second >= daGriddy.size()
		||  targetPos.first  < 0 || targetPos.first  >= daGriddy[targetPos.second].size()
		||  daGriddy[targetPos.second][targetPos.first] == Material::Void)
		{
			targetPos = findWarpPointSimple(targetPos, currentFacing);
		}

		if (daGriddy[targetPos.second][targetPos.first] == Material::Wall)
		{
			return false;
		}
		currentPos = targetPos;
		return true;
	};
	const auto processPath = [&]() -> size_t
	{
		for (size_t i = 0; i < walks.size(); i++)
		{
			for (size_t step = 0; step < walks[i]; ++step)
			{
				if (!walkOne())
				{
					break;
				}
			}

			if (i < turns.size())
			{
				currentFacing = Turn(currentFacing, turns[i]);
			}
		}

		const auto indexedPos = currentPos + coord{1, 1};
		return (1000 * indexedPos.second) + (4 * indexedPos.first) + static_cast<coordPoint>(currentFacing);
	};
	const auto output = [&](size_t result, size_t result2, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Password = " << result << ", Password 2 = " << result2 << "\n";

	};

	reset();
	ForEachLineInTestInputFile(parseLine);
	finalize(4);
	auto result = processPath();
	//auto result2 = processPath();
	if (result != 6032) __debugbreak();
	//if (result2 != 5031) __debugbreak();
	output(result, 0, L"Test");

	reset();
	ForEachLineInInputFile(parseLine);
	finalize(50);
	result = processPath();
	output(result, 0, L"Final");

	return resultStream.str();
};
std::wstring Day22::GetIdStr()
{
	return L"Day22";
};
