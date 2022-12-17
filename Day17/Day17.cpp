#include "Day17.h"

#include <string_view>
#include <regex>
#include <numeric>
#include <unordered_set>

#include "../Utils.h"
#include "../ScopeProfiler.h"

namespace Day17Internals
{
	enum class JetDirection
	{
		LeftSide,
		RightSide,
		HaWoMukidashite,
	};

	enum class JetStreamSamDirection
	{
		MemoriesBroken,
		TheTruthGoesUnspoken,
		IveEvenForgottenMyName,

		IDontKnowTheSeason,
		OrWhatIsTheReason,
		ImStandingHereHoldingMyBlade,

		ADesolatePlace,
		WithoutAnyTrace,
		ItsTheOnlyColdWindIFeel,

		ItsMeThatISpite,
		AsIStandUpAndFight,
		TheOnlyThingIKnowForReal,

		ThereWillBeBlood_Shed,
		TheManInTheMirrorNodsHisHead,
		TheOnlyOne_Left,
		WillRideUponTheDragonsBack,
		BecauseTheMountainsDontGiveBackWhatTheyTake,

		OhNoThereWillBeBlood_Shed,
		ItsTheOnlyThingIveEverKnown
	};

	enum class Material : uint8_t
	{
		Air,
		Rock,
		Wall,
	};

	struct Rock
	{
		grid<Material> RockShape;
	};

	static const Rock ROCK_HOR{
		{
			{Material::Rock, Material::Rock, Material::Rock, Material::Rock}
		}
	};
	static const Rock ROCK_VER{
		{
			  {Material::Rock}
			, {Material::Rock}
			, {Material::Rock}
			, {Material::Rock}
		}
	};
	static const Rock ROCK_PLUS{
		{
			  {Material::Air,  Material::Rock, Material::Air}
			, {Material::Rock, Material::Rock, Material::Rock}
			, {Material::Air,  Material::Rock, Material::Air}
		}
	};
	static const Rock ROCK_CUBE{
		{
			  {Material::Rock, Material::Rock}
			, {Material::Rock, Material::Rock}
		}
	};
	static const Rock ROCK_J_INVERSE{
		{
			  {Material::Rock, Material::Rock, Material::Rock}
			, {Material::Air,  Material::Air,  Material::Rock}
			, {Material::Air,  Material::Air,  Material::Rock}
		}
	};

	static const std::vector<Rock> ROCK_DROP_ORDER{
		  ROCK_HOR
		, ROCK_PLUS
		, ROCK_J_INVERSE
		, ROCK_VER
		, ROCK_CUBE
	};
}

using namespace Day17Internals;

std::wstring Day17::GetResultStr()
{
	std::wstringstream resultStream;
	std::vector<JetDirection> jetSequence{};
	size_t jetSequenceIndex{};
	grid<Material> daGriddy{};

	const auto reset = [&]
	{
		jetSequence = {};
		jetSequenceIndex = {};
		daGriddy = {};
	};

	const auto parseLine = [&](const std::wstring_view& line)
	{
		jetSequence.reserve(jetSequence.size() + line.length());
		for (const auto& c : line)
		{
			switch (c)
			{
			case '<':
				jetSequence.push_back(JetDirection::LeftSide);
				break;
			case '>':
				jetSequence.push_back(JetDirection::RightSide);
				break;
			}
		}
	};

	const auto whereRockIsNotAir = [](const Material&, const Material& rockMat) -> bool { return rockMat != Material::Air; };

	const auto dropSingleRock = [&](const Rock& originalRockShape, size_t gridWidth)
	{
		// Make a copy, I think this one might be one where we gotta rotate stuff eventually.
		Rock rockShape = originalRockShape;
		coord currentOffset{};
		// Appear.
		{
			// First, resize to accommodate the new rock.
			const auto firstEmpty = std::ranges::find(daGriddy, std::vector{ gridWidth, Material::Air });
			size_t beginSize = std::distance(daGriddy.begin(), firstEmpty);
			constexpr size_t offset = 3;
			daGriddy.resize(
				beginSize + offset + rockShape.RockShape.size()
				, std::vector{ gridWidth, Material::Air }
			);

			currentOffset = { 2, beginSize + offset };
		}

		bool bAtRest = false;
		while (!bAtRest)
		{
			// Jet push
			{
				const JetDirection jetDirection = jetSequence[jetSequenceIndex];
				jetSequenceIndex = (jetSequenceIndex + 1) % jetSequence.size();
				coord nextOffset = currentOffset;
				switch (jetDirection)
				{
				case JetDirection::LeftSide:
					--nextOffset.first;
					break;
				case JetDirection::RightSide:
					++nextOffset.first;
					break;
				}
				bool bHasCollision = !(nextOffset.first >= 0 && nextOffset.first + rockShape.RockShape[0].size() <= daGriddy[0].size());
				if (!bHasCollision)
				{
					ForGridOnOtherGrid<Material>(
						daGriddy
						, rockShape.RockShape
						, nextOffset
						, whereRockIsNotAir
						, [&](Material& receive, const Material&) { bHasCollision |= (receive != Material::Air); }
					);
				}

				if (!bHasCollision)
				{
					currentOffset = nextOffset;
				}
			}

			// Drop it https://youtu.be/AfLLzxYROF4?t=74
			{
				// NOTE: I'm moving the grid upwards, in a strange inverse mirror world.
				// This saves me a few conversions from assuming the floor is the last index.
				const coord nextOffset = { currentOffset.first, currentOffset.second - 1 };

				bool bHasCollision = !(nextOffset.second >= 0);
				if (!bHasCollision)
				{
					ForGridOnOtherGrid<Material>(
						  daGriddy
						, rockShape.RockShape
						, nextOffset
						, whereRockIsNotAir
						, [&](Material& receive, const Material&) { bHasCollision |= (receive != Material::Air); }
					);

				}

				if (bHasCollision)
				{
					// Rest, exit function.
					bAtRest = true;
					ForGridOnOtherGrid<Material>(
						daGriddy
						, rockShape.RockShape
						, currentOffset
						, whereRockIsNotAir
						, [](Material& receive, const Material& setter) { receive = setter; }
					);
				}
				else
				{
					// Continue with next loop.
					currentOffset = nextOffset;
				}
			}
		}

		return currentOffset;
	};

	const auto getTowerHeight = [&]() -> ptrdiff_t
	{
		// Figure out the height.
		auto iter = std::find_if(
			  daGriddy.crbegin()
			, daGriddy.crend()
			, [](const std::vector<Material>& row) { return row != std::vector<Material>{row.size(), Material::Air}; }
		);
		if (iter.base() != daGriddy.cend())
		{
			return std::distance(daGriddy.cbegin(), iter.base());
		}
		else
		{
			return 0;
		}

	};

	const auto dropAllRocks = [&](size_t turns, size_t width) -> ptrdiff_t
	{
		ScopeProfiler prof{ L"DropIt" };
		daGriddy = {};
		jetSequenceIndex = {};

		ptrdiff_t currentAccumulatedHeight = 0;
		size_t currentRock = 0;
		bool bFirstStateFound = false;
		coordPoint firstOffset = 0;
		size_t firstRockIndex = 0;
		size_t firstJetSequencePostDrop = 0;
		bool bModuloFound = false;

		for (size_t i = 0; i < turns; i++, currentRock = (currentRock + 1) % ROCK_DROP_ORDER.size())
		{
			const size_t jetSequencePre = jetSequenceIndex;
			auto rockOffset = dropSingleRock(ROCK_DROP_ORDER[currentRock], width);

			if (!bFirstStateFound)
			{
				firstOffset = rockOffset.first;
				firstRockIndex = currentRock;
				firstJetSequencePostDrop = jetSequenceIndex;
				bFirstStateFound = true;
			}
			else if (!bModuloFound && rockOffset.first == firstOffset && currentRock == firstRockIndex && jetSequenceIndex == firstJetSequencePostDrop)
			{
				// The next patterns will just be repeats of the current pattern,
				// i.e. + will fall on (2, 1), J will fall on (0, 3), etc.
				// Skip all those, by modulo.
				bModuloFound = true;

				// Undo the previous move so we can ascertain the height
				ForGridOnOtherGrid<Material>(
					  daGriddy
					, ROCK_DROP_ORDER[currentRock].RockShape
					, rockOffset
					, whereRockIsNotAir
					, [](Material& receive, const Material&) { receive = Material::Air; }
				);

				// Ascertain the height
				currentAccumulatedHeight = getTowerHeight();

				// How many times will this pattern repeat?
				currentAccumulatedHeight *= (turns / i);

				// Get how many turns are left by modulo
				turns %= i;
				
				// Undo the index so that we can start from scratch.
				--i;

				// Clear the array so that we start from scratch (since it repeats anyway)
				daGriddy = {};
			}
		}
		// Figure out the height.
		auto iter = std::find_if(
			  daGriddy.crbegin()
			, daGriddy.crend()
			, [](const std::vector<Material>& row) { return row != std::vector<Material>{row.size(), Material::Air}; }
		);
		if (iter.base() != daGriddy.cend())
		{
			return currentAccumulatedHeight + std::distance(daGriddy.cbegin(), iter.base());
		}
		else
		{
			return currentAccumulatedHeight;
		}
	};

	const auto printDaGriddy = [&]
	{
		for (auto iter = daGriddy.crbegin(); iter != daGriddy.crend(); ++iter)
		{
			std::wcout << L"|";
			for (Material m : *iter)
			{
				switch (m)
				{
				case Day17Internals::Material::Air:
					std::wcout << L" ";
					break;
				case Day17Internals::Material::Rock:
					std::wcout << L"#";
					break;
				}
			}
			std::wcout << L"|\n";
		}
		std::wcout << L"+";
		for (size_t i = 0; i < daGriddy[0].size(); i++)
		{
			std::wcout << L"-";
		}
		std::wcout << L"+\n";
	};

	const auto output = [&](ptrdiff_t towerHeight, size_t turns, const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] Tower is " << towerHeight << L" units high after " << turns << " turns.\n";
	};

	reset();
	ForEachLineInTestInputFile(parseLine);
	ptrdiff_t result = dropAllRocks(2022, 7);
	if (result != 3068) __debugbreak();
	//ptrdiff_t result2 = dropAllRocks(1'000'000'000'000, 7);
	output(result, 2022, L"Test");
	//output(result2, 1'000'000'000'000, L"Test");
	//if (result2 != 1514285714288) __debugbreak();

	reset();
	ForEachLineInInputFile(parseLine);
	result = dropAllRocks(2022, 7);
	if (result != 3127) __debugbreak();
	//result2 = dropAllRocks(1'000'000'000'000, 7);
	output(result, 2022, L"Final");
	//output(result2, 1'000'000'000'000, L"Final");
	//if (result2 != 1514285714288) __debugbreak();

	return resultStream.str();
};

std::wstring Day17::GetIdStr()
{
	return L"Day17";
};
