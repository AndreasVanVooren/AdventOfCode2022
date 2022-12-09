#include "Day07.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <ranges>
#include "../Utils.h"
#include "../ScopeProfiler.h"

struct FakeDir
{
	FakeDir* parent = nullptr;
	std::map<std::string, FakeDir> subDirs{};
	std::map<std::string, int> files{};
	int accumulatedSize = 0;
};

struct DirSizeAccumulator
{
	int operator()(const FakeDir& dir) const
	{
		int sizeTotal = dir.accumulatedSize <= 100000 ? dir.accumulatedSize : 0;
		for (const auto& [dirName, subDir] : dir.subDirs)
		{
			sizeTotal += DirSizeAccumulator{}(subDir);
		}
		return sizeTotal;
	}
};

struct SmallestRemovableDirFinder
{
	int requiredSpace;
	int operator()(const FakeDir& dir) const
	{
		int smallestFoundSize = (dir.accumulatedSize >= requiredSpace) ? dir.accumulatedSize : std::numeric_limits<int>::max();
		for (const auto& [dirName, subDir] : dir.subDirs)
		{
			smallestFoundSize = std::min(SmallestRemovableDirFinder{requiredSpace}(subDir), smallestFoundSize);
		}
		return smallestFoundSize;
	}
};

std::string Day07::GetResultStr()
{
	std::stringstream resultStream{};
	FakeDir* currentDir = nullptr;
	FakeDir root{};
	const auto reset = [&]
	{
		currentDir = nullptr;
		root = {};
	};
	const auto processCmd = [&](const std::string& cmd, const std::string& args)
	{
		if (cmd == "cd")
		{
			if (args == "/")
			{
				//std::cout << "Navigating to root\n";
				currentDir = &root;
			}
			else if (args == "..")
			{
				currentDir = currentDir->parent;
				//std::cout << "Navigating to parent dir\n";
			}
			else
			{
				if (currentDir->subDirs.contains(args))
				{
					//std::cout << "Navigating to " << args << "\n";
					currentDir = &currentDir->subDirs[args];
				}
			}
		}
	};
	const auto processLambda = [&](const std::string& line)
	{
		if (line.starts_with("$ "))
		{
			const auto [cmd, args] = SplitString(line.substr(2), " ");
			processCmd(cmd, args);
		}
		else if (line.starts_with("dir "))
		{
			const std::string substr = line.substr(4);
			if (!currentDir->subDirs.contains(substr))
			{
				//std::cout << "Found new dir " << substr << "\n";
				currentDir->subDirs.emplace(substr, FakeDir{currentDir});
			}
		}
		else if(!line.empty())
		{
			const auto [sizeStr, fileName] = SplitString(line, " ");
			if (!currentDir->files.contains(fileName))
			{
				int size = std::stoi(sizeStr);
				currentDir->files.emplace(fileName, size);
				//std::cout << "Found new file " << fileName << " with size " << size << "\n";
				auto* fileDir = currentDir;
				while (fileDir != nullptr)
				{
					fileDir->accumulatedSize += size;
					fileDir = fileDir->parent;
				}
			}
		}
	};
	
	const auto processTree = [&](const std::string& debug)
	{
		{
			ScopeProfiler prof{ "DirSizeAccumulator" };

			const int size = DirSizeAccumulator{}(root);
			//std::cout << "Total size of smaller files for [" << debug << "] = " << size << "\n";
			resultStream << "Total size of smaller files for [" << debug << "] = " << size << "\n";
		}
		{
			ScopeProfiler prof{ "SmallestRemovableDirFinder" };
			constexpr int maxSize = 70'000'000;
			constexpr int updateSize = 30'000'000;
			const int diskSize = root.accumulatedSize;
			const int leftSize = maxSize - diskSize;
			const int requiredSize = updateSize - leftSize;
			if (requiredSize > 0)
			{
				int result = SmallestRemovableDirFinder{ requiredSize }(root);
				//std::cout << "Smallest removable size for [" << debug << "] = " << result << "\n";
				resultStream << "Smallest removable size for [" << debug << "] = " << result << "\n";
			}
		}
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	processTree("test");

	reset();
	ForEachLineInInputFile(processLambda);
	processTree("final");

	return resultStream.str();
};
std::string Day07::GetIdStr()
{
	return "Day07";
};
