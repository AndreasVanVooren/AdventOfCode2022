#include "BaseDay.h"

#include <fstream>
#include <chrono>
#include <iostream>
#include "ScopeProfiler.h"

std::filesystem::path BaseDay::GetWorkingFolder()
{
	return std::filesystem::current_path() / GetIdStr();
}

std::filesystem::path BaseDay::GetInputPath()
{
	return GetWorkingFolder() / L"input";
}

std::filesystem::path BaseDay::GetTestInputPath(int numSuffix /*= -1*/)
{
	const std::wstring fileName 
		= (numSuffix >= 0)
		? (L"input_test_" + std::to_wstring(numSuffix))
		: L"input_test";
	return GetWorkingFolder() / fileName;
}

void BaseDay::ForEachLineInFile(std::wifstream& file, const std::function<void(const std::wstring&)>& func)
{
	std::wstring line;
	while (std::getline(file, line))
	{
		func(line);
	}
}

void BaseDay::ForEachLineInFile(const std::filesystem::path& path, const std::function<void(const std::wstring&)>& func)
{
	if (!std::filesystem::exists(path))
	{
		// What the heck do you think you're doing!!!
		__debugbreak();
		return;
	}
	ScopeProfiler prof{ L"Iter file " + path.wstring() };
	std::wifstream fileStream{ path };
	ForEachLineInFile(fileStream, func);
}

void BaseDay::ForEachLineInInputFile(const std::function<void(const std::wstring&)>& func)
{
	ForEachLineInFile(GetInputPath(), func);
}

void BaseDay::ForEachLineInTestInputFile(const std::function<void(const std::wstring&)>& func, int numSuffix /*= -1*/)
{
	ForEachLineInFile(GetTestInputPath(numSuffix), func);
}
