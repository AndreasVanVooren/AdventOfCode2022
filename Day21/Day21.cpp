#include "Day21.h"

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

namespace Day21Internal
{
	using IdType = wstring;
	using MathType = int64_t;

	enum class MathOp
	{
		Add,
		Sub,
		Mul,
		Div,
		Mod,
	};

	struct QueuedMathOp
	{
		IdType LhsId;
		MathOp Operation;
		IdType RhsId;
	};

	struct Monkey
	{
		QueuedMathOp MathOp;
		mutable std::optional<MathType> Result{};
	};

	MathType ResolveMathOps(const map<IdType, Monkey>& others, const Monkey& rootMonkey)
	{
		if (!rootMonkey.Result.has_value())
		{
			const auto lhsIter = others.find(rootMonkey.MathOp.LhsId);
			const auto rhsIter = others.find(rootMonkey.MathOp.RhsId);
			const MathType lhs = ResolveMathOps(others, lhsIter->second);
			const MathType rhs = ResolveMathOps(others, rhsIter->second);
			switch (rootMonkey.MathOp.Operation)
			{
			case MathOp::Add: rootMonkey.Result = lhs + rhs; break;
			case MathOp::Sub: rootMonkey.Result = lhs - rhs; break;
			case MathOp::Mul: rootMonkey.Result = lhs * rhs; break;
			case MathOp::Div: rootMonkey.Result = lhs / rhs; break;
			case MathOp::Mod: rootMonkey.Result = lhs % rhs; break;
			}

		}

		return rootMonkey.Result.value();
	}
	static const IdType ROOT_ID = L"root";
	static const IdType HUMAN_ID = L"humn";
}

using namespace Day21Internal;

std::wstring Day21::GetResultStr()
{
	wstringstream resultStream{};
	map<IdType, Monkey> monkeys{};

	const auto reset = [&] { monkeys = {}; };
	const auto parseLines = [&](const wstring_view& line)
	{
		auto [id, sequence] = SplitString(line, L": ");
		Monkey newMonkey{};

		if (sequence[0] >= L'0' && sequence[0] <= L'9')
		{
			newMonkey.Result = static_cast<MathType>(stoi(sequence));
		}
		else
		{
			auto [lhs, restOf] = SplitString(sequence, L" ");
			auto [operand, rhs] = SplitString(restOf, L" ");
			newMonkey.MathOp.LhsId = lhs;
			newMonkey.MathOp.RhsId = rhs;
			switch (operand[0])
			{
			case L'+': newMonkey.MathOp.Operation = MathOp::Add; break;
			case L'-': newMonkey.MathOp.Operation = MathOp::Sub; break;
			case L'*': newMonkey.MathOp.Operation = MathOp::Mul; break;
			case L'/': newMonkey.MathOp.Operation = MathOp::Div; break;
			}
		}

		monkeys.emplace(id, newMonkey);
	};
	const auto resolve = [&]
	{
		auto iter = monkeys.find(ROOT_ID);
		return ResolveMathOps(monkeys, iter->second);
	};
	const auto resolveInverse = [&]
	{
		auto rootIter = monkeys.find(ROOT_ID);
		auto humanIter = monkeys.find(HUMAN_ID);
		vector<typename decltype(monkeys)::const_iterator> iterPath{};
		auto tempIter = humanIter;
		iterPath.push_back(tempIter);
		do
		{
			// Good naming convention here bruv
			tempIter = ranges::find_if(monkeys, [&](const auto& pair) {return tempIter->first == pair.second.MathOp.LhsId || tempIter->first == pair.second.MathOp.RhsId; });
			iterPath.push_back(tempIter);
		}
		while (tempIter != rootIter);

		ranges::reverse(iterPath);

		const auto resolveForPathIndex = [&](const size_t& index)
		{
			const auto& srcIter = iterPath[index];
			const auto& nextIter = iterPath[index + 1];
			const auto& resolvedIter
				= (srcIter->second.MathOp.LhsId == nextIter->first)
				? monkeys.find(srcIter->second.MathOp.RhsId)
				: monkeys.find(srcIter->second.MathOp.LhsId);
			return ResolveMathOps(monkeys, resolvedIter->second);
		};

		MathType sumToFind = resolveForPathIndex(0);

		for (size_t i = 1; i < iterPath.size() - 1; ++i)
		{
			const MathType tempSum = resolveForPathIndex(i);
			const bool isLhs = iterPath[i]->second.MathOp.LhsId == iterPath[i + 1]->first;
			switch (iterPath[i]->second.MathOp.Operation)
			{
			// Because of commutativity, we just need to subtract tempSum.
			// if C = A + B = B + A => A = C - B or B = C - A
			case MathOp::Add: sumToFind -= tempSum; break;
			// if C = A * B = B * A => A = C / B or B = C / A
			case MathOp::Mul: sumToFind /= tempSum; break;

			// DVPT = 3
			// PTDQ = HUMN - DVPT = 2 => HUMN - 3 = 2 => isLhs -> HUMN = 2 (sumToFind) + 3 (tempSum)
			// PTDQ = DVPT - HUMN = 2 => -HUMN = PTDQ - DVPT => -HUMN = 2 - 3 => HUMN = 3 (tempSum) - 2 (sumToFind)

			// DVPT = 3
			// PTDQ = HUMN / DVPT = 2 => HUMN / 3 = 2 => isLhs -> HUMN = 2 (sumToFind) * 3 (tempSum)
			// PTDQ = DVPT / HUMN = 2 => DVPT/PTDQ = HUMN => HUMN = 3 (tempSum) / 2 (sumToFind)

			case MathOp::Sub: sumToFind = isLhs ? (sumToFind + tempSum) : (tempSum - sumToFind); break;
			case MathOp::Div: sumToFind = isLhs ? (sumToFind * tempSum) : (tempSum / sumToFind); break;
			}
		}

		return sumToFind;
	};
	const auto output = [&](MathType result, MathType result2, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Original sum = " << result << ", Human is required to say = " << result2 << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLines);
	auto sum = resolve();
	if (sum != 152) __debugbreak();
	auto sum2 = resolveInverse();
	if (sum2 != 301) __debugbreak();
	output(sum, sum2, L"Test");

	reset();
	ForEachLineInInputFile(parseLines);
	sum = resolve();
	if (sum != 75147370123646) __debugbreak();
	sum2 = resolveInverse();
	output(sum, sum2, L"Final");


	return resultStream.str();
};
std::wstring Day21::GetIdStr()
{
	return L"Day21";
};
