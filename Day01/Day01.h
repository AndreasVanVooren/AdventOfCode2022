#pragma once
#include "../BaseDay.h"
class Day01 : public BaseDay
{
public:
	virtual std::string GetResultStr();
	virtual std::string GetIdStr();
	std::string GetResultForStream(const std::filesystem::path& path);
};
