#include "Day08.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <ranges>
#include "../Utils.h"
#include "../ScopeProfiler.h"

std::wstring Day08::GetResultStr()
{
	std::wstringstream resultStream{};

	std::vector<std::vector<int>> treeGrid{};

	const auto reset = [&]
	{
		treeGrid = {};
	};

	const auto fetchTreeRow = [&](const std::wstring_view& line)
	{
		if (line.empty()) return;
		std::vector<int> temp{};
		for (const auto& c : line)
		{
			if (c >= L'0' && c <= L'9')
			{
				temp.push_back(c - L'0');
			}
		}
		treeGrid.push_back(std::move(temp));
	};

	const auto getStatsForTreeAt = [&](const size_t rowIndex, const size_t colIndex)
	{
		if (rowIndex == 0 || rowIndex == (treeGrid.size() - 1) || colIndex == 0 || colIndex == (treeGrid[rowIndex].size() - 1))
		{
			return std::make_pair(true, ptrdiff_t{ 0 });
		}
		//ScopeProfiler prof{ "getStatsForTreeAt" };
		const auto getTreeStatIter = [](const auto& begin, const auto& end, const auto& pred)
		{
			const auto encounter = std::find_if(begin, end, pred);
			const ptrdiff_t distance = std::distance(begin, encounter);
			const bool bVisible = distance == std::distance(begin, end);
			const ptrdiff_t scenicScore = bVisible ? distance : (distance + 1);
			return std::make_pair(bVisible, scenicScore);
		};
		const auto [bVisibleFromLeft, scenicScoreLeft] = getTreeStatIter(
			  std::prev(treeGrid[rowIndex].crend(), colIndex)
			, treeGrid[rowIndex].crend()
			, [&](const int& tree) {return treeGrid[rowIndex][colIndex] <= tree; }
		);
		const auto [bVisibleFromRight, scenicScoreRight] = getTreeStatIter(
			  std::next(treeGrid[rowIndex].cbegin(), colIndex + 1)
			, treeGrid[rowIndex].cend()
			, [&](const int& tree) {return treeGrid[rowIndex][colIndex] <= tree; }
		);
		const auto [bVisibleFromTop, scenicScoreTop] = getTreeStatIter(
			  std::prev(treeGrid.crend(), rowIndex)
			, treeGrid.crend()
			, [&](const std::vector<int>& treeRow) {return treeGrid[rowIndex][colIndex] <= treeRow[colIndex]; }
		);
		const auto [bVisibleFromBot, scenicScoreBot] = getTreeStatIter(
			  std::next(treeGrid.cbegin(), rowIndex + 1)
			, treeGrid.cend()
			, [&](const std::vector<int>& treeRow) {return treeGrid[rowIndex][colIndex] <= treeRow[colIndex]; }
		);

		const bool bIsVisible = bVisibleFromBot || bVisibleFromLeft || bVisibleFromRight || bVisibleFromTop;
		const ptrdiff_t scenicScore = scenicScoreLeft * scenicScoreRight * scenicScoreTop * scenicScoreBot;
		return std::make_pair( bIsVisible, scenicScore );
	};

	const auto getAccumulatedTreeStats = [&]
	{
		ScopeProfiler prof{ L"getAccumulatedTreeStats" };

		size_t num = 0;
		ptrdiff_t highestScenicScore = 0;
		size_t highestRow = std::numeric_limits<size_t>::max();
		size_t highestCol = std::numeric_limits<size_t>::max();
		for (size_t row = 0; row < treeGrid.size(); ++row)
		{
			for (size_t col = 0; col < treeGrid[row].size(); ++col)
			{
				const auto [visible, score] = getStatsForTreeAt(row, col);
				if (score > highestScenicScore)
				{
					highestScenicScore = score;
					highestRow = row;
					highestCol = col;
				}
				if (visible)
				{
					++num;
				}
			}
		}
		return std::make_tuple(num, highestScenicScore, highestRow, highestCol);
	};

	reset();
	ForEachLineInTestInputFile(fetchTreeRow);
	{
		const auto [numVisible, highestScenic, row, col] = getAccumulatedTreeStats();
		resultStream << L"Test input returned " << numVisible << L" trees visible, with a highest scenic score of " << highestScenic << L" at [" << col << L"," << row << L"]\n";
		if (numVisible != 21) __debugbreak();
		if (highestScenic != 8) __debugbreak();
		if (col != 2) __debugbreak();
		if (row != 3) __debugbreak();
	}

	reset();
	ForEachLineInInputFile(fetchTreeRow);
	{
		const auto [numVisible, highestScenic, row, col] = getAccumulatedTreeStats();
		resultStream << L"Input returned " << numVisible << L" trees visible, with a highest scenic score of " << highestScenic << L" at [" << col << L"," << row << L"]\n";
	}

	return resultStream.str();
};
std::wstring Day08::GetIdStr()
{
	return L"Day08";
};
