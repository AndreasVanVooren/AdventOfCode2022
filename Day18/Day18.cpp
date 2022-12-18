#include "Day18.h"

#include <string_view>
#include <regex>
#include <numeric>
#include <unordered_set>

#include "../Utils.h"
#include "../ScopeProfiler.h"

namespace Day18Internals
{
	using coord3d = std::tuple<coordPoint, coordPoint, coordPoint>;
	inline constexpr coord3d operator+(const coord3d& a, const coord3d& b)
	{
		return {
			  std::get<0>(a) + std::get<0>(b)
			, std::get<1>(a) + std::get<1>(b)
			, std::get<2>(a) + std::get<2>(b)
		};
	}
	inline constexpr coord3d operator-(const coord3d& a, const coord3d& b)
	{
		return {
			  std::get<0>(a) - std::get<0>(b)
			, std::get<1>(a) - std::get<1>(b)
			, std::get<2>(a) - std::get<2>(b)
		};
	}
	inline constexpr coord3d operator*(const coord3d& a, const coord3d& b)
	{
		return {
			  std::get<0>(a) * std::get<0>(b)
			, std::get<1>(a) * std::get<1>(b)
			, std::get<2>(a) * std::get<2>(b)
		};
	}
	inline constexpr coord3d operator/(const coord3d& a, const coord3d& b)
	{
		return {
			  std::get<0>(a) / std::get<0>(b)
			, std::get<1>(a) / std::get<1>(b)
			, std::get<2>(a) / std::get<2>(b)
		};
	}
	enum Material
	{
		Unknown,
		Lava,
		Air,
		Bubble,
	};
	using matgrid3d = std::vector<grid<Material>>;

	using boolgrid3d = std::vector<grid<bool>>;
	static constexpr boolgrid3d GetAdjacencyKernel()
	{
		return {
		  { {0,0,0}, {0,1,0}, {0,0,0} }
		, { {0,1,0}, {1,0,1}, {0,1,0} }
		, { {0,0,0}, {0,1,0}, {0,0,0} }
		};
	}
	static constexpr coord3d GetAdjacencyKernelSize()
	{
		return {
		  (GetAdjacencyKernel()[0][0].size())
		, (GetAdjacencyKernel()[0].size())
		, (GetAdjacencyKernel().size())
		};
	}
	static constexpr coord3d GetAdjacencyKernelStartOffset()
	{
		return {
		  (std::get<0>(GetAdjacencyKernelSize()) / 2)
		, (std::get<1>(GetAdjacencyKernelSize()) / 2)
		, (std::get<2>(GetAdjacencyKernelSize()) / 2)
		};
	}
	static constexpr coord3d GetAdjacencyKernelEndOffset()
	{
		return GetAdjacencyKernelSize() - GetAdjacencyKernelStartOffset() - coord3d{ 1,1,1 };
	}

	template<typename T, typename TFunc>
	void ForEachElementIn3DGrid(const std::vector<grid<T>>& g, TFunc func)
	{
		for (coordPoint shelfIndex = 0; shelfIndex < g.size(); ++shelfIndex)
		{
			const grid<T>& shelf = g[shelfIndex];
			for (coordPoint rowIndex = 0; rowIndex < shelf.size(); ++rowIndex)
			{
				const std::vector<T>& row = shelf[rowIndex];
				for (coordPoint columnIndex = 0; columnIndex < row.size(); ++columnIndex)
				{
					const T& column = row[columnIndex];
					func(column, coord3d{columnIndex, rowIndex, shelfIndex});
				}
			}
		}
	}
	template<typename T, typename TFunc>
	void ForEachElementIn3DGrid(std::vector<grid<T>>& g, TFunc func)
	{
		for (coordPoint shelfIndex = 0; shelfIndex < g.size(); ++shelfIndex)
		{
			grid<T>& shelf = g[shelfIndex];
			for (coordPoint rowIndex = 0; rowIndex < shelf.size(); ++rowIndex)
			{
				std::vector<T>& row = shelf[rowIndex];
				for (coordPoint columnIndex = 0; columnIndex < row.size(); ++columnIndex)
				{
					T& column = row[columnIndex];
					func(column, coord3d{ columnIndex, rowIndex, shelfIndex });
				}
			}
		}
	}
}

using namespace Day18Internals;

std::wstring Day18::GetResultStr()
{
	using namespace std;
	wstringstream resultStream;
	matgrid3d daGriddy{};
	coord3d tlCoord{};
	coord3d brCoord{};

	const auto reset = [&]()
	{
		daGriddy = { {{Material::Unknown}} };
		tlCoord = {};
		brCoord = {};
	};
	const auto addKnownCoordToGrid = [&](const coord3d& c)
	{
		auto spawnZ = [&](coordPoint amount, bool bBegin)
		{
			coordPoint yItems = get<1>(brCoord) + 1 - get<1>(tlCoord);
			coordPoint xItems = get<0>(brCoord) + 1 - get<0>(tlCoord);
			daGriddy.insert(
				  bBegin ? daGriddy.cbegin() : daGriddy.cend()
				, amount
				, grid<Material>(
					yItems
					, vector<Material>(xItems, Material::Unknown)
				)
			);
		};
		auto spawnY = [&](coordPoint amount, bool bBegin)
		{
			coordPoint xItems = get<0>(brCoord) + 1 - get<0>(tlCoord);
			for (auto& g : daGriddy)
			{
				g.insert(
					bBegin ? g.cbegin() : g.cend()
					, amount
					, vector<Material>( xItems, Material::Unknown)
				);
			}
		};
		auto spawnX = [&](coordPoint amount, bool bBegin)
		{
			for (auto& g : daGriddy)
			{
				for (auto& vec : g)
				{
					vec.insert(
						bBegin ? vec.cbegin() : vec.cend()
						, amount
						, Material::Unknown
					);
				}
			}
		};

		if (get<2>(c) < get<2>(tlCoord))
		{
			coordPoint additionalItems = get<2>(tlCoord) - get<2>(c);
			spawnZ(additionalItems, true);
			get<2>(tlCoord) = get<2>(c);
		}
		if (get<2>(c) > get<2>(brCoord))
		{
			coordPoint additionalItems = get<2>(c) - get<2>(brCoord);
			spawnZ(additionalItems, false);
			get<2>(brCoord) = get<2>(c);
		}

		if (get<1>(c) < get<1>(tlCoord))
		{
			coordPoint additionalItems = get<1>(tlCoord) - get<1>(c);
			spawnY(additionalItems, true);
			get<1>(tlCoord) = get<1>(c);
		}
		if (get<1>(c) > get<1>(brCoord))
		{
			coordPoint additionalItems = get<1>(c) - get<1>(brCoord);
			spawnY(additionalItems, false);
			get<1>(brCoord) = get<1>(c);
		}

		if (get<0>(c) < get<0>(tlCoord))
		{
			coordPoint additionalItems = get<0>(tlCoord) - get<0>(c);
			spawnX(additionalItems, true);
			get<0>(tlCoord) = get<0>(c);
		}
		if (get<0>(c) > get<0>(brCoord))
		{
			coordPoint additionalItems = get<0>(c) - get<0>(brCoord);
			spawnX(additionalItems, false);
			get<0>(brCoord) = get<0>(c);
		}
	};

	const auto parseLine = [&](const wstring_view& line)
	{
		const auto [xStr,otherStr] = SplitString(line, L",");
		const auto [yStr,zStr] = SplitString(otherStr, L",");
		const coord3d c{ stoi(xStr), stoi(yStr), stoi(zStr) };
		addKnownCoordToGrid(c);
		const auto accessPoint = c - tlCoord;
		daGriddy[get<2>(accessPoint)][get<1>(accessPoint)][get<0>(accessPoint)] = Material::Lava;
	};
	const auto fillAirPockets = [&]
	{
		ScopeProfiler prof{ L"FillAir" };
		for (size_t shelfIndex = get<2>(GetAdjacencyKernelStartOffset());
			shelfIndex < daGriddy.size() - get<2>(GetAdjacencyKernelEndOffset());
			++shelfIndex)
		{
			const auto& shelf = daGriddy[shelfIndex];
			for (size_t rowIndex = get<1>(GetAdjacencyKernelStartOffset());
				rowIndex < shelf.size() - get<1>(GetAdjacencyKernelEndOffset());
				++rowIndex)
			{
				const auto& row = shelf[rowIndex];
				for (size_t columnIndex = get<0>(GetAdjacencyKernelStartOffset());
					columnIndex < row.size() - get<0>(GetAdjacencyKernelEndOffset());
					++columnIndex)
				{
					if (row[columnIndex] != Material::Unknown) continue;

					const coord3d startCoord{ columnIndex, rowIndex, shelfIndex };
					vector<coord3d> visited = { startCoord };
					vector<coord3d> queue = { startCoord };
					bool bFoundAir = false;
					bool bFoundBubble = false;
					while (!bFoundAir && !queue.empty())
					{
						auto queueCopy = std::move(queue);
						for (const coord3d& gridCoord : queueCopy)
						{
							ForEachElementIn3DGrid(
								GetAdjacencyKernel()
								, [&](const bool& value, const coord3d& kernelCoord)
								{
									if (!value) return;
									if (bFoundAir || bFoundBubble) return;

									const coord3d coordToCheck = gridCoord + (kernelCoord - GetAdjacencyKernelStartOffset());
									const Material& matAtPoint = daGriddy[get<2>(coordToCheck)][get<1>(coordToCheck)][get<0>(coordToCheck)];
									if (matAtPoint == Material::Air)
									{
										bFoundAir = true;
									}
									else if (matAtPoint == Material::Bubble)
									{
										bFoundBubble = true;
									}
									else if(matAtPoint == Material::Unknown)
									{
										if (ranges::find(visited, coordToCheck) == visited.cend())
										{
											visited.push_back(coordToCheck);
											queue.push_back(coordToCheck);
										}
									}
								}
							);

							if (bFoundAir || bFoundBubble) break;
						}
					}

					for (const coord3d& gridCoord : visited)
					{
						daGriddy[get<2>(gridCoord)][get<1>(gridCoord)][get<0>(gridCoord)]
							= bFoundAir ? Material::Air : Material::Bubble;
					}
				}
			}
		}

	};
	const auto finalize = [&]
	{
		ScopeProfiler prof{ L"Finalize" };
		addKnownCoordToGrid(tlCoord - coord3d{1, 1, 1});
		addKnownCoordToGrid(brCoord + coord3d{1, 1, 1});
		ForEachElementIn3DGrid(
			daGriddy
			, [&](Material& mat, const coord3d& coord)
			{
				if (get<2>(coord) == 0 || get<2>(coord) == (daGriddy.size() - 1)
				|| get<1>(coord) == 0 || get<1>(coord) == (daGriddy[0].size() - 1)
				|| get<0>(coord) == 0 || get<0>(coord) == (daGriddy[0][0].size() - 1))
				{
					mat = Material::Air;
				}
			}
		);
		fillAirPockets();
	};
	const auto getSurfaceArea = [&]()
	{
		// TODO: FFT?
		ScopeProfiler prof{ L"GetSurfaceArea" };
		size_t totalSurface = 0;
		size_t totalSurfaceNoBubble = 0;
		for (size_t shelfIndex = get<2>(GetAdjacencyKernelStartOffset());
			shelfIndex < daGriddy.size() - get<2>(GetAdjacencyKernelEndOffset());
			++shelfIndex)
		{
			const auto& shelf = daGriddy[shelfIndex];
			for (size_t rowIndex = get<1>(GetAdjacencyKernelStartOffset());
				rowIndex < shelf.size() - get<1>(GetAdjacencyKernelEndOffset());
				++rowIndex)
			{
				const auto& row = shelf[rowIndex];
				for (size_t columnIndex = get<0>(GetAdjacencyKernelStartOffset());
					columnIndex < row.size() - get<0>(GetAdjacencyKernelEndOffset());
					++columnIndex)
				{
					if (row[columnIndex] != Material::Lava)
					{
						continue;
					}

					const coord3d gridCoord{ columnIndex, rowIndex, shelfIndex };
					ForEachElementIn3DGrid(
						GetAdjacencyKernel()
						, [&](const bool& value, const coord3d& coord)
						{
							if (!value) return;

							const coord3d coordToCheck = gridCoord + (coord - GetAdjacencyKernelStartOffset());
							const Material& matAtPoint = daGriddy[get<2>(coordToCheck)][get<1>(coordToCheck)][get<0>(coordToCheck)];
							if (matAtPoint == Material::Air)
							{
								++totalSurface;
								++totalSurfaceNoBubble;
							}
							else if (matAtPoint == Material::Bubble)
							{
								++totalSurface;
							}
						}
					);
				}
			}
		}
		return std::tie(totalSurface, totalSurfaceNoBubble);
	};
	const auto output = [&](size_t result, size_t resultNoBubble, const wstring& debug)
	{
		resultStream << L"[" << debug << L"] Surface area of droplet = " << resultNoBubble << ", including bubbles = " << result << "\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine, 0);
	finalize();
	auto pair = getSurfaceArea();
	auto result = get<0>(pair);
	auto resultNoAir = get<1>(pair);
	if (result != 10) __debugbreak();
	if (resultNoAir != 10) __debugbreak();
	output(result, resultNoAir, L"Test 0");

	reset();
	ForEachLineInTestInputFile(parseLine, 1);
	finalize();
	pair = getSurfaceArea();
	result = get<0>(pair);
	resultNoAir = get<1>(pair);
	if (result != 64) __debugbreak();
	if (resultNoAir != 58) __debugbreak();
	output(result, resultNoAir, L"Test 1");

	reset();
	ForEachLineInInputFile(parseLine);
	finalize();
	pair = getSurfaceArea();
	result = get<0>(pair);
	resultNoAir = get<1>(pair);
	if (result != 4636) __debugbreak();
	output(result, resultNoAir, L"Final");


	return resultStream.str();
};
std::wstring Day18::GetIdStr()
{
	return L"Day18";
};
