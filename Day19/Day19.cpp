#include "Day19.h"

#include <string_view>
#include <regex>
#include <numeric>
#include <unordered_set>

#include "../Utils.h"
#include "../ScopeProfiler.h"

namespace Day19Internals
{
	enum class CurrencyType
	{
		Ore,
		Clay,
		Obsidian,
		Geode,
		MAX
	};

	struct Currency
	{
		int Value[static_cast<int>(CurrencyType::MAX)]{};
	};
	struct Blueprint
	{
		Currency RobotCost[static_cast<int>(CurrencyType::MAX)]{};
	};
	struct Arsenal
	{
		size_t RobotCount[static_cast<int>(CurrencyType::MAX)]{1,0,0,0};
	};

	constexpr bool CanAfford(const Currency& wallet, const Currency& cost)
	{
		for (size_t i = 0; i < static_cast<int>(CurrencyType::MAX); i++)
		{
			if (wallet.Value[i] < cost.Value[i])
			{
				return false;
			}
		}
		return true;
	}

	constexpr Currency operator+ (const Currency& a, const Currency& b)
	{
		Currency result{};
		for (size_t i = 0; i < static_cast<int>(CurrencyType::MAX); i++)
		{
			result.Value[i] = a.Value[i] + b.Value[i];
		}
		return result;
	}
	constexpr Currency operator- (const Currency& a, const Currency& b)
	{
		Currency result{};
		for (size_t i = 0; i < static_cast<int>(CurrencyType::MAX); i++)
		{
			result.Value[i] = a.Value[i] - b.Value[i];
		}
		return result;
	}
	constexpr Currency& operator+=(Currency& a, const Currency& b)
	{
		a = a + b;
		return a;
	}
	constexpr Currency& operator-=(Currency& a, const Currency& b)
	{
		a = a - b;
		return a;
	}

	size_t GetBlueprintValue(const Blueprint& bp, size_t minutes = 24, const Currency& startWallet = {}, const Arsenal& startArsenal = {})
	{
		Currency currentWallet = startWallet;
		Arsenal currentArsenal = startArsenal;

		for (size_t min = 0; min < minutes; ++min)
		{
			bool bSpawnAtEnd[static_cast<int>(CurrencyType::MAX)]{};
			for (int robotTypeIdx = static_cast<int>(CurrencyType::MAX) - 1; robotTypeIdx >= static_cast<int>(CurrencyType::Ore); --robotTypeIdx)
			{
				if (CanAfford(currentWallet, bp.RobotCost[robotTypeIdx]))
				{
					bSpawnAtEnd[robotTypeIdx] = true;
				}
			}

			for (size_t robotTypeIdx = 0; robotTypeIdx < static_cast<int>(CurrencyType::MAX); robotTypeIdx++)
			{
				currentWallet.Value[robotTypeIdx] += currentArsenal.RobotCount[robotTypeIdx];
			}

			size_t highestBranchFound = 0;
			for (size_t robotTypeIdx = 0; robotTypeIdx < static_cast<int>(CurrencyType::MAX); robotTypeIdx++)
			{
				// Cases where we buy something.
				if (bSpawnAtEnd[robotTypeIdx])
				{
					Currency subWallet = currentWallet - bp.RobotCost[robotTypeIdx];
					Arsenal subArsenal = currentArsenal;
					++subArsenal.RobotCount[robotTypeIdx];
					highestBranchFound = std::max(highestBranchFound, GetBlueprintValue(bp, minutes - (min + 1), subWallet, subArsenal));
				}
			}
			// Case where we don't buy anything.
			if (std::ranges::any_of(bSpawnAtEnd, [](const bool& val) {return val; }))
			{
				// Return early, as we have the value from our sub-branches
				return std::max(highestBranchFound, GetBlueprintValue(bp, minutes - (min + 1), currentWallet, currentArsenal));
			}
		}

		return currentWallet.Value[static_cast<int>(CurrencyType::Geode)];
	}
}

using namespace std;
using namespace Day19Internals;

wstring Day19::GetResultStr()
{
	wstringstream resultStream;
	vector<Blueprint> blueprints;

	const auto reset = [&]()
	{
		blueprints = {};
	};

	const auto parseLines = [&](const wstring_view& line)
	{
		const std::wregex regexObj{ L"Blueprint [0-9]+: Each ore robot costs ([0-9]+) ore\\. Each clay robot costs ([0-9]+) ore\\. Each obsidian robot costs ([0-9]+) ore and ([0-9]+) clay\\. Each geode robot costs ([0-9]+) ore and ([0-9]+) obsidian\\." };
		std::wsvmatch match;

		if (regex_match(line.cbegin(), line.cend(), match, regexObj))
		{
			Blueprint bp;
			bp.RobotCost[static_cast<int>(CurrencyType::Ore)] = {stoi(match[1].str())};
			bp.RobotCost[static_cast<int>(CurrencyType::Clay)] = {stoi(match[2].str())};
			bp.RobotCost[static_cast<int>(CurrencyType::Obsidian)] = {stoi(match[3].str()), stoi(match[4].str())};
			bp.RobotCost[static_cast<int>(CurrencyType::Geode)] = {stoi(match[5].str()), 0, stoi(match[6].str())};
			blueprints.push_back(std::move(bp));
		}
	};
	const auto calcResult = [&]() -> size_t
	{
		size_t totalSize = 0;
		for (size_t i = 0; i < blueprints.size(); i++)
		{
			const ScopeProfiler prof{ L"Blueprint value for blueprint " + to_wstring(i + 1) };
			const size_t valueForBp = GetBlueprintValue(blueprints[i]);
			std::wcout << L"Blueprint " << i + 1 << L" has value of " << valueForBp << "\n";
			totalSize += valueForBp * (i + 1);
		}
		return totalSize;
	};

	const auto output = [&](size_t result, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Blueprint quality sum = " << result << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLines);
	size_t result = calcResult();
	output(result, L"Test");

	reset();
	ForEachLineInInputFile(parseLines);
	result = calcResult();
	output(result, L"Final");

	return resultStream.str();
};
wstring Day19::GetIdStr()
{
	return L"Day19";
};
