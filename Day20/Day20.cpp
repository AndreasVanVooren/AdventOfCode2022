#include "Day20.h"

#include <string_view>
#include <regex>
#include <numeric>
#include <unordered_set>

#include "../Utils.h"
#include "../ScopeProfiler.h"

using namespace std;

wstring Day20::GetResultStr()
{
	wstringstream resultStream{};
	vector<int64_t> numberList{};

	const auto reset = [&] { numberList = {}; };

	const auto parseLine = [&](const wstring_view& line)
	{
		if (line.empty()) return;
		numberList.push_back(static_cast<int64_t>(stoi(line)));
	};

	const auto getDecrypted = [&](int64_t multiplier, size_t mixAmount)
	{
		vector<int64_t> workingList = numberList;
		if (multiplier != 1)
		{
			for (int64_t& num : workingList)
			{
				num *= multiplier;
			}
		}
		vector<size_t> indexList{};
		indexList.resize(numberList.size());
		iota(indexList.begin(), indexList.end(), 0);

		for (size_t mix = 0; mix < mixAmount; mix++)
		{
			for (size_t i = 0; i < numberList.size(); i++)
			{
				int64_t placesToMove = numberList[i] * multiplier;
				placesToMove
					= (placesToMove > 0)
					? (placesToMove % (numberList.size() - 1))
					: -((-placesToMove) % static_cast<int64_t>(numberList.size() - 1));

				if (placesToMove == 0) continue;

				const vector<size_t>::const_iterator iter = ranges::find(indexList, i);
				ptrdiff_t index = distance(indexList.cbegin(), iter);
				for (int64_t j = 0; j < placesToMove; j++)
				{
					ptrdiff_t nextIndex = (index + 1) % numberList.size();
					std::swap(workingList[index], workingList[nextIndex]);
					std::swap(indexList[index], indexList[nextIndex]);
					index = nextIndex;
				}
				for (int64_t j = 0; j > placesToMove; j--)
				{
					ptrdiff_t prevIndex = (index > 0) ? (index - 1) : (numberList.size() - 1);
					std::swap(workingList[index], workingList[prevIndex]);
					std::swap(indexList[index], indexList[prevIndex]);
					index = prevIndex;
				}
			}
		}
		return workingList;
	};
	const auto getNumAtOffset = [](const vector<int64_t>& vec, int offset)
	{
		const vector<int64_t>::const_iterator zeroIter = ranges::find(vec, 0);
		const ptrdiff_t zeroIndex = distance(vec.cbegin(), zeroIter);
		const ptrdiff_t trueIndex = (zeroIndex + offset) % vec.size();
		return vec[trueIndex];
	};
	const auto getGroveCoordinateSum = [&](size_t multiplier = 1, size_t mixAmount = 1)
	{
		ScopeProfiler prof{ L"GroveCoordinates " + to_wstring(multiplier) + L" - " + to_wstring(mixAmount)};
		const vector<int64_t> workingList = getDecrypted(multiplier, mixAmount);
		const int64_t sum
			= getNumAtOffset(workingList, 1000)
			+ getNumAtOffset(workingList, 2000)
			+ getNumAtOffset(workingList, 3000);
		return sum;
	};

	const auto output = [&](const int64_t result, const int64_t result2, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Grove coordinate sum = " << result << ", with proper decryption = " << result2 << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine);
	auto sum = getGroveCoordinateSum();
	auto sum2 = getGroveCoordinateSum(811589153, 10);
	if (sum != 3) __debugbreak();
	if (sum2 != 1623178306) __debugbreak();
	output(sum,sum2, L"Test");


	reset();
	numberList = { 0,2,-4,3,6,3,7,-1,-3 };
	sum = getGroveCoordinateSum();
	sum2 = getGroveCoordinateSum(811589153, 10);
	if (sum != 12) __debugbreak();

	reset();
	ForEachLineInInputFile(parseLine);
	sum = getGroveCoordinateSum();
	//sum2 = getGroveCoordinateSum(811589153, 10);
	if (sum != 5904) __debugbreak();
	//if (sum2 <= 4355798984151) __debugbreak();
	output(sum, sum2, L"Final");

	return resultStream.str();
};
wstring Day20::GetIdStr()
{
	return L"Day20";
};
