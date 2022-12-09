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
	return GetWorkingFolder() / "input";
}

std::filesystem::path BaseDay::GetTestInputPath(int numSuffix /*= -1*/)
{
	const std::string fileName 
		= (numSuffix >= 0)
		? ("input_test_" + std::to_string(numSuffix))
		: "input_test";
	return GetWorkingFolder() / fileName;
}

void BaseDay::ForEachLineInFile(std::ifstream& file, const std::function<void(const std::string&)>& func)
{
	std::string line;
	while (std::getline(file, line))
	{
		func(line);
	}
}

void BaseDay::ForEachLineInFile(const std::filesystem::path& path, const std::function<void(const std::string&)>& func)
{
	ScopeProfiler prof{ "Iter file " + path.string() };
	std::ifstream fileStream{ path };
	ForEachLineInFile(fileStream, func);
}

void BaseDay::ForEachLineInInputFile(const std::function<void(const std::string&)>& func)
{
	ForEachLineInFile(GetInputPath(), func);
}

void BaseDay::ForEachLineInTestInputFile(const std::function<void(const std::string&)>& func, int numSuffix /*= -1*/)
{
	ForEachLineInFile(GetTestInputPath(numSuffix), func);
}
