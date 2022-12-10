#pragma once
#include "../BaseDay.h"
class Day01 : public BaseDay
{
public:
	virtual std::wstring GetResultStr();
	virtual std::wstring GetIdStr();
	std::wstring GetResultForStream(const std::filesystem::path& path);
};
