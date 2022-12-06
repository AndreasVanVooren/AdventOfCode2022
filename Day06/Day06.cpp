#include "Day06.h"

#include <algorithm>
#include <vector>
#include <regex>
#include <ranges>
#include "../Utils.h"

std::string Day06::GetResultStr()
{
	std::stringstream resultStream{};
	std::string debug{};
	const auto processLambda = [&](const std::string& line)
	{
		if (line.empty()) { return; }
		size_t packetStartIndex = static_cast<size_t>(-1);
		size_t msgStartIndex = static_cast<size_t>(-1);
		for (size_t charIdx = 0; (charIdx < line.length() - 13); ++charIdx)
		{
			bool bInvalidPacketStart = false;
			bool bInvalidMsgStart = false;
			for (size_t i = charIdx; i < charIdx + 14 && i < line.length(); ++i)
			{
				for (size_t j = i + 1; j < charIdx + 14 && j < line.length() ; j++)
				{
					if (line[i] == line[j])
					{
						if ((j - charIdx) < 4)
						{
							bInvalidPacketStart = true;
						}
						bInvalidMsgStart = true;
					}
				}
			}

			if (!bInvalidPacketStart && packetStartIndex == static_cast<size_t>(-1))
			{
				packetStartIndex = charIdx + 4;
			}
			if (!bInvalidMsgStart && msgStartIndex == static_cast<size_t>(-1))
			{
				msgStartIndex = charIdx + 14;
			}
		}

		resultStream << "[" << debug << "] Start of packet = " << packetStartIndex << ", Start of msg = " << msgStartIndex << "\n";
	};
	debug = "test";
	ForEachLineInTestInputFile(processLambda);
	debug = "realsies";
	ForEachLineInInputFile(processLambda);

	return resultStream.str();
};
std::string Day06::GetIdStr()
{
	return "Day06";
};
