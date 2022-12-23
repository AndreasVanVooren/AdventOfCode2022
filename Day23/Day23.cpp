#include "Day23.h"
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

namespace Day23Internals
{
	enum class Direction
	{
		N,
		S,
		W,
		E,
	};

	struct Elf
	{
		int Index;
		coord Position{};
	};

	bool CanMoveInDir(std::array<bool, 9> emptySpaceGrid, Direction dir)
	{
		switch (dir)
		{
		case Direction::N:
			return emptySpaceGrid[0] && emptySpaceGrid[1] && emptySpaceGrid[2];
		case Direction::S:
			return emptySpaceGrid[6] && emptySpaceGrid[7] && emptySpaceGrid[8];
		case Direction::W:
			return emptySpaceGrid[0] && emptySpaceGrid[3] && emptySpaceGrid[6];
		case Direction::E:
			return emptySpaceGrid[2] && emptySpaceGrid[5] && emptySpaceGrid[8];
		}
		return false;
	}
	coord GetOffset(const coord& c, Direction dir)
	{
		switch (dir)
		{
		case Direction::N:
			return {c.first, c.second - 1};
		case Direction::S:
			return {c.first, c.second + 1};
		case Direction::W:
			return {c.first - 1, c.second};
		case Direction::E:
			return {c.first + 1, c.second};
		default:
			return c;
		}
	}
}

using namespace Day23Internals;

std::wstring Day23::GetResultStr()
{
	wstringstream resultStream{};
	grid<int> daGriddy{};
	vector<Elf> elves{};

	const auto reset = [&] { daGriddy = {}; elves = {}; };
	const auto parseLine = [&](const wstring_view& line)
	{
		if (line.empty()) return;

		daGriddy.emplace_back();
		for (const auto& c : line)
		{
			if (c == L'.')
			{
				daGriddy.back().push_back(-1);
			}
			else if(c == L'#')
			{
				daGriddy.back().push_back(static_cast<int>(elves.size()));
				elves.emplace_back(
					static_cast<int>(elves.size())
					, coord{ daGriddy.back().size() - 1, daGriddy.size() - 1 }
				);
			}
		}
	};

	const auto printGrid = [&]
	{
		size_t numElves = 0;
		for (const auto& vec : daGriddy)
		{
			for (const auto& elem : vec)
			{
				if (elem != -1)
				{
					++numElves;
				}
				std::wcout << ((elem != -1) ? L"#" : L".");
			}
			std::wcout << L"\n";
		}
		std::wcout << L"Num elves: " << numElves << "\n";
	};

	const auto addCoordToGrid = [&](const coord& c, int value)
	{
		coord tlCoord{ 0,0 };
		coord brCoord{ daGriddy.size() > 0 ? (daGriddy[0].size() - 1) : -1, (daGriddy.size() - 1) };
		if (c.second < tlCoord.second)
		{
			coordPoint additionalItems = tlCoord.second - c.second;
			coordPoint horizontalItems = brCoord.first + 1 - tlCoord.first;
			daGriddy.insert(daGriddy.begin(), additionalItems, std::vector<int>(horizontalItems, -1));
			tlCoord.second = c.second;
		}
		if (c.second > brCoord.second)
		{
			coordPoint additionalItems = c.second - brCoord.second;
			coordPoint horizontalItems = brCoord.first + 1 - tlCoord.first;
			daGriddy.insert(daGriddy.end(), additionalItems, std::vector<int>(horizontalItems, -1));
			brCoord.second = c.second;
		}

		if (c.first < tlCoord.first)
		{
			coordPoint additionalItems = tlCoord.first - c.first;
			for (size_t i = 0; i < daGriddy.size(); ++i)
			{
				auto& vec = daGriddy[i];
				vec.insert(vec.begin(), additionalItems, -1);
			}
			tlCoord.first = c.first;
		}
		if (c.first > brCoord.first)
		{
			coordPoint additionalItems = c.first - brCoord.first;
			for (size_t i = 0; i < daGriddy.size(); ++i)
			{
				auto& vec = daGriddy[i];
				vec.insert(vec.end(), additionalItems, -1);
			}
			brCoord.first = c.first;
		}

		const coord newOffset = coord{0, 0} - tlCoord;
		const coord newCoord = c + newOffset;
		daGriddy[newCoord.second][newCoord.first] = value;
		for (auto& elf : elves)
		{
			elf.Position += newOffset;
		}

		return newCoord;
	};

	const auto trimGrid = [&]
	{
		// Trim grid.
		auto eraseStart = find_if(daGriddy.crbegin(), daGriddy.crend(), [](const vector<int>& row) {return ranges::any_of(row, [](const int& i) {return i != -1; }); });
		daGriddy.erase(eraseStart.base(), daGriddy.cend());
		auto eraseEnd = ranges::find_if(daGriddy, [](const vector<int>& row) {return ranges::any_of(row, [](const int& i) {return i != -1; }); });
		auto yOffset = distance(daGriddy.begin(), eraseEnd);
		daGriddy.erase(daGriddy.cbegin(), eraseEnd);
		optional<size_t> eraseStartIndex{};
		optional<size_t> eraseEndIndex{};
		for (size_t i = 0; i < daGriddy[0].size() && (!eraseStartIndex.has_value() || !eraseEndIndex.has_value()); i++)
		{
			size_t reverseIndex = daGriddy[0].size() - 1 - i;

			const bool bFoundNonEmptyAtStart = !eraseEndIndex.has_value() && ranges::any_of(daGriddy, [&](const vector<int>& row) {return row[i] != -1; });
			const bool bFoundNonEmptyAtEnd = !eraseStartIndex.has_value() && ranges::any_of(daGriddy, [&](const vector<int>& row) {return row[reverseIndex] != -1; });
			if (bFoundNonEmptyAtStart)
			{
				eraseEndIndex = i;
			}
			if (bFoundNonEmptyAtEnd)
			{
				eraseStartIndex = reverseIndex + 1;
			}
		}
		if (eraseStartIndex)
		{
			for (vector<int>& vec : daGriddy)
			{
				vec.erase(next(vec.cbegin(), eraseStartIndex.value()), vec.cend());
			}
		}
		if (eraseEndIndex)
		{
			for (auto& vec : daGriddy)
			{
				vec.erase(vec.cbegin(), next(vec.cbegin(), eraseEndIndex.value()));
			}
		}
		auto xOffset = eraseEndIndex.value_or(0);
		const coord offset{ xOffset, yOffset };
		for (Elf& elf : elves)
		{
			elf.Position -= offset;
		}
	};

	const auto getEmpty = [&]
	{
		return accumulate(
			daGriddy.begin()
			, daGriddy.end()
			, size_t{ 0 }
			, [](size_t init, const vector<int>& row)
			{
				return init + ranges::count(row, -1);
			}
		);
	};

	const auto processMovement = [&](size_t moveCount = 10)
	{
		Direction order[4]{ Direction::N, Direction::S, Direction::W, Direction::E };
		size_t m = 0;
		optional<size_t> numEmptyAt{};
		for (; ; m++)
		{
			struct QueuedMove
			{
				int elfThatMoves;
				coord targetLocation;
			};
			vector<QueuedMove> queuedMoves;
			for (auto& elf : elves)
			{
				const auto& colIndex = elf.Position.first;
				const auto& rowIndex = elf.Position.second;
				std::array<bool, 9> bIsEmpty{};
				bIsEmpty[4] = true;
				for (coordPoint kernelRow = rowIndex - 1; kernelRow <= rowIndex + 1; ++kernelRow)
				{
					for (coordPoint kernelCol = colIndex - 1; kernelCol <= colIndex + 1; ++kernelCol)
					{
						if (kernelRow == rowIndex && kernelCol == colIndex
							|| kernelRow < 0 || kernelRow >= daGriddy.size()
							|| kernelCol < 0 || kernelCol >= daGriddy[rowIndex].size()
							|| daGriddy[kernelRow][kernelCol] == -1)
						{
							const auto index = (((kernelRow - rowIndex) + 1) * 3) + ((kernelCol - colIndex) + 1);
							bIsEmpty[index] = true;
						}
					}
				}
				if (ranges::all_of(bIsEmpty, [](const bool& b) {return b; }))
				{
					continue;
				}

				for (Direction dir : order)
				{
					if (CanMoveInDir(bIsEmpty, dir))
					{
						queuedMoves.emplace_back(elf.Index, GetOffset(elf.Position, dir));
						break;
					}
				}
			}

			// No elf moves, next turn no elf will move either.
			if (queuedMoves.empty())
			{
				++m;
				break;
			}

			// Remove duplicates, the complex way
			auto lastIter = queuedMoves.end();
			for (auto iter = queuedMoves.begin(); iter != lastIter;)
			{
				bool bDupesFound = false;
				for (auto subIter = next(iter); subIter != lastIter;)
				{
					if (subIter->targetLocation == iter->targetLocation)
					{
						bDupesFound = true;
						if (subIter != prev(lastIter))
						{
							swap(*subIter, *prev(lastIter));
						}
						--lastIter;
						continue;
					}
					++subIter;
				}
				if (bDupesFound)
				{
					if (iter != prev(lastIter))
					{
						swap(*iter, *prev(lastIter));
					}
					--lastIter;
					continue;
				}
				++iter;
			}
			queuedMoves.erase(lastIter, queuedMoves.end());

			if (ranges::any_of(queuedMoves, [](const QueuedMove& move) {return move.targetLocation.second < 0; }))
			{
				addCoordToGrid(coord{ 0, -1 }, -1);
				for (QueuedMove& move : queuedMoves)
				{
					move.targetLocation += {0, 1};
				}
			}
			if (ranges::any_of(queuedMoves, [](const QueuedMove& move) {return move.targetLocation.first < 0; }))
			{
				addCoordToGrid(coord{ -1, 0 }, -1);
				for (QueuedMove& move : queuedMoves)
				{
					move.targetLocation += {1, 0};
				}
			}

			for (const QueuedMove& move : queuedMoves)
			{
				Elf& elf = elves[move.elfThatMoves];
				daGriddy[elf.Position.second][elf.Position.first] = -1;
				addCoordToGrid(move.targetLocation, elf.Index);
				elf.Position = move.targetLocation;
			}

			// Rotate, but poorly made
			for (size_t i = 0; i < 3; i++)
			{
				swap(order[i], order[i + 1]);
			}

			//printGrid();

			if (m == moveCount - 1)
			{
				trimGrid();
				numEmptyAt = getEmpty();
			}
		}

		if (!numEmptyAt.has_value())
		{
			trimGrid();
			numEmptyAt = getEmpty();
		}

		return tie( m, numEmptyAt.value() );
	};

	const auto output = [&](auto result, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Covering " << get<1>(result) << " tiles, " << get<0>(result) << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine, 0);
	printGrid();
	auto moveList = processMovement();
	output(moveList, L"Test 0");

	reset();
	ForEachLineInTestInputFile(parseLine, 1);
	printGrid();
	moveList = processMovement();
	if (get<1>(moveList) != 110) __debugbreak();
	if (get<0>(moveList) != 20) __debugbreak();
	output(moveList, L"Test 1");

	reset();
	ForEachLineInTestInputFile(parseLine, 2);
	printGrid();
	moveList = processMovement();
	if (get<1>(moveList) != 812) __debugbreak();
	output(moveList, L"Test 2");

	reset();
	ForEachLineInTestInputFile(parseLine, 3);
	printGrid();
	moveList = processMovement();
	if (get<1>(moveList) != 1062) __debugbreak();
	output(moveList, L"Test 3");

	reset();
	ForEachLineInInputFile(parseLine);
	moveList = processMovement();
	if (get<1>(moveList) >= 3875) __debugbreak();
	output(moveList, L"Final");

	return resultStream.str();
};

std::wstring Day23::GetIdStr()
{
	return L"Day23";
};
