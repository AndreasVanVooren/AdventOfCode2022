#include "Day12.h"

#include <set>

#include "../ScopeProfiler.h"

struct Node
{
	wchar_t elevation{};
	std::vector<Node*> usableNodes{};
	int distance = std::numeric_limits<int>::max();
};

std::wstring Day12::GetResultStr()
{
	std::wstringstream resultStream;

	std::vector<Node> nodes{};
	size_t columns = 0;
	size_t startIndex{};
	size_t endIndex{};

	const auto reset = [&]
	{
		startIndex = {};
		endIndex = {};
		nodes = {};
		columns = {};
	};
	const auto processLines = [&](const std::wstring& line)
	{
		for (size_t i = 0; i < line.length(); i++)
		{
			if (line[i] >= L'a' && line[i] <= L'z')
			{
				nodes.push_back(Node{line[i]});
			}
			else if (line[i] == L'S')
			{
				startIndex = nodes.size();
				nodes.push_back(Node{ L'a' });
			}
			else if (line[i] == L'E')
			{
				endIndex = nodes.size();
				nodes.push_back(Node{ L'z' });
			}
		}
		if (columns == 0)
		{
			columns = nodes.size();
		}
	};
	const auto establishNodeLinks = [&]
	{
		ScopeProfiler prof{ L"EstablishNodeLinks" };
		const size_t rows = nodes.size() / columns;
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			Node& node = nodes[i];
			const size_t col = i % columns;
			const size_t row = i / columns;

			const auto assign = [&](bool oobCondition, size_t nodeIdx)
			{
				if (!oobCondition)
				{
					return;
				}
				const int diff = nodes[nodeIdx].elevation - node.elevation;
				if (diff >= -1)
				{
					node.usableNodes.push_back(&nodes[nodeIdx]);
				}
			};

			assign(col > 0, (col - 1) + (row * columns));
			assign(col < columns - 1, (col + 1) + (row * columns));
			assign(row > 0, col + ((row - 1) * columns));
			assign(row < rows - 1, col + ((row + 1) * columns));
		}
	};

	const auto establishDistancesFrom = [](Node* endPoint)
	{
		ScopeProfiler prof{ L"establishDistancesFrom" };
		std::set<Node*> visited = {};
		std::set<Node*> queue{};
		queue.emplace(endPoint);
		visited.emplace(endPoint);
		int distance = 0;
		while (queue.empty() == false)
		{
			auto queueCopy = queue;
			queue.clear();
			for (Node* node : queueCopy)
			{
				node->distance = distance;
				for (Node* subNode : node->usableNodes)
				{
					if (!visited.contains(subNode))
					{
						visited.emplace(subNode);
						queue.emplace(subNode);
					}
				}
			}
			++distance;
		}
	};

	const auto pathSizes = [&]
	{
		ScopeProfiler prof{ L"pathSizes" };
		size_t shortestSizeStartIndex = std::numeric_limits<size_t>::max();
		size_t shortestSizeANode = std::numeric_limits<size_t>::max();
		for (size_t i = 0; i < nodes.size(); ++i)
		{
			auto& node = nodes[i];
			if (node.elevation != L'a')
			{
				continue;
			}
			auto shortestStepCountTest = nodes[i].distance;
			if (shortestStepCountTest < shortestSizeANode)
			{
				shortestSizeANode = shortestStepCountTest;
			}
			if (i == startIndex)
			{
				shortestSizeStartIndex = shortestStepCountTest;
			}
		}
		return std::make_pair(shortestSizeStartIndex, shortestSizeANode);
	};

	const auto output = [&](const auto stepCount, const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] Step count = " << stepCount << "\n";
	};

	reset();
	ForEachLineInTestInputFile(processLines);
	establishNodeLinks();
	establishDistancesFrom(&nodes[endIndex]);
	auto [shortestStepCountTest, shortestStepCountTestA] = pathSizes();
	if (shortestStepCountTest != 31) __debugbreak();
	if (shortestStepCountTestA != 29) __debugbreak();
	output(shortestStepCountTest, L"Test start index");
	output(shortestStepCountTestA, L"Test A node");

	reset();
	ForEachLineInInputFile(processLines);
	establishNodeLinks();
	establishDistancesFrom(&nodes[endIndex]);
	auto [shortestStepCountFinal, shortestStepCountFinalA] = pathSizes();
	output(shortestStepCountFinal, L"Final start index");
	output(shortestStepCountFinalA, L"Final A node");

	return resultStream.str();
};

std::wstring Day12::GetIdStr()
{
	return L"Day12";
};
