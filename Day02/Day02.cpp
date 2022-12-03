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

	static const std::map<std::string, RPSValue> OriginalValueMapping
	{
		  {"A", RPSValue::Rock}
		, {"B", RPSValue::Paper}
		, {"C", RPSValue::Scissors}
		, {"X", RPSValue::Rock}
		, {"Y", RPSValue::Paper}
		, {"Z", RPSValue::Scissors}
	};

	static const std::map<std::string, RPSResult> ResultMapping
	{
		  {"X", RPSResult::Loss}
		, {"Y", RPSResult::Draw}
		, {"Z", RPSResult::Win}
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

std::string Day02::GetResultStr()
{
	using namespace Day02Internal;
	std::stringstream resultStream{};
	std::vector<int> scoresPerTurn{};
	int scoreTotal{};

	const auto initValues = [&]
	{
		scoreTotal = 0;
		scoresPerTurn = {};
	};

	const auto logResult = [&](const std::string& debug = {})
	{
		if (debug.empty())
		{
			resultStream << "Total result = " << scoreTotal << "\n";
		}
		else
		{
			resultStream << "Total result for " << debug << " = " << scoreTotal << "\n";
		}
	};

	const auto lineLambdaAssumingXYZIsValue = [&scoreTotal, &scoresPerTurn](const std::string& line)
	{
		const auto [lhs, rhs] = SplitString(line, " ");
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
	const auto lineLambdaAssumingXYZIsResult = [&scoreTotal, &scoresPerTurn](const std::string& line)
	{
		const auto [lhs, rhs] = SplitString(line, " ");
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
	logResult("XYZ = value");
	if (scoreTotal != 15)
	{
		// Break, this is unexpected.
		__debugbreak();
	}

	initValues();
	ForEachLineInInputFile(lineLambdaAssumingXYZIsValue);
	logResult("XYZ = value");

	initValues();
	ForEachLineInTestInputFile(lineLambdaAssumingXYZIsResult);
	logResult("XYZ = result");

	if (scoreTotal != 12)
	{
		// Break, this is unexpected.
		__debugbreak();
	}


	initValues();
	ForEachLineInInputFile(lineLambdaAssumingXYZIsResult);
	logResult("XYZ = result");


	return resultStream.str();
};

std::string Day02::GetIdStr()
{
	return "Day02";
}
