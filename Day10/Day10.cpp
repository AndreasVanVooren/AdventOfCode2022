#include "Day10.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <ranges>
#include "../Utils.h"
#include "../ScopeProfiler.h"

std::wstring Day10::GetResultStr()
{
	std::wstringstream resultStream{};

	int currentRegisterValue = 1;
	std::vector<int> registerValuePerCycle{};
	static constexpr auto NUM_PX_PER_SCANLINE = 40;
	static constexpr auto NUM_SCANLINES = 6;

	const auto reset = [&]
	{
		registerValuePerCycle = {};
		registerValuePerCycle.reserve(NUM_PX_PER_SCANLINE * (NUM_SCANLINES + 1));

		currentRegisterValue = 1;
	};
	const auto startCycle = [&]
	{
		registerValuePerCycle.push_back(currentRegisterValue);
	};
	const auto signalStrengthDuringCycle = [&](int cycle)
	{
		return registerValuePerCycle[cycle] * (cycle + 1);
	};
	const auto signalSum = [&]
	{
		return signalStrengthDuringCycle(19)
			+ signalStrengthDuringCycle(59)
			+ signalStrengthDuringCycle(99)
			+ signalStrengthDuringCycle(139)
			+ signalStrengthDuringCycle(179)
			+ signalStrengthDuringCycle(219);
	};
	const auto processOp = [&](const std::wstring_view& line)
	{
		if (line == L"noop")
		{
			startCycle();
		}
		else if (line.starts_with(L"addx "))
		{
			const int amount = stoi(line.substr(5));;
			startCycle();
			startCycle();
			currentRegisterValue += amount;
		}
	};
	const auto isPixelLit = [&](int x, int y) -> bool
	{
		const int asCycle = x + (y * NUM_PX_PER_SCANLINE);
		return (x >= (registerValuePerCycle[asCycle] - 1) && x <= (registerValuePerCycle[asCycle] + 1));
	};
	const auto render = [&](const std::wstring& debug)
	{
		ScopeProfiler prof{ L"Screen for " + debug };
		resultStream << L"Output screen for [" << debug << L"] :\n";

		static const std::wstring CHAR_TL = L"+";
		static const std::wstring CHAR_TB = L"-";
		static const std::wstring CHAR_TR = L"+";
		static const std::wstring CHAR_LR = L"|";
		static const std::wstring CHAR_BL = L"+";
		static const std::wstring CHAR_BR = L"+";
		static const std::wstring CHAR_BLOCK = L"#";

		// Preamble
		resultStream << CHAR_TL;
		for (int pixel = 0; pixel < NUM_PX_PER_SCANLINE; ++pixel)
		{
			resultStream << CHAR_TB;
		}
		resultStream << CHAR_TR << L"\n";
		// Line draw loop
		for (int scanLine = 0; scanLine < NUM_SCANLINES; ++scanLine)
		{
			resultStream << CHAR_LR;
			for (int pixel = 0; pixel < NUM_PX_PER_SCANLINE; ++pixel)
			{
				resultStream << (isPixelLit(pixel, scanLine) ? CHAR_BLOCK : L" ");
			}
			resultStream << CHAR_LR << L"\n";
		}
		// Conclusion
		resultStream << CHAR_BL;
		for (int pixel = 0; pixel < NUM_PX_PER_SCANLINE; pixel++)
		{
			resultStream << CHAR_TB;
		}
		resultStream << CHAR_BR << L"\n";
	};
	const auto output = [&](const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] Signal strength sum = " << signalSum() << L"\n";
	};

	reset();
	ForEachLineInTestInputFile(processOp);
	if (registerValuePerCycle[ 19] != 21) __debugbreak();
	if (registerValuePerCycle[ 59] != 19) __debugbreak();
	if (registerValuePerCycle[ 99] != 18) __debugbreak();
	if (registerValuePerCycle[139] != 21) __debugbreak();
	if (registerValuePerCycle[179] != 16) __debugbreak();
	if (registerValuePerCycle[219] != 18) __debugbreak();
	if (signalStrengthDuringCycle( 19) !=  420) __debugbreak();
	if (signalStrengthDuringCycle( 59) != 1140) __debugbreak();
	if (signalStrengthDuringCycle( 99) != 1800) __debugbreak();
	if (signalStrengthDuringCycle(139) != 2940) __debugbreak();
	if (signalStrengthDuringCycle(179) != 2880) __debugbreak();
	if (signalStrengthDuringCycle(219) != 3960) __debugbreak();
	if (signalSum() != 13140) __debugbreak();
	render(L"test");
	output(L"test");


	reset();
	ForEachLineInInputFile(processOp);
	render(L"final");
	output(L"final");

	return resultStream.str();
};
std::wstring Day10::GetIdStr()
{
	return L"Day10";
};
