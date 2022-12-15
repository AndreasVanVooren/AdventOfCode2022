#include "Day14.h"
#include "../Utils.h"
#include "../ScopeProfiler.h"

using coord = std::pair<int, int>;
namespace Day14Internal
{
	static constexpr coord ORIGIN_POINT{ 500, 0 };
	static constexpr coord DEFAULT_TL{ 500, 0 };
	static constexpr coord DEFAULT_BR{ 500, 0 };

	enum class Material : uint8_t
	{
		Air,
		Rock,
		Sand,
	};

	static constexpr coord ORIGIN_LIST[] = {
		ORIGIN_POINT
	};
}

using namespace Day14Internal;

std::wstring Day14::GetResultStr()
{
	std::wstringstream resultStream;
	coord tlCoord = DEFAULT_TL;
	coord brCoord = DEFAULT_BR;
	coord abyssPoint = DEFAULT_BR;
	coord shiftedOrigin = ORIGIN_POINT;
	std::vector<std::vector<Material>> matGrid;
	std::pair<size_t, size_t> grainsOfSand;

	const auto reset = [&]
	{
		matGrid = {};
		tlCoord = DEFAULT_TL;
		brCoord = DEFAULT_BR;
		abyssPoint = DEFAULT_BR;
		shiftedOrigin = ORIGIN_POINT;
		grainsOfSand = {};

		matGrid.resize(brCoord.second + 1 - tlCoord.second);
		for (auto& vec : matGrid)
		{
			vec.resize(brCoord.first + 1 - tlCoord.first);
		}
	};

	const auto addKnownCoordToGrid = [&](const coord& c, bool bWithFloor = true)
	{
		if (c.second < tlCoord.second)
		{
			int additionalItems = tlCoord.second - c.second;
			int horizontalItems = brCoord.first + 1 - tlCoord.first;
			matGrid.insert(matGrid.begin(), additionalItems, std::vector<Material>(horizontalItems, Material::Air));
			tlCoord.second = c.second;
		}
		if (c.second > brCoord.second)
		{
			int additionalItems = c.second - brCoord.second;
			int horizontalItems = brCoord.first + 1 - tlCoord.first;
			if (bWithFloor)
			{
				matGrid.insert(matGrid.end(), additionalItems - 1, std::vector<Material>(horizontalItems, Material::Air));
				matGrid.insert(matGrid.end(), 1, std::vector<Material>(horizontalItems, Material::Rock));
			}
			else
			{
				matGrid.insert(matGrid.end(), additionalItems, std::vector<Material>(horizontalItems, Material::Air));
			}
			brCoord.second = c.second;
		}

		if (c.first < tlCoord.first)
		{
			int additionalItems = tlCoord.first - c.first;
			for (size_t i = 0; i < matGrid.size(); ++i)
			{
				auto& vec = matGrid[i];
				const Material fill = (bWithFloor && i == matGrid.size() - 1) ? Material::Rock : Material::Air;
				vec.insert(vec.begin(), additionalItems, fill);
			}
			tlCoord.first = c.first;
		}
		if (c.first > brCoord.first)
		{
			int additionalItems = c.first - brCoord.first;
			for (size_t i = 0; i < matGrid.size(); ++i)
			{
				auto& vec = matGrid[i];
				const Material fill = (bWithFloor && i == matGrid.size() - 1) ? Material::Rock : Material::Air;
				vec.insert(vec.end(), additionalItems, fill);
			}
			brCoord.first = c.first;
		}
	};

	const auto isBeyondHypotheticalAbyss = [&](const coord& c)
	{
		return c.second > abyssPoint.second;
	};
	const auto isBeyondVeryRealAbyss = [&](const coord& c)
	{
		return c.second > brCoord.second;
	};
	const auto isCoordAir = [&](const coord& c)
	{
		if (c.second < 0 || c.second >= matGrid.size() || c.first < 0 || c.first >= matGrid[c.second].size())
		{
			return true;
		}
		return matGrid[c.second][c.first] == Material::Air;
	};

	const auto putUnitOnGrid = [&](const coord& c, Material mat)
	{
		matGrid[c.second][c.first] = mat;
	};

	const auto addPathToGrid = [&](const std::vector<coord> pathCoords, Material mat = Material::Rock)
	{
		for (const auto& c : pathCoords)
		{
			addKnownCoordToGrid(c, false);
		}
		// Handle segment
		for (size_t i = 0; i < pathCoords.size() - 1; ++i)
		{
			const bool bXDifferent = pathCoords[i].first != pathCoords[i + 1].first;
			const bool bYDifferent = pathCoords[i].second != pathCoords[i + 1].second;
			if (bXDifferent && bYDifferent) __debugbreak();
			const auto& identicalSegment = bXDifferent ? pathCoords[i].second : pathCoords[i].first;
			const auto& lhsDifferentSegment = bXDifferent ? pathCoords[i].first  : pathCoords[i].second;
			const auto& rhsDifferentSegment = bXDifferent ? pathCoords[i + 1].first  : pathCoords[i + 1].second;
			const auto& largeDifferentSegment = rhsDifferentSegment > lhsDifferentSegment ? rhsDifferentSegment : lhsDifferentSegment;
			const auto& smallDifferentSegment = rhsDifferentSegment > lhsDifferentSegment ? lhsDifferentSegment : rhsDifferentSegment;
			for (int j = 0; j <= largeDifferentSegment - smallDifferentSegment; ++j)
			{
				const auto segmentOffset = smallDifferentSegment + j;
				putUnitOnGrid(std::make_pair(
					  (bXDifferent ? segmentOffset : identicalSegment) - tlCoord.first
					, (bXDifferent ? identicalSegment : segmentOffset) - tlCoord.second
				), mat);
			}
		}
	};

	const auto finalizeGrid = [&]
	{
		abyssPoint = brCoord;
		const coord floorPointL{ ORIGIN_POINT.first - (brCoord.second + 2), brCoord.second + 2 };
		const coord floorPointR{ ORIGIN_POINT.first + (brCoord.second + 2), brCoord.second + 2 };
		addKnownCoordToGrid(floorPointL);
		addKnownCoordToGrid(floorPointR);
		shiftedOrigin = { ORIGIN_POINT.first - tlCoord.first, ORIGIN_POINT.second - tlCoord.second };
		brCoord = { brCoord.first - tlCoord.first, brCoord.second - tlCoord.second };
		abyssPoint = { abyssPoint.first - tlCoord.first, abyssPoint.second - tlCoord.second };
	};

	const auto parseLine = [&](const std::wstring_view& line)
	{
		std::vector<coord> path;
		
		std::pair<std::wstring, std::wstring> pair = SplitString(line, L" -> ");
		while (!pair.first.empty())
		{
			const auto [xStr, yStr] = SplitString(pair.first, L",");
			path.push_back(std::make_pair(std::stoi(xStr), std::stoi(yStr)));
			pair = SplitString(pair.second, L" -> ");
		}
		addPathToGrid(path);
	};

	const auto printGrid = [&]
	{
		// TODO: border
		for (const auto& vec : matGrid)
		{
			for (const auto& mat : vec)
			{
				switch (mat)
				{
				case Material::Air: std::wcout << L" "; break;
				case Material::Rock: std::wcout << L"#"; break;
				case Material::Sand: std::wcout << L"o"; break;
				}
			}
			std::wcout << L"\n";
		}
	};
	
	const auto getSandResults = [&]() -> std::pair<size_t, size_t>
	{
		ScopeProfiler prof{ L"Sand results" };
		//printGrid();
		size_t abyssResult = 0;
		size_t fillResult = 0;
		bool bHitAbyss = false;
		while (isCoordAir(shiftedOrigin))
		{
			coord activeGrain = shiftedOrigin;
			putUnitOnGrid(activeGrain, Material::Sand);
			bool bRest = false;
			while (!bRest)
			{
				const coord down = { activeGrain.first, activeGrain.second + 1 };
				const coord downLeft = { activeGrain.first - 1, activeGrain.second + 1 };
				const coord downRight = { activeGrain.first + 1, activeGrain.second + 1 };

				if (isBeyondHypotheticalAbyss(down))
				{
					bHitAbyss = true;
				}

				if (isBeyondVeryRealAbyss(down))
				{
					bHitAbyss = true;
					break;
				}
				else if (isCoordAir(down))
				{
					putUnitOnGrid(activeGrain, Material::Air);
					activeGrain = down;
					putUnitOnGrid(down, Material::Sand);
				}
				else if (isCoordAir(downLeft))
				{
					putUnitOnGrid(activeGrain, Material::Air);
					activeGrain = downLeft;
					putUnitOnGrid(downLeft, Material::Sand);
				}
				else if (isCoordAir(downRight))
				{
					putUnitOnGrid(activeGrain, Material::Air);
					activeGrain = downRight;
					putUnitOnGrid(downRight, Material::Sand);
				}
				else
				{
					// Rest case
					if (!bHitAbyss)
					{
						++abyssResult;
					}
					++fillResult;
					bRest = true;
				}
			}
		}

		//printGrid();
		return {abyssResult, fillResult};
	};

	const auto output = [&](std::pair<size_t, size_t> grains, const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] " << grains.first << " grains of sand are off to never-neverland, although sand stops at " << grains.second << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine);
	finalizeGrid();
	grainsOfSand = getSandResults();
	if (grainsOfSand.first != 24) __debugbreak();
	if (grainsOfSand.second != 93) __debugbreak();
	output(grainsOfSand, L"TEST");

	reset();
	ForEachLineInInputFile(parseLine);
	finalizeGrid();
	grainsOfSand = getSandResults();
	if (grainsOfSand.first != 795) __debugbreak();
	output(grainsOfSand, L"OUTPUT");

	return resultStream.str();
};
std::wstring Day14::GetIdStr()
{
	return L"Day14";
};
