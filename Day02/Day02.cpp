#include "Day02.h"

#include <vector>
#include <map>
#include <utility>
#include "../Utils.h"

namespace Day02Internal
{
	enum class RPSValue : int
	{
		Rock = 1,
		Paper = 2,
		Scissors = 3,
	};

	enum class RPSResult : int
	{
		Loss = 0,
		Draw = 3,
		Win = 6,
	};

	static const std::map<std::wstring, RPSValue> OriginalValueMapping
	{
		  {L"A", RPSValue::Rock}
		, {L"B", RPSValue::Paper}
		, {L"C", RPSValue::Scissors}
		, {L"X", RPSValue::Rock}
		, {L"Y", RPSValue::Paper}
		, {L"Z", RPSValue::Scissors}
	};

	static const std::map<std::wstring, RPSResult> ResultMapping
	{
		  {L"X", RPSResult::Loss}
		, {L"Y", RPSResult::Draw}
		, {L"Z", RPSResult::Win}
	};

	RPSResult GetResultForRHS(RPSValue lhs, RPSValue rhs)
	{
		if (lhs == rhs)
		{
			return RPSResult::Draw;
		}
		switch (lhs)
		{
		case RPSValue::Rock:
			switch (rhs)
			{
			case RPSValue::Paper:
				return RPSResult::Win;
				break;
			case RPSValue::Scissors:
				return RPSResult::Loss;
				break;
			}
			break;
		case RPSValue::Paper:
			switch (rhs)
			{
			case RPSValue::Rock:
				return RPSResult::Loss;
				break;
			case RPSValue::Scissors:
				return RPSResult::Win;
				break;
			}
			break;
		case RPSValue::Scissors:
			switch (rhs)
			{
			case RPSValue::Rock:
				return RPSResult::Win;
				break;
			case RPSValue::Paper:
				return RPSResult::Loss;
				break;
			}
			break;
		}
		return RPSResult::Draw;
	}

	RPSValue GetValueForResult(RPSValue lhs, RPSResult rhs)
	{
		switch (rhs)
		{
		case RPSResult::Loss:
			switch (lhs)
			{
			case RPSValue::Rock:
				return RPSValue::Scissors;
			case RPSValue::Paper:
				return RPSValue::Rock;
			case RPSValue::Scissors:
				return RPSValue::Paper;
			}
		case RPSResult::Draw:
			return lhs;
		case RPSResult::Win:
			switch (lhs)
			{
			case RPSValue::Rock:
				return RPSValue::Paper;
			case RPSValue::Paper:
				return RPSValue::Scissors;
			case RPSValue::Scissors:
				return RPSValue::Rock;
			}
			break;
		default:
			break;
		}
		// Dummy value
		return {};
	}
}

std::wstring Day02::GetResultStr()
{
	using namespace Day02Internal;
	std::wstringstream resultStream{};
	std::vector<int> scoresPerTurn{};
	int scoreTotal{};

	const auto initValues = [&]
	{
		scoreTotal = 0;
		scoresPerTurn = {};
	};

	const auto logResult = [&](const std::wstring& debug = {})
	{
		if (debug.empty())
		{
			resultStream << L"Total result = " << scoreTotal << L"\n";
		}
		else
		{
			resultStream << L"Total result for " << debug << L" = " << scoreTotal << L"\n";
		}
	};

	const auto lineLambdaAssumingXYZIsValue = [&scoreTotal, &scoresPerTurn](const std::wstring_view& line)
	{
		const auto [lhs, rhs] = SplitString(line, L" ");
		// TODO: Ensure the line is valid, for now assume it is.
		const auto lhsIter = OriginalValueMapping.find(lhs);
		const auto rhsIter = OriginalValueMapping.find(rhs);
		if (lhsIter != OriginalValueMapping.end() && rhsIter != OriginalValueMapping.end())
		{
			const RPSResult result = GetResultForRHS(lhsIter->second, rhsIter->second);
			const int scoreTurn = static_cast<int>(rhsIter->second) + static_cast<int>(result);
			scoreTotal += scoreTurn;
			scoresPerTurn.push_back(scoreTurn);
		}
	};
	const auto lineLambdaAssumingXYZIsResult = [&scoreTotal, &scoresPerTurn](const std::wstring_view& line)
	{
		const auto [lhs, rhs] = SplitString(line, L" ");
		// TODO: Ensure the line is valid, for now assume it is.
		const auto lhsIter = OriginalValueMapping.find(lhs);
		const auto rhsIter = ResultMapping.find(rhs);
		if (lhsIter != OriginalValueMapping.end() && rhsIter != ResultMapping.end())
		{
			const RPSValue counterValue = GetValueForResult(lhsIter->second, rhsIter->second);
			const int scoreTurn = static_cast<int>(rhsIter->second) + static_cast<int>(counterValue);
			scoreTotal += scoreTurn;
			scoresPerTurn.push_back(scoreTurn);
		}
	};

	initValues();
	ForEachLineInTestInputFile(lineLambdaAssumingXYZIsValue);
	logResult(L"XYZ = value");
	if (scoreTotal != 15)
	{
		// Break, this is unexpected.
		__debugbreak();
	}

	initValues();
	ForEachLineInInputFile(lineLambdaAssumingXYZIsValue);
	logResult(L"XYZ = value");

	initValues();
	ForEachLineInTestInputFile(lineLambdaAssumingXYZIsResult);
	logResult(L"XYZ = result");

	if (scoreTotal != 12)
	{
		// Break, this is unexpected.
		__debugbreak();
	}


	initValues();
	ForEachLineInInputFile(lineLambdaAssumingXYZIsResult);
	logResult(L"XYZ = result");


	return resultStream.str();
};

std::wstring Day02::GetIdStr()
{
	return L"Day02";
}
