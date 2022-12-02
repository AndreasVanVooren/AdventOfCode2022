#include "BaseDay.h"

#include <fstream>

std::filesystem::path BaseDay::GetWorkingFolder()
{
	return std::filesystem::current_path() / GetIdStr();
}

std::filesystem::path BaseDay::GetInputPath()
{
	return GetWorkingFolder() / "input";
}

std::filesystem::path BaseDay::GetTestInputPath()
{
	return GetWorkingFolder() / "input_test";
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
	std::ifstream fileStream{ path };
	ForEachLineInFile(fileStream, func);
}

void BaseDay::ForEachLineInInputFile(const std::function<void(const std::string&)>& func)
{
	ForEachLineInFile(GetInputPath(), func);
}

void BaseDay::ForEachLineInTestInputFile(const std::function<void(const std::string&)>& func)
{
	ForEachLineInFile(GetTestInputPath(), func);
}
