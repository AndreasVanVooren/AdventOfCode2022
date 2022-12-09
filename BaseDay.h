#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <fstream>
#include <filesystem>

class BaseDay
{
public:
	virtual std::string GetResultStr() = 0;
	virtual std::string GetIdStr() = 0;
	std::filesystem::path GetWorkingFolder();
	std::filesystem::path GetInputPath();
	std::filesystem::path GetTestInputPath(int numSuffix = -1);
	void ForEachLineInFile(std::ifstream& file, const std::function<void(const std::string&)>& func);
	void ForEachLineInFile(const std::filesystem::path& path, const std::function<void(const std::string&)>& func);
	void ForEachLineInInputFile(const std::function<void(const std::string&)>& func);
	void ForEachLineInTestInputFile(const std::function<void(const std::string&)>& func, int numSuffix = -1);
};