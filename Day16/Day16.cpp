#include "Day16.h"

#include <string_view>
#include <regex>
#include <numeric>
#include <unordered_set>

#include "../Utils.h"
#include "../ScopeProfiler.h"

namespace Day16Internals
{
	using NodeIdType = size_t;
	using FlowRateType = int;
	NodeIdType GetIdentifierForCharacterSequence(const std::wstring_view& view)
	{
		// NOTE: Using this instead of std::stoi with base 26, as it's a lot faster
		// (and also I think I actually need base 36 for stoi to work)
		return (view[0] - 'A') * 26 + (view[1] - 'A');
	}

	std::wstring GetStringForIdentifier(const NodeIdType& id)
	{
		wchar_t result[3]{};
		result[0] = static_cast<wchar_t>(id / 26) + L'A';
		result[1] = static_cast<wchar_t>(id % 26) + L'A';
		result[2] = 0;
		return std::wstring(result);
	}

	struct Node;
	using NodeListType = std::vector<Node>;
	using NodeConstIter = NodeListType::const_iterator;
	using NodeIter = NodeListType::iterator;

	struct Node
	{
		NodeIdType Identifier{};
		FlowRateType FlowRate{};
		std::vector<NodeIter> AttachedNodes{};
		std::vector<NodeIdType> UnprocessedAttachments{};
		std::vector<std::pair<NodeConstIter, size_t>> DistanceToEachNode{};
	};

	using SalesmanType = std::pair<NodeConstIter, size_t>;
	struct SalesGroupCandidate
	{
		FlowRateType FlowCreated{};
		std::vector<SalesmanType> Salesmen;
		std::vector<NodeConstIter> VisitedNodes{};
	};

	inline bool operator==(const SalesGroupCandidate& a, const SalesGroupCandidate& b)
	{
		std::vector<SalesmanType> uniqueSalesmen;
		std::ranges::set_difference(a.Salesmen, b.Salesmen, std::back_inserter(uniqueSalesmen));
		std::vector<NodeConstIter> uniqueNodes;
		std::ranges::set_difference(a.VisitedNodes, b.VisitedNodes, std::back_inserter(uniqueNodes));
		return a.FlowCreated == b.FlowCreated
			&& uniqueSalesmen.size() == 0
			&& uniqueNodes.size() == 0;
	}

	size_t GetPotential(const SalesGroupCandidate& a, size_t maxTime)
	{
		if (a.Salesmen.size() == 0)
		{
			return 0;
		}
		
		size_t totalPotential = 0;
		for (const auto& salesmen : a.Salesmen)
		{
			for (const auto& pair : salesmen.first->DistanceToEachNode)
			{
				if (std::ranges::find(a.VisitedNodes, pair.first) == a.VisitedNodes.cend())
				{
					if (maxTime > (salesmen.second + pair.second))
					{
						totalPotential += (maxTime - (salesmen.second + pair.second)) * pair.first->FlowRate;
					}
				}
			}
		}
		return totalPotential * a.FlowCreated;
	}

	size_t TravelSalesmen(NodeConstIter startNode, size_t amountOfSalesmen = 1, size_t maxDist = 30)
	{
		SalesGroupCandidate topCandidate{};
		{
			SalesGroupCandidate initialCandidate{ 0, std::vector<SalesmanType>(amountOfSalesmen, SalesmanType{startNode,0}), {startNode} };
			std::vector<SalesGroupCandidate> potentialCandidates = {std::move(initialCandidate)};
			while (!potentialCandidates.empty())
			{
				auto workingCandidatesList = std::move(potentialCandidates);

				// For each candidate, generate new candidates, based on the amount of salesmen travelling simultaneously.
				for (const SalesGroupCandidate& candidate : workingCandidatesList)
				{
					std::vector<SalesGroupCandidate> newCandidates{candidate};

					for (size_t salesmanIndex = 0; salesmanIndex < candidate.Salesmen.size(); ++salesmanIndex)
					{
						// Copy this over, so that we can eval each already existing partial candidate at our own leisure.
						std::vector<SalesGroupCandidate> newCandidatesForSalesman = newCandidates;
						for (const SalesGroupCandidate& partialCandidate : newCandidates)
						{
							const auto& partialSalesman = partialCandidate.Salesmen[salesmanIndex];
							for (const auto& [node, dist] : partialSalesman.first->DistanceToEachNode)
							{
								const size_t newTimeTravelled = partialSalesman.second + dist + 1; // Add one, because opening valve == 1 minutes.
								if (newTimeTravelled >= maxDist)
								{
									// Out of time, abort candidate.
									continue;
								}

								if (!partialCandidate.VisitedNodes.empty() && std::ranges::find(partialCandidate.VisitedNodes, node) != partialCandidate.VisitedNodes.cend())
								{
									// Already visited, ignore.
									continue;
								}

								std::vector<NodeConstIter> nodesTemp = partialCandidate.VisitedNodes;
								nodesTemp.push_back(node);
								FlowRateType totalFlowRate = partialCandidate.FlowCreated + (node->FlowRate * static_cast<FlowRateType>(maxDist - newTimeTravelled));
								auto tempSalesmen = partialCandidate.Salesmen;
								tempSalesmen[salesmanIndex] = {node, newTimeTravelled};
								newCandidatesForSalesman.emplace_back(std::move(totalFlowRate), std::move(tempSalesmen), std::move(nodesTemp));
							}
						}
						std::erase(newCandidatesForSalesman, candidate);
						newCandidates.insert(newCandidates.end(), newCandidatesForSalesman.begin(), newCandidatesForSalesman.end());
					}
					std::erase(newCandidates, candidate);

					if (newCandidates.size() == 0)
					{
						// This candidate qualifies as a top candidate for flow rate, try max.
						if (topCandidate.FlowCreated < candidate.FlowCreated)
						{
							topCandidate = candidate;
						}
					}
					else
					{
						auto highestPotential = std::ranges::sort(
							newCandidates
							, [&maxDist](const SalesGroupCandidate& a, const SalesGroupCandidate& b)
							{
								auto aPot = GetPotential(a, maxDist);
								auto bPot = GetPotential(b, maxDist);
								if (aPot > bPot)
								{
									return true;
								}
								else if (aPot < bPot)
								{
									return false;
								}
								else if (aPot != 0 && bPot != 0)
								{
									return false;
								}
								else if (aPot == 0 && bPot == 0)
								{
									return a.FlowCreated > b.FlowCreated;
								}
								else
								{
									return aPot != 0;
								}
							}
						);
						newCandidates.resize((newCandidates.size() + 2) / 3);


						//// Erase cases with the least amount of movement, i.e. If there's one where two move,
						//// then discard all the ones where only one moves. Those tend to have higher flow rates, due to more valves being open.
						//auto bestMoving = std::ranges::max(newCandidates, std::less<size_t>{}, [](const SalesGroupCandidate& c) {return c.VisitedNodes.size(); });
						//std::erase_if(newCandidates, [&](const SalesGroupCandidate& c) {return c.VisitedNodes.size() < bestMoving.VisitedNodes.size(); });
						//
						//// Erase identical cases, where for example:
						//// Case 1: X travels A -> B -> C, Y travels A -> D -> E, generates N flow
						//// Case 2: X travels A -> D -> E, Y travels A -> B -> C, generates N flow
						//auto lastIter = newCandidates.end();
						//for (auto iter = newCandidates.begin(); iter != lastIter;)
						//{
						//	if (std::count(iter, lastIter, *iter) > 1)
						//	{
						//		if (iter != std::prev(lastIter))
						//		{
						//			std::swap(*iter, *std::prev(lastIter));
						//		}
						//		--lastIter;
						//		continue;
						//	}
						//	++iter;
						//}
						//newCandidates.erase(lastIter, newCandidates.end());
						//
						//// Erase flows generated below the average threshold.
						//auto sum = std::accumulate(newCandidates.begin(), newCandidates.end()
						//	, static_cast<FlowRateType>(0)
						//	, [](FlowRateType init, const SalesGroupCandidate& c) {return c.FlowCreated + init;});
						//auto avg = sum / newCandidates.size();
						//std::erase_if(newCandidates, [&](const SalesGroupCandidate& c) {return c.FlowCreated < avg; });


						potentialCandidates.insert(potentialCandidates.cend(), newCandidates.cbegin(), newCandidates.cend());
					}
				}
				if (!potentialCandidates.empty())
				{
					// Erase cases with the least amount of movement, i.e. If there's one where two move,
					// then discard all the ones where only one moves. Those tend to have higher flow rates, due to more valves being open.
					auto bestMoving2 = std::ranges::max(potentialCandidates, std::less<size_t>{}, [](const SalesGroupCandidate& c) {return c.VisitedNodes.size(); });
					std::erase_if(potentialCandidates, [&](const SalesGroupCandidate& c) {return c.VisitedNodes.size() < bestMoving2.VisitedNodes.size(); });

					// Erase excessive amount of candidates
					if (potentialCandidates.size() > 10000)
					{
						std::ranges::sort(potentialCandidates, [](const SalesGroupCandidate& a, const SalesGroupCandidate& b) {return a.FlowCreated > b.FlowCreated; });
						potentialCandidates.erase(std::next(potentialCandidates.begin()), potentialCandidates.end());
					}
				}
			}
		}

		return topCandidate.FlowCreated;
	}
}

using namespace Day16Internals;

std::wstring Day16::GetResultStr()
{
	std::wstringstream resultStream;

	std::vector<Node> nodes{};
	Node* startingNode = nullptr;

	const auto reset = [&]
	{
		nodes = {};
		startingNode = nullptr;
	};

	const auto parseLine = [&](const std::wstring_view& line)
	{
		const std::wregex regexObj{ L"Valve ([A-Z]+) has flow rate=([0-9]+); tunnel[s]? lead[s]? to valve[s]? ([A-Z,\\s]*)" };
		std::wsvmatch match;

		if (std::regex_match(line.cbegin(), line.cend(), match, regexObj))
		{
			Node node{};
			node.Identifier = GetIdentifierForCharacterSequence(std::wstring_view{ match[1].first, match[1].second });
			node.FlowRate = std::stoi(std::wstring_view{ match[2].first, match[2].second });
			auto pair = SplitString(match[3].str(), L", ");
			while (!pair.first.empty())
			{
				node.UnprocessedAttachments.push_back(GetIdentifierForCharacterSequence(pair.first));
				pair = SplitString(pair.second, L", ");
			}

			nodes.push_back(node);
		}
	};

	const auto processAttachments = [&]
	{
		ScopeProfiler prof{ L"ProcessAttachments" };
		for (NodeIter nodeIter = nodes.begin(); nodeIter != nodes.end(); ++nodeIter)
		{
			Node& node = *nodeIter;
			for (const auto& id : node.UnprocessedAttachments)
			{
				NodeIter attachedIter = std::ranges::find_if(nodes, [&](const Node& n) {return n.Identifier == id;});
				node.AttachedNodes.push_back(attachedIter);
				auto reverseRefIter = std::ranges::find(attachedIter->UnprocessedAttachments, node.Identifier);
				if (reverseRefIter != attachedIter->UnprocessedAttachments.cend())
				{
					attachedIter->UnprocessedAttachments.erase(reverseRefIter);
					attachedIter->AttachedNodes.push_back(nodeIter);
				}
			}
			node.UnprocessedAttachments.clear();
		}
	};

	const auto processNodeDistancesFrom = [](NodeIter node)
	{
		size_t distance = 0;
		std::vector<NodeIter> visited;
		visited.push_back(node);
		std::vector<NodeIter> queue{node};
		while (!queue.empty())
		{
			auto workingQueue = std::move(queue);
			for (NodeIter queuedNode : workingQueue)
			{
				if (std::ranges::find_if(node->DistanceToEachNode, [&](const auto& pair) {return pair.first == queuedNode; }) == node->DistanceToEachNode.cend())
				{
					node->DistanceToEachNode.emplace_back(queuedNode, distance);
					if (node != queuedNode)
					{
						queuedNode->DistanceToEachNode.emplace_back(node, distance);
					}
				}
				for (NodeIter attachedNode : queuedNode->AttachedNodes)
				{
					if (std::ranges::find(visited, attachedNode) == visited.cend())
					{
						visited.push_back(attachedNode);
						queue.push_back(attachedNode);
					}
				}
			}
			++distance;
		}
	};

	const auto processNodeDistances = [&]
	{
		ScopeProfiler prof{ L"ProcessNodeDistances" };
		for (Node& node : nodes)
		{
			node.DistanceToEachNode.reserve(nodes.size());
		}

		std::vector<NodeIter> visited{};
		visited.reserve(nodes.size());
		for (NodeIter node = nodes.begin(); node != nodes.end(); ++node)
		{
			processNodeDistancesFrom(node);
			visited.push_back(node);
		}

		for (auto& node : nodes)
		{
			std::erase_if(node.DistanceToEachNode, [](const auto& pair) {return pair.first->FlowRate == 0; });
		}
	};

	const auto finalize = [&]
	{
		processAttachments();
		processNodeDistances();
	};

	const auto travelSalesmenFromStart = [&](size_t salesmen, size_t timeLimit) -> size_t
	{
		ScopeProfiler prof{ L"TravelSalesmen" };
		return TravelSalesmen(std::ranges::find_if(nodes, [](const Node& node) {return node.Identifier == 0; }), salesmen, timeLimit);
	};

	const auto output = [&](size_t result, size_t result2, const std::wstring& debug)
	{
		resultStream << L"[" << debug << L"] One single salesman generates = " << result << " pressure, two generate " << result2 << " pressure\n";
	};

	//reset();
	//ForEachLineInTestInputFile(parseLine);
	//finalize();
	//auto result = travelSalesmenFromStart(1, 30);
	//auto resultWithElephant = travelSalesmenFromStart(2, 26);
	//if (result != 1651) __debugbreak();
	//if (resultWithElephant != 1707) __debugbreak();
	//output(result, resultWithElephant, L"Test");
	//
	//reset();
	//ForEachLineInInputFile(parseLine);
	//finalize();
	//result = travelSalesmenFromStart(1, 30);
	//resultWithElephant = travelSalesmenFromStart(2, 26);
	//if (result != 1728) __debugbreak();
	//if (resultWithElephant <= 1762) __debugbreak();
	//output(result, resultWithElephant, L"Final");


	//return resultStream.str();

	return L"The truth is out there...";
};
std::wstring Day16::GetIdStr()
{
	return L"Day16";
};
