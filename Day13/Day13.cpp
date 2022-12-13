#include "Day13.h"
#include <numeric>
#include <variant>
#include "../ScopeProfiler.h"

namespace Day13Internal
{
	struct Item;
	using ItemVariant = std::variant<int, Item>;
	struct Item
	{
		std::vector<ItemVariant> Elements;
	};
	using ItemPair = std::pair<Item, Item>;
	using ItemPairList = std::vector<ItemPair>;


	template<typename Elem, typename Traits>
	std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& stream, const ItemVariant& elem);
	template<typename Elem, typename Traits>
	std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& stream, const Item& item);

	template<typename Elem, typename Traits>
	std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& stream, const ItemVariant& elem)
	{
		if (std::holds_alternative<int>(elem))
		{
			stream << std::get<int>(elem);
		}
		else
		{
			stream << std::get<Item>(elem);
		}
		return stream;
	}

	template<typename Elem, typename Traits>
	std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& stream, const Item& item)
	{
		stream << "[";
		bool bFoundFirst = false;
		for (const ItemVariant& elem : item.Elements)
		{
			if (bFoundFirst)
			{
				stream << ",";
			}
			stream << elem;
			bFoundFirst = true;
		}
		stream << "]";
		return stream;
	}

}
using namespace Day13Internal;

std::wstring Day13::GetResultStr()
{
	std::wstringstream resultStream;
	ItemPairList packetPairs;
	size_t currentIndex = 0;

	const auto reset = [&]
	{
		packetPairs = {};
		currentIndex = {};
	};

	const auto parsePacket = [](const std::wstring& line) -> Item
	{
		if (line.empty()) return Item{};

		Item result{};
		std::vector<Item*> itemStack{};
		std::optional<int> currentInt = {};
		for (wchar_t c : line)
		{
			switch (c)
			{
			case L'[':
				if (itemStack.empty())
				{
					itemStack.push_back(&result);
				}
				else
				{
					itemStack.back()->Elements.push_back(Item{});
					itemStack.push_back(&std::get<Item>(itemStack.back()->Elements.back()));
				}
				break;
			case L']':
				if (currentInt)
				{
					itemStack.back()->Elements.push_back(currentInt.value());
					currentInt = std::optional<int>{};
				}
				itemStack.pop_back();
				break;
			case L',':
				if (currentInt)
				{
					itemStack.back()->Elements.push_back(currentInt.value());
					currentInt = std::optional<int>{};
				}
				break;
			case L'0':
			case L'1':
			case L'2':
			case L'3':
			case L'4':
			case L'5':
			case L'6':
			case L'7':
			case L'8':
			case L'9':
				if (!currentInt)
				{
					currentInt = c - L'0';
				}
				else
				{
					currentInt.value() *= 10;
					currentInt.value() += c - L'0';
				}
				break;
			default:
				break;
			}
		}
		return result;
	};
	const auto processLambda = [&](const std::wstring& line)
	{
		if (line.empty())
		{
			++currentIndex;
			return;
		}
		if (packetPairs.size() == currentIndex)
		{
			packetPairs.push_back({ parsePacket(line), {} });
		}
		else
		{
			packetPairs.back().second = parsePacket(line);
		}
	};

	const auto isPairOrdered = [](const Item& lhsItem, const Item& rhsItem, bool debug = false) -> bool
	{
		std::vector<const Item*> itemStackLhs{};
		std::vector<const Item*> itemStackRhs{};
		std::vector<size_t> positionCache{};
		std::vector<Item> tempItemStack{};

		itemStackLhs.push_back(&lhsItem);
		itemStackRhs.push_back(&rhsItem);
		positionCache.push_back(0);

		if (debug)
		{
			std::wcout << L"- Comparing " << lhsItem << L" vs " << rhsItem << L"\n";
		}

		const auto indent = [](size_t indent)
		{
			for (size_t i = 0; i < indent; ++i) std::wcout << L" ";
		};

		while (itemStackLhs.size() > 0 && itemStackRhs.size() > 0)
		{
			const auto& elementsLhs = itemStackLhs.back()->Elements;
			const auto& elementsRhs = itemStackRhs.back()->Elements;
			const size_t stackStart = positionCache.back();
			bool bNeedsNextLoop = false;
			for (size_t i = stackStart; i < elementsLhs.size(); ++i)
			{
				if (i >= elementsRhs.size())
				{
					if (debug)
					{
						indent((itemStackLhs.size()) * 2);
						std::wcout << L"- Right side ran out of items, so inputs are not in the right order\n";
					}
					return false;
				}

				const auto& lhs = elementsLhs[i];
				const auto& rhs = elementsRhs[i];

				if (debug)
				{
					indent((itemStackLhs.size()) * 2);
					std::wcout << L"- Compare " << lhs << " vs " << rhs << L"\n";
				}

				if (std::holds_alternative<int>(lhs) != std::holds_alternative<int>(rhs))
				{
					if (std::holds_alternative<int>(lhs))
					{
						if (tempItemStack.size() == 0)
						{
							tempItemStack.push_back(Item{ {std::get<int>(lhs)} });
						}
						itemStackLhs.push_back(&tempItemStack.back());
						itemStackRhs.push_back(&std::get<Item>(rhs));
						if (debug)
						{
							indent((itemStackLhs.size()) * 2);
							std::wcout << L"- Mixed types: convert left side to " << tempItemStack.back().Elements[0] << " and retry\n";
						}
					}
					else
					{
						if (tempItemStack.size() == 0)
						{
							tempItemStack.push_back(Item{ {std::get<int>(rhs)} });
						}
						itemStackRhs.push_back(&tempItemStack.back());
						itemStackLhs.push_back(&std::get<Item>(lhs));
						if (debug)
						{
							indent((itemStackLhs.size()) * 2);
							std::wcout << L"- Mixed types: convert right side to " << tempItemStack.back().Elements[0] << " and retry\n";
						}
					}
					positionCache.back() = i + 1;
					positionCache.push_back(0);
					bNeedsNextLoop = true;
					break;
				}
				else
				{
					if (std::holds_alternative<int>(lhs))
					{
						if (std::get<int>(lhs) < std::get<int>(rhs))
						{
							if (debug)
							{
								indent((itemStackLhs.size()) * 2 + 1);
								std::wcout << L"- Left side is smaller, so inputs are in the right order\n";
							}
							return true;
						}
						if (std::get<int>(lhs) > std::get<int>(rhs))
						{
							if (debug)
							{
								indent((itemStackLhs.size()) * 2 + 1);
								std::wcout << L"- Right side is smaller, so inputs are not in the right order\n";
							}
							return false;
						}
					}
					// Must hold item as alt
					else
					{
						itemStackLhs.push_back(&std::get<Item>(lhs));
						itemStackRhs.push_back(&std::get<Item>(rhs));
						positionCache.back() = i + 1;
						positionCache.push_back(0);
						bNeedsNextLoop = true;
						break;
					}
				}
			}

			if (bNeedsNextLoop)
			{
				continue;
			}

			if (elementsLhs.size() < elementsRhs.size())
			{
				if (debug)
				{
					indent((itemStackLhs.size()) * 2 + 1);
					std::wcout << L"- Left side ran out of items, so inputs are in the right order\n";
				}
				return true;
			}

			itemStackLhs.pop_back();
			itemStackRhs.pop_back();
			positionCache.pop_back();
			if (tempItemStack.size() > 0)
			{
				tempItemStack.pop_back();
			}
		}

		if (debug)
		{
			std::wcout << L"Loop end\n";
		}
		return true;
	};

	const auto getOrderedPairIndices = [&]
	{
		const ScopeProfiler prof{ L"GetOrderedPairIndices" };

		std::vector<size_t> indices{};
		for (size_t i = 0; i < packetPairs.size(); ++i)
		{
			if (isPairOrdered(packetPairs[i].first, packetPairs[i].second))
			{
				indices.push_back(i);
			}
		}
		return indices;
	};

	const auto getOrderedPairIndexSum = [&]
	{
		const auto indices = getOrderedPairIndices();
		return std::accumulate(indices.begin(), indices.end(), static_cast<size_t>(0), [](size_t init, size_t val) {return init + val + 1; });
	};

	const auto getDecoderKey = [&]
	{
		const ScopeProfiler prof{ L"GetDecoderKey" };

		const auto dividerA = parsePacket(L"[[2]]");
		const auto dividerB = parsePacket(L"[[6]]");
		std::vector<const Item*> itemList = { &dividerA, &dividerB };
		for (const auto& pair : packetPairs)
		{
			itemList.push_back(&pair.first);
			itemList.push_back(&pair.second);
		}
		std::ranges::sort(itemList, [&isPairOrdered](const Item* const lhs, const Item* const rhs) {return isPairOrdered(*lhs, *rhs); });
		const auto dividerAIter = std::ranges::find(itemList, &dividerA);
		const auto dividerBIter = std::ranges::find(itemList, &dividerB);
		if (dividerAIter == itemList.end()) __debugbreak();
		if (dividerBIter == itemList.end()) __debugbreak();
		const auto dividerADist = std::distance(itemList.begin(), dividerAIter);
		const auto dividerBDist = std::distance(itemList.begin(), dividerBIter);

		return static_cast<size_t>((dividerADist + 1) * (dividerBDist + 1));
	};

	const auto output = [&](size_t sum, size_t key, const std::wstring& debug)
	{
		resultStream << "[" << debug << "] Index sum = " << sum << ", Decoder key = " << key << "\n";
	};

	reset();
	ForEachLineInTestInputFile(processLambda);
	size_t indexSum = getOrderedPairIndexSum();
	if (indexSum != 13) __debugbreak();
	size_t key = getDecoderKey();
	if (key != 140) __debugbreak();
	output(indexSum, key, L"Test");

	reset();
	ForEachLineInInputFile(processLambda);
	indexSum = getOrderedPairIndexSum();
	if (indexSum != 6623) __debugbreak();
	key = getDecoderKey();
	if (key != 23049) __debugbreak();
	output(indexSum, key, L"Final");

	return resultStream.str();
};
std::wstring Day13::GetIdStr()
{
	return L"Day13";
};
