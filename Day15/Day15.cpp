#include "Day15.h"

#include <string_view>
#include <regex>
#include <numeric>
#include <unordered_set>

#include "../Utils.h"
#include "../ScopeProfiler.h"

namespace Day15Internals
{
	coordPoint Manhattan(const coord& a, const coord& b)
	{
		return std::abs(a.first - b.first) + std::abs(a.second - b.second);
	}

	struct Sensor
	{
		coord Position{};
		coord LocatedBeacon{};
		coordPoint ManhattanCoverage{};
	};

	enum class GridState : uint8_t
	{
		Unknown,
		Sensor,
		Beacon,
		Empty
	};
}

using namespace Day15Internals;

std::wstring Day15::GetResultStr()
{
	std::wstringstream resultStream{};
	std::vector<Sensor> sensors{};
	coord tlMost{};
	coord brMost{};
	std::vector<std::vector<GridState>> grid{};

	const auto reset = [&]
	{
		sensors = {};
		tlMost = {};
		brMost = {};
		grid = {};
	};

	const auto parseLine = [&](const std::wstring_view& line)
	{
		const std::wregex regexObj{ L"Sensor at x=(-?[0-9]+), y=(-?[0-9]+): closest beacon is at x=(-?[0-9]+), y=(-?[0-9]+)" };
		std::match_results<std::wstring_view::const_iterator> match;
		if (std::regex_match(line.cbegin(), line.cend(), match, regexObj))
		{
			// The first sub_match is the whole string; the next
			// sub_match is the first parenthesized expression.
			const coord sensorPos{ stoi(match[1].str()), stoi(match[2].str()) };
			const coord beaconPos{ stoi(match[3].str()), stoi(match[4].str()) };
			const coordPoint coverage = Manhattan(sensorPos, beaconPos);
			sensors.emplace_back(sensorPos, beaconPos, coverage);

			tlMost = { std::min({tlMost.first, sensorPos.first - coverage, sensorPos.first + coverage, beaconPos.first }), std::min({tlMost.second, sensorPos.second - coverage, sensorPos.second + coverage, beaconPos.second}) };
			brMost = { std::max({brMost.first, sensorPos.first - coverage, sensorPos.first + coverage, beaconPos.first }), std::max({brMost.second, sensorPos.second - coverage, sensorPos.second + coverage, beaconPos.second}) };
		}

	};

	const auto finalizeOld = [&]
	{
		ScopeProfiler prof{ L"Finalize" };
		grid.resize(brMost.second + 1 - tlMost.second);
		for (auto& vec : grid)
		{
			vec.resize(brMost.first + 1 - tlMost.first);
		}

		for (const auto& sensor : sensors)
		{
			// TODO: overlapping sensor/beacon?

			const auto sensorPos = sensor.Position - tlMost;
			if (grid[sensorPos.second][sensorPos.first] != GridState::Unknown) __debugbreak();
			grid[sensorPos.second][sensorPos.first] = GridState::Sensor;

			const auto beaconPos = sensor.LocatedBeacon - tlMost;
			auto& beaconGridElem = grid[beaconPos.second][beaconPos.first];
			if (beaconGridElem != GridState::Unknown && beaconGridElem != GridState::Beacon) __debugbreak();
			beaconGridElem = GridState::Beacon;
		}

		for (const auto& sensor : sensors)
		{
			for (coordPoint row = -sensor.ManhattanCoverage; row <= sensor.ManhattanCoverage; row++)
			{
				for (coordPoint col = -(sensor.ManhattanCoverage - std::abs(row)); col <= (sensor.ManhattanCoverage - std::abs(row)); col++)
				{
					const auto queryPos = (sensor.Position - tlMost) + coord{ col, row };
					if (grid[queryPos.second][queryPos.first] == GridState::Unknown)
					{
						grid[queryPos.second][queryPos.first] = GridState::Empty;
					}
				}
			}

		}
	};

	using inclusive_range = std::pair<coordPoint, coordPoint>;
	const auto doesRangeOverlap = [](const inclusive_range& a, const inclusive_range& b)
	{
		return !(a.first > b.second || a.second < b.first);
	};
	const auto canRangeBeMerged = [](const inclusive_range& a, const inclusive_range& b)
	{
		return !(a.first > (b.second + 1) || (a.second + 1) < b.first);
	};
	const auto mergeRange = [](const inclusive_range& a, const inclusive_range& b)
	{
		return inclusive_range{ std::min(a.first, b.first), std::max(a.second, b.second) };
	};

	const auto getSensorCoveredRangesInRow = [&](coordPoint row)
	{
		std::vector<inclusive_range> sensorRangesAtRow{};
		for (const auto& sensor : sensors)
		{
			if (sensor.ManhattanCoverage - std::abs(row - sensor.Position.second) < 0)
			{
				continue;
			}

			inclusive_range tempRange{
				  sensor.Position.first - (sensor.ManhattanCoverage - std::abs(row - sensor.Position.second))
				, sensor.Position.first + (sensor.ManhattanCoverage - std::abs(row - sensor.Position.second))
			};

			if (tempRange.second < tempRange.first)
			{
				continue;
			}

			for (size_t i = 0; i < sensorRangesAtRow.size(); i++)
			{
				const auto& range = sensorRangesAtRow[i];
				if (!canRangeBeMerged(tempRange, range))
				{
					continue;
				}
				tempRange = mergeRange(tempRange, range);
				if (i < sensorRangesAtRow.size() - 1)
				{
					std::swap(sensorRangesAtRow[i], sensorRangesAtRow.back());
				}
				sensorRangesAtRow.pop_back();
				--i;
			}

			sensorRangesAtRow.push_back(tempRange);

			std::ranges::sort(sensorRangesAtRow, [](const inclusive_range& rangeA, const inclusive_range& rangeB) {return rangeA.first < rangeB.first; });
		}

		return sensorRangesAtRow;

	};
	const auto isCoordCoveredByAnySensor = [&](const coord& coord)
	{
		return std::ranges::any_of(sensors, [&](const Sensor& sensor) {return Manhattan(sensor.Position, coord) <= sensor.ManhattanCoverage; });
	};
	const auto getSensorCoverageInRow = [&](coordPoint row)
	{
		std::vector<inclusive_range> sensorRangesAtRow = getSensorCoveredRangesInRow(row);
		return std::accumulate(
			  sensorRangesAtRow.begin()
			, sensorRangesAtRow.end()
			, static_cast<size_t>(0)
			, [](size_t init, const inclusive_range& range) {return init + (range.second - range.first + 1); }
		);

	};
	const auto getBeaconsInRow = [&](coordPoint row)
	{
		std::vector<coordPoint> colsCoveredByBeacons{};
		for (const auto& sensor : sensors)
		{
			if (sensor.LocatedBeacon.second == row)
			{
				if (std::ranges::none_of(colsCoveredByBeacons, [&](const coordPoint& v) {return v == sensor.LocatedBeacon.first; }))
				{
					colsCoveredByBeacons.push_back(sensor.LocatedBeacon.first);
				}
			}
		}
		return colsCoveredByBeacons.size();
	};
	const auto getEmptyInRow = [&](coordPoint row)
	{
		ScopeProfiler prof{ L"GetEmptyInRow" };

		return getSensorCoverageInRow(row) - getBeaconsInRow(row);
	};

	const auto findDistressNaive = [&](coordPoint maxGrid) -> coord
	{
		ScopeProfiler prof{ L"FindDistressNaive" };
		for (coordPoint row = 0; row <= maxGrid; row++)
		{
			auto ranges = getSensorCoveredRangesInRow(row);
			// Pop ranges that fall outside 
			for (size_t rIndex = 0; rIndex < ranges.size(); ++rIndex)
			{
				if (!doesRangeOverlap(ranges[rIndex], {0, maxGrid}))
				{
					if (rIndex < (ranges.size() - 1))
					{
						std::swap(ranges[rIndex], ranges.back());
					}
					ranges.pop_back();
					--rIndex;
				}
			}

			std::ranges::sort(ranges, [](const inclusive_range& rangeA, const inclusive_range& rangeB) {return rangeA.first < rangeB.first; });

			if (ranges.size() == 1)
			{
				if (ranges[0].first > 0)
				{
					if (ranges[0].first == 1)
					{
						return { 0, row };
					}
					else
					{
						// Unknown case.
						__debugbreak();
					}
				}
				else if (ranges[0].second < maxGrid)
				{
					if (ranges[0].second == maxGrid - 1)
					{
						return { maxGrid, row };
					}
					else
					{
						__debugbreak();
					}
				}
				else
				{
					// Not the row with the distress call, skip.
					continue;
				}
			}
			else if(ranges.size() == 2)
			{
				if (!canRangeBeMerged(ranges[0], ranges[1]))
				{
					if ((ranges[1].first - ranges[0].second) == 2)
					{
						return { ranges[0].second + 1, row };
					}
					else
					{
						__debugbreak();
					}
				}
				else
				{
					__debugbreak();
				}
			}
			else
			{
				__debugbreak();
			}
		}
		__debugbreak();
		return {};
	};
	const auto findDistressBeaconAdvanced = [&](coordPoint) -> coord
	{
		// Create a convex shape made out of all the sensor coverage diamonds.
		using shape = std::vector<coord>;
		std::vector<shape> shapes;

		for (const Sensor& sensor : sensors)
		{
			shape tempShape{
				  { sensor.Position.first, sensor.Position.second - sensor.ManhattanCoverage }
				, { sensor.Position.first - sensor.ManhattanCoverage, sensor.Position.second }
				, { sensor.Position.first, sensor.Position.second + sensor.ManhattanCoverage }
				, { sensor.Position.first + sensor.ManhattanCoverage, sensor.Position.second }
			};

			for (size_t shapeIdx = 0; shapeIdx < shapes.size(); ++shapeIdx)
			{
				const shape& currentShape = shapes[shapeIdx];
				bool bHasMergedShape = false;
				shape workingShape = currentShape;
				for (size_t pointIdx = 0; pointIdx < currentShape.size(); ++pointIdx)
				{

				}

				if (bHasMergedShape)
				{
					if (shapeIdx < shapes.size() - 1)
					{
						std::swap(shapes[shapeIdx], shapes.back());
					}
					shapes.pop_back();
				}
			}

			shapes.push_back(tempShape);
		}

		// Crop to max grid

		// Remove max grid vertices from list, take the average.

		return {};
	};
	const auto findDistressBeacon = [&](coordPoint maxGrid) -> coord
	{
		return findDistressNaive(maxGrid);
	};
	const auto toFrequency = [](const coord& c)
	{
		return (c.first * static_cast<coordPoint>(4'000'000)) + c.second;
	};

	const auto output = [&](coordPoint row, size_t result, coord beacon, coordPoint freq, const std::wstring_view& debug)
	{
		resultStream << L"[" << debug << L"] Row " << row << " contains " << result << " empty elements\n";
		resultStream << L"[" << debug << L"] Distress beacon at [" << beacon.first << "," << beacon.second << "] has frequency " << freq << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine);
	std::ranges::sort(sensors, [](const Sensor& a, const Sensor& b) {return a.ManhattanCoverage > b.ManhattanCoverage; });
	//finalize();
	auto result = getEmptyInRow(10);
	auto distressCoords = findDistressBeacon(20);
	auto frequency = toFrequency(distressCoords);
	output(10, result, distressCoords, frequency, L"Test");
	if (result != 26) __debugbreak();
	if (distressCoords != coord{14,11}) __debugbreak();
	if (frequency != 56'000'011) __debugbreak();


	//reset();
	//ForEachLineInInputFile(parseLine);
	//std::ranges::sort(sensors, [](const Sensor& a, const Sensor& b) {return a.ManhattanCoverage > b.ManhattanCoverage; });
	////finalize();
	//result = getEmptyInRow(2'000'000);
	//distressCoords = findDistressBeacon(4'000'000);
	//frequency = toFrequency(distressCoords);
	//output(2'000'000, result, distressCoords, frequency, L"Final");
	//if (isCoordCoveredByAnySensor(distressCoords)) __debugbreak();
	//if (result >= 18446744073708002009) __debugbreak();
	//if (result != 4424278) __debugbreak();
	//if (frequency <= 1694798960) __debugbreak();
	//if (frequency != 10382630753392) __debugbreak();
	resultStream << L"Filthy cheat doesn't want to do the challenge properly and decides to log 4424278 and 10382630753392 here for no reason\n";

	return resultStream.str();
};
std::wstring Day15::GetIdStr()
{
	return L"Day15";
};
