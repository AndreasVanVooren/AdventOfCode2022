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
	std::map<std::wstring, FakeDir> subDirs{};
	std::map<std::wstring, int> files{};
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

std::wstring Day07::GetResultStr()
{
	std::wstringstream resultStream{};
	FakeDir* currentDir = nullptr;
	FakeDir root{};
	const auto reset = [&]
	{
		currentDir = nullptr;
		root = {};
	};
	const auto processCmd = [&](const std::wstring& cmd, const std::wstring& args)
	{
		if (cmd == L"cd")
		{
			if (args == L"/")
			{
				//std::wcout << "Navigating to root\n";
				currentDir = &root;
			}
			else if (args == L"..")
			{
				currentDir = currentDir->parent;
				//std::wcout << "Navigating to parent dir\n";
			}
			else
			{
				if (currentDir->subDirs.contains(args))
				{
					//std::wcout << "Navigating to " << args << "\n";
					currentDir = &currentDir->subDirs[args];
				}
			}
		}
	};
	const auto processLambda = [&](const std::wstring_view& line)
	{
		if (line.starts_with(L"$ "))
		{
			const auto [cmd, args] = SplitString(line.substr(2), L" ");
			processCmd(cmd, args);
		}
		else if (line.starts_with(L"dir "))
		{
			const std::wstring_view substr = line.substr(4);
			if (!currentDir->subDirs.contains(substr.data()))
			{
				//std::wcout << "Found new dir " << substr << "\n";
				currentDir->subDirs.emplace(substr, FakeDir{currentDir});
			}
		}
		else if(!line.empty())
		{
			const auto [sizeStr, fileName] = SplitString(line, L" ");
			if (!currentDir->files.contains(fileName))
			{
				int size = std::stoi(sizeStr);
				currentDir->files.emplace(fileName, size);
				//std::wcout << "Found new file " << fileName << " with size " << size << "\n";
				auto* fileDir = currentDir;
				while (fileDir != nullptr)
				{
					fileDir->accumulatedSize += size;
					fileDir = fileDir->parent;
				}
			}
		}
	};
	
	const auto processTree = [&](const std::wstring& debug)
	{
		{
			ScopeProfiler prof{ L"DirSizeAccumulator" };

			const int size = DirSizeAccumulator{}(root);
			//std::wcout << L"Total size of smaller files for [" << debug << L"] = " << size << L"\n";
			resultStream << L"Total size of smaller files for [" << debug << L"] = " << size << L"\n";
		}
		{
			ScopeProfiler prof{ L"SmallestRemovableDirFinder" };
			constexpr int maxSize = 70'000'000;
			constexpr int updateSize = 30'000'000;
			const int diskSize = root.accumulatedSize;
			const int leftSize = maxSize - diskSize;
			const int requiredSize = updateSize - leftSize;
			if (requiredSize > 0)
			{
				int result = SmallestRemovableDirFinder{ requiredSize }(root);
				//std::wcout << L"Smallest removable size for [" << debug << L"] = " << result << L"\n";
				resultStream << L"Smallest removable size for [" << debug << L"] = " << result << L"\n";
			}
		}
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	processTree(L"test");

	reset();
	ForEachLineInInputFile(processLambda);
	processTree(L"final");

	return resultStream.str();
};
std::wstring Day07::GetIdStr()
{
	return L"Day07";
};
