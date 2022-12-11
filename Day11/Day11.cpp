#include "Day11.h"

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

using WorryType = uint64_t;

// 16*16 % 15 = ((15+1) * 16) % 15 = (15*16 + 1*16) % 15 = ((15*16)%15) + 16%15
// 16*14 % 15 = ((15+1) * 14) % 15 = (15*14 + 1*14) % 15 = ((15*16)%15) + 14%15
// 16*17 % 15 = ((15+1) * 17) % 15 = (15*17 + 1*17) % 15 = ((15*16)%15) + 17%15
// 31*17 % 15 = ((30+1) * 17) % 15 = (30*17 + 1*17) % 15 = ((15*16)%15) + 17%15
// 32*17 % 15 = ((30+1) * 17) % 15 = (30*17 + 1*17) % 15 = ((15*16)%15) + 17%15
// 1*17 % 15 = 17 % 15

class ModuloHelper
{
public:
	ModuloHelper(WorryType value, WorryType base)
		: _base{ base }
		, _value{ value % base }
	{}
	~ModuloHelper() = default;
	ModuloHelper(const ModuloHelper&) = default;
	ModuloHelper& operator=(const ModuloHelper&) = default;
	ModuloHelper(ModuloHelper&&) = default;
	ModuloHelper& operator=(ModuloHelper&&) = default;

	ModuloHelper operator+(WorryType num)
	{
		return ModuloHelper{ _value + num, _base };
	}
	ModuloHelper operator*(WorryType num)
	{
		return ModuloHelper{ _value * num, _base };
	}

	operator WorryType() const
	{
		return _value;
	}

private:
	ModuloHelper() = delete;
	WorryType _base;
	WorryType _value;
};


namespace Day11Internal
{
	enum class MathOp
	{
		Add,
		Sub,
		Mul,
		Div,
		Mod,
	};

	auto HandleOp(auto x, auto y, MathOp op)
	{
		switch (op)
		{
		case MathOp::Add:
			return x + y;
		case MathOp::Mul:
			return x * y;
		default:
			return x;
		}
	}

	struct Item
	{
		WorryType WorryLevel;
		std::vector<ModuloHelper> WorryLevelsPerMonkey{};
	};

	struct Monkey
	{
		std::vector<Item> ItemsInHand{};

		std::optional<WorryType> InspectOpLhs{};
		MathOp InspectOperator{};
		std::optional<WorryType> InspectOpRhs{};

		std::optional<WorryType> TestOpRhs{};

		size_t TestSuccessTarget{};
		size_t TestFailureTarget{};

		size_t InspectionCounter{};
	};

	void ParseStartingItems(const std::wstring& substr, Monkey& target)
	{
		auto sidePair = SplitString(substr, L", ");
		do
		{
			target.ItemsInHand.push_back({ static_cast<WorryType>(std::stoi(sidePair.first)) });
			sidePair = SplitString(sidePair.second, L", ");
		} while (!sidePair.first.empty());
	}

	void ParseInspectOp(const std::wstring& substr, Monkey& target)
	{
		const std::wregex regexObj{ L"new = ([a-z0-9]+) ([\\+\\-\\*\\/]) ([a-z0-9]+)" };
		std::wsmatch match;
		if (std::regex_match(substr, match, regexObj))
		{
			if (match[1].str() != L"old")
			{
				target.InspectOpLhs = static_cast<WorryType>(std::stoi(match[1].str()));
			}

			switch (match[2].str()[0])
			{
			case L'+': target.InspectOperator = MathOp::Add; break;
			case L'-': target.InspectOperator = MathOp::Sub; break;
			case L'*': target.InspectOperator = MathOp::Mul; break;
			case L'/': target.InspectOperator = MathOp::Div; break;
			case L'%': target.InspectOperator = MathOp::Mod; break;
			}

			if (match[3].str() != L"old")
			{
				target.InspectOpRhs = static_cast<WorryType>(std::stoi(match[3].str()));
			}
		}
	}

	void ParseTestOp(const std::wstring& substr, Monkey& target)
	{
		// For the time being, assume the substr = "divisible by "
		static constexpr const std::wstring_view divStr{ L"divisible by " };
		if (substr.starts_with(divStr))
		{
			target.TestOpRhs = static_cast<WorryType>(std::stoi(substr.substr(divStr.length())));
		}
	}

	void ParseResultOp(const std::wstring& substr, Monkey& target)
	{
		static constexpr const std::wstring_view trueStr{ L"true: " };
		static constexpr const std::wstring_view falseStr{ L"false: " };
		static constexpr const std::wstring_view preamble{ L"throw to monkey " };
		if (substr.starts_with(trueStr))
		{
			target.TestSuccessTarget = static_cast<size_t>(std::stoi(substr.substr(trueStr.length() + preamble.length())));
		}
		else if (substr.starts_with(falseStr))
		{
			target.TestFailureTarget = static_cast<size_t>(std::stoi(substr.substr(falseStr.length() + preamble.length())));
		}
	}
};

std::wstring Day11::GetResultStr()
{
	using namespace Day11Internal;
	std::wstringstream resultStream;
	std::vector<Monkey> monkeys{};
	std::vector<Monkey> monkeysWorking{};

	const auto reset = [&]
	{
		monkeys = {};
		monkeysWorking = {};
	};

	const auto parseLines = [&](const std::wstring& line)
	{
		static constexpr const std::wstring_view monkeyCmd{ L"Monkey" };
		static constexpr const std::wstring_view startItemCmd{ L"  Starting items: " };
		static constexpr const std::wstring_view inspectCmd{ L"  Operation: " };
		static constexpr const std::wstring_view testCmd{ L"  Test: " };
		static constexpr const std::wstring_view ifCmd{ L"    If " };
		if (line.starts_with(monkeyCmd))
		{
			// Start a new monkey;
			monkeys.emplace_back();
		}
		else if (line.starts_with(startItemCmd))
		{
			ParseStartingItems(line.substr(startItemCmd.length()), monkeys.back());
		}
		else if (line.starts_with(inspectCmd))
		{
			ParseInspectOp(line.substr(inspectCmd.length()), monkeys.back());
		}
		else if (line.starts_with(testCmd))
		{
			ParseTestOp(line.substr(testCmd.length()), monkeys.back());
		}
		else if (line.starts_with(ifCmd))
		{
			ParseResultOp(line.substr(ifCmd.length()), monkeys.back());
		}
	};
	const auto postParse = [&](const std::wstring& debug)
	{
		const ScopeProfiler prof{ L"[" + debug + L"] Post-parse" };
		std::vector<WorryType> divisors{};
		for (auto& monkey : monkeys)
		{
			divisors.push_back(monkey.TestOpRhs.value());
		}
		for (auto& monkey : monkeys)
		{
			for (auto& item : monkey.ItemsInHand)
			{
				for (const auto& div : divisors)
				{
					item.WorryLevelsPerMonkey.push_back(ModuloHelper{ item.WorryLevel, div });
				}
			}
		}
	};
	const auto handleItemWithoutRelax = [&](Monkey& monkey, Item& item, size_t monkeyIndex, int logLevel = 0)
	{
		for (auto& worryLevel: item.WorryLevelsPerMonkey)
		{
			const auto inspectWorry = HandleOp(
				  worryLevel
				, monkey.InspectOpRhs.value_or(worryLevel)
				, monkey.InspectOperator
			);
			if (logLevel >= 3) std::wcout << L"    Old value = " << worryLevel << "\n";
			if (logLevel >= 3) std::wcout << L"    New value = " << inspectWorry << "\n";

			worryLevel = inspectWorry;
		}

		++monkey.InspectionCounter;

		const auto testOpRhs = monkey.TestOpRhs.value();
		const bool evalResult = WorryType(item.WorryLevelsPerMonkey[monkeyIndex]) == 0;
		if (logLevel >= 3) std::wcout << L"      Current item is" << (evalResult ? L" " : L" not ") << "divisible by " << testOpRhs << "\n";

		const auto newTarget = evalResult ? monkey.TestSuccessTarget : monkey.TestFailureTarget;
		monkeysWorking[newTarget].ItemsInHand.push_back(item);
		if (logLevel >= 3) std::wcout << L"      Item with worry level " << item.WorryLevel << " is thrown to monkey " << newTarget << "\n";

	};
	const auto handleItemWithRelax = [&](Monkey& monkey, Item& item, int logLevel = 0)
	{
		if(logLevel >= 3) std::wcout << L"    Handling an item with worry level " << item.WorryLevel << "\n";

		const auto oldWorry = item.WorryLevel;
		const auto inspectWorry = HandleOp(
			  monkey.InspectOpLhs.value_or(item.WorryLevel)
			, monkey.InspectOpRhs.value_or(item.WorryLevel)
			, monkey.InspectOperator
		);
		if(logLevel >= 3) std::wcout << L"      Worry changed from " << oldWorry << L" to " << inspectWorry << L" after inspection\n";

		++monkey.InspectionCounter;

		auto postInspectWorry = inspectWorry;
		postInspectWorry /= 3;
		if (logLevel >= 3) std::wcout << L"      Monkey gets bored with item and worry drops to " << postInspectWorry << L"\n";

		item.WorryLevel = postInspectWorry;

		const auto testOpRhs = monkey.TestOpRhs.value_or(item.WorryLevel);
		const bool evalResult = (item.WorryLevel % testOpRhs) == 0;
		if(logLevel >= 3) std::wcout << L"      Current item is" << (evalResult ? L" " : L" not ") << "divisible by " << testOpRhs << "\n";

		const auto newTarget = evalResult ? monkey.TestSuccessTarget : monkey.TestFailureTarget;
		monkeysWorking[newTarget].ItemsInHand.push_back(item);
		if (logLevel >= 3) std::wcout << L"      Item with worry level " << item.WorryLevel << " is thrown to monkey " << newTarget << "\n";

	};
	const auto handleMonkey = [&](Monkey& monkey, size_t monkeyIndex, bool bWithPostInspectRelax, int logLevel = 0)
	{
		if (logLevel >= 3) std::wcout << L"  Monkey " << monkeyIndex << L":\n";
		for (auto& item : monkey.ItemsInHand)
		{
			if (bWithPostInspectRelax)
			{
				handleItemWithRelax(monkey, item, logLevel);
			}
			else
			{
				handleItemWithoutRelax(monkey, item, monkeyIndex, logLevel);
			}
		}
		monkey.ItemsInHand.clear();
	};
	const auto handleRound = [&](size_t roundIndex, bool bWithPostInspectRelax, int logLevel = 0)
	{
		if (logLevel >= 2) std::wcout << L"Round " << roundIndex+1 << L":\n";
		for (size_t i = 0; i < monkeysWorking.size(); i++)
		{
			handleMonkey(monkeysWorking[i], i, bWithPostInspectRelax, logLevel);
		}
		if (logLevel >= 3) std::wcout << L"\n";
		if (logLevel >= 2) std::wcout << L"After round " << roundIndex+1 << L", monkeys are holding items with following worry levels:\n";
		for (size_t i = 0; i < monkeysWorking.size(); ++i)
		{
			if (logLevel >= 2) std::wcout << L"  Monkey " << i << L" (inspected " << std::setw(6) << monkeysWorking[i].InspectionCounter << " times): ";
			bool bEncounteredFirst = false;
			for (const auto& item : monkeysWorking[i].ItemsInHand)
			{
				if (bEncounteredFirst)
				{
					if(logLevel >= 2) std::wcout << ", ";
				}
				if(logLevel >= 2) std::wcout << item.WorryLevel;
				bEncounteredFirst = true;
			}
			if(logLevel >= 2) std::wcout << "\n";
		}
		if(logLevel >= 2) std::wcout << "\n";
	};
	const auto handle = [&](const std::wstring& debug, size_t numRounds, bool bWithPostInspectRelax)
	{
		monkeysWorking = monkeys;
		const ScopeProfiler prof{ L"[" + debug + L"] Handle all monkey rounds"};
		for (size_t i = 0; i < numRounds; i++)
		{
			const bool bShouldLog = i == 0 || i == (numRounds - 1) || ((i % 1000) == 999) || i == 19;
			handleRound(i, bWithPostInspectRelax, 0);
			//handleRound(i, bWithPostInspectRelax, 3);
		}
		std::ranges::sort(monkeysWorking, [](const Monkey& a, const Monkey& b) {return a.InspectionCounter > b.InspectionCounter; });
	};

	const auto monkeyBusinessLevel = [&]
	{
		return monkeysWorking[0].InspectionCounter * monkeysWorking[1].InspectionCounter;
	};

	const auto output = [&](const std::wstring& debug)
	{
		const auto level = monkeyBusinessLevel();
		resultStream << "[" << debug << "] Monkey business level = " << level << "\n";
		return level;
	};

	reset();
	ForEachLineInTestInputFile(parseLines);
	postParse(L"Test");
	handle(L"Test (relaxed, 20 rounds)", 20, true);
	const auto testLevel = output(L"Test (relaxed, 20 rounds)");
	if (testLevel != 10605) __debugbreak();
	monkeysWorking = {};
	handle(L"Test (stressed, 10000 rounds)", 10000, false);
	const auto testLevel2 = output(L"Test (stressed, 10000 rounds)");
	if (testLevel2 != 2713310158) __debugbreak();

	reset();
	ForEachLineInInputFile(parseLines);
	postParse(L"Realsies");
	handle(L"Realsies", 20, true);
	const auto realLevel = output(L"Realsies");
	if (realLevel != 98280) __debugbreak();
	monkeysWorking = {};
	handle(L"Realsies (stressed, 10000 rounds)", 10000, false);
	const auto realLevel2 = output(L"Realsies (stressed, 10000 rounds)");
	if (realLevel2 != 17673687232) __debugbreak();

	return resultStream.str();
};
std::wstring Day11::GetIdStr()
{
	return L"Day11";
	
};
