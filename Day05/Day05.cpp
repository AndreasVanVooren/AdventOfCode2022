#include "Day05.h"

#include <algorithm>
#include <vector>
#include <regex>
#include <ranges>
#include "../Utils.h"

std::string Day05::GetResultStr()
{
	std::stringstream resultStream{};
	std::vector<std::vector<char>> warehouse{};
	bool bHasFilledWarehouse = false;
	bool bIsCrateMover9001 = false;

	const auto stackTop = [&warehouse]() -> std::string
	{
		std::stringstream stream{};
		for (const auto& stack : warehouse)
		{
			if (stack.size() > 0)
			{
				stream << *stack.crbegin();
			}
		}
		return stream.str();
	};
	const auto reset = [&]
	{
		warehouse = {};
		bHasFilledWarehouse = false;
	};
	const auto output = [&](const std::string& debug)
	{
		resultStream << "Stack top [" << debug <<"]: " << stackTop() << "\n";
	};
	const auto addLineToWarehouse = [&](const std::string& line)
	{
		if (line.empty())
		{
			for (auto& stack : warehouse)
			{
				// Pop the back since that's the index.
				stack.pop_back();
				std::reverse(stack.begin(), stack.end());
			}
			bHasFilledWarehouse = true;
		}
		else
		{
			// The input test file is filled with spaces where no element is present.
			size_t numColumns = (line.length() + 3) / 4;
			if (warehouse.size() == 0)
			{
				warehouse.resize(numColumns);
			}
			for (size_t i = 0; i < numColumns; ++i)
			{
				size_t charIdx = (i * 4) + 1;
				// Warehouse is in format [A] [B] [C]
				// -> start at 1 for A] [B] [C]
				// -> advance 4 for B] [C]
				if (line[charIdx] != ' ')
				{
					warehouse[i].push_back(line[charIdx]);
				}
			}
		}
	};
	const auto processCmd = [&](const std::string& line)
	{
		if (line.empty()) return;
		const std::regex regexObj{"move ([0-9]+) from ([0-9]+) to ([0-9]+)"};
		std::smatch match;
		if (std::regex_match(line, match, regexObj))
		{
			// The first sub_match is the whole string; the next
			// sub_match is the first parenthesized expression.
			const int src = stoi(match[2].str()) - 1;
			const int dst = stoi(match[3].str()) - 1;
			// We're adding the final row to the columns, which is just a bunch of indices.
			const int num = stoi(match[1].str());
			if (num > warehouse[src].size())
			{
				__debugbreak();
			}
			if (bIsCrateMover9001)
			{
				warehouse[dst].insert(warehouse[dst].end(), std::prev(warehouse[src].cend(), num), warehouse[src].cend());
			}
			else
			{
				warehouse[dst].insert(warehouse[dst].end(), warehouse[src].crbegin(), std::next(warehouse[src].crbegin(), num));
			}
			warehouse[src].resize(warehouse[src].size() - num);
		}
	};
	const auto processLambda = [&](const std::string& line)
	{
		if (!bHasFilledWarehouse)
		{
			addLineToWarehouse(line);
		}
		else
		{
			processCmd(line);
		}
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	output("Test");
	if (stackTop() != "CMZ") __debugbreak();
	reset();
	ForEachLineInInputFile(processLambda);
	output("Final");

	bIsCrateMover9001 = true;

	reset();
	ForEachLineInTestInputFile(processLambda);
	output("Test9001");
	if (stackTop() != "MCD") __debugbreak();
	reset();
	ForEachLineInInputFile(processLambda);
	output("Final9001");


	return resultStream.str();
};
std::string Day05::GetIdStr()
{
	return "Day05";
};
