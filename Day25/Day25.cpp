#include "Day25.h"
#include <cstddef>
#include <string>
#include <algorithm>
#include <iostream>
#include <numeric>

using namespace std;

namespace Day25Internals
{
	wstring IntToSnafuV1(size_t theInt)
	{
		wstringstream resultStream{};
		// The INT to SNAFU conversion is a bit scuffed
		while(theInt > 0)
		{
			int64_t intCopy = static_cast<int64_t>(theInt);
			int64_t theMultiplier = 1;
			for (; ((intCopy - (theMultiplier / 5)) / theMultiplier) > 2; theMultiplier *= 5)
			{
			}

			switch (((intCopy - (theMultiplier / 5)) / theMultiplier)) {
				case 0:
					resultStream << L"0";
					break;
				case 1:
					resultStream << L"1";
					break;
				case 2:
					resultStream << L"2";
					break;
				case -1:
					resultStream << L"-";
					break;
				case -2:
					resultStream << L"=";
					break;
			}
			theInt -= intCopy * theMultiplier;
			if(theMultiplier > 1 && intCopy == 0)
			{
				for (size_t i = 1; i < theMultiplier; ++i) {
					resultStream << L'0';
				}
			}
		}

		return resultStream.str();
	}

	constexpr size_t SnafuToIntV1(wstring_view theSnafu)
	{
		size_t thePower = theSnafu.length() - 1;
		size_t theMultiplier = 1;
		size_t theValue = 0;
		for (size_t i = 0; i < thePower; ++i) {
			theMultiplier *= 5;
		}

		for (const wchar_t& c : theSnafu) {
			switch (c)
			{
				case L'2':
				case L'1':
				case L'0':
					theValue += theMultiplier * (c - L'0');
					break;
				case L'-':
					theValue -= theMultiplier;
					break;
				case L'=':
					theValue -= theMultiplier * 2;
					break;
			}
			theMultiplier /= 5;
		}
		return theValue;
	}

	static_assert(SnafuToIntV1(L"1") == 1);
	static_assert(SnafuToIntV1(L"2") == 2);
	static_assert(SnafuToIntV1(L"1=") == 3);
	static_assert(SnafuToIntV1(L"1-") == 4);
	static_assert(SnafuToIntV1(L"10") == 5);
	static_assert(SnafuToIntV1(L"11") == 6);
	static_assert(SnafuToIntV1(L"12") == 7);
	static_assert(SnafuToIntV1(L"2=") == 8);
	static_assert(SnafuToIntV1(L"2-") == 9);
	static_assert(SnafuToIntV1(L"20") == 10);
	static_assert(SnafuToIntV1(L"1=0") == 15);
	static_assert(SnafuToIntV1(L"1-0") == 20);
	static_assert(SnafuToIntV1(L"1=11-2") == 2022);
	static_assert(SnafuToIntV1(L"1-0---0") == 12345);
	static_assert(SnafuToIntV1(L"1121-1110-1=0") == 314159265);
	static_assert(SnafuToIntV1(L"2=-1=0") == 4890);

}

using namespace Day25Internals;

std::wstring Day25::GetResultStr()
{
	const auto testSnafu = [](size_t input, const wstring_view& expected) -> bool
	{
		wstring result = IntToSnafuV1(input);
		if(result != expected)
		{
			wcout << L"Expected conversion from " << input << L" to [" << expected << L"], but got [" << result << L"]\n";
			return false;
		}
		return true;
	};
	const auto testSnafuAll = [&]() -> bool
	{
		bool bSnafu = testSnafu(1, L"1");
		bSnafu &= testSnafu(2, L"2");
		bSnafu &= testSnafu(3, L"1=");
		bSnafu &= testSnafu(4, L"1-");
		bSnafu &= testSnafu(5, L"10");
		bSnafu &= testSnafu(6, L"11");
		bSnafu &= testSnafu(7, L"12");
		bSnafu &= testSnafu(8, L"2=");
		bSnafu &= testSnafu(9, L"2-");
		bSnafu &= testSnafu(10, L"20");
		bSnafu &= testSnafu(15, L"1=0");
		bSnafu &= testSnafu(20, L"1-0");
		bSnafu &= testSnafu(2022, L"1=11-2");
		bSnafu &= testSnafu(12345, L"1-0---0");
		bSnafu &= testSnafu(314'159'265, L"1121-1110-1=0");
		bSnafu &= testSnafu(4890, L"2=-1=0");
		return bSnafu;
	};

	if(!testSnafuAll())
	{
		return L"One of the test cases failed!!!";
	}

	wstringstream resultStream;
	vector<size_t> fuelUnits;

	const auto reset = [&]
	{
		fuelUnits = {};
	};

	const auto parseLine = [&](const wstring_view& line)
	{
		if(line.empty()) return;
		fuelUnits.push_back(SnafuToIntV1(line));
	};

	const auto sum = [&]
	{
		return accumulate(
			fuelUnits.cbegin()
			, fuelUnits.cend()
			, size_t{0}
		);
	};
	
	const auto output = [&](size_t resultV1, const wstring& resultSnafuV1, const wstring& debug)
	{
		resultStream << L"[" << debug << "] Sum is " << resultV1 << L", Snafu code ["<<resultSnafuV1 <<"]\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine);
	size_t resultInt = sum();
	if(resultInt != 4890) __debugbreak();
	wstring resultSnafu = IntToSnafuV1(resultInt);
	if(resultSnafu != L"2=-1=0") __debugbreak();
	output(resultInt, resultSnafu, L"Test");

	reset();
	ForEachLineInInputFile(parseLine);
	resultInt = sum();
	resultSnafu = IntToSnafuV1(resultInt);
	output(resultInt, resultSnafu, L"Final");


	return resultStream.str();
};
std::wstring Day25::GetIdStr()
{
	return L"Day25";
};
