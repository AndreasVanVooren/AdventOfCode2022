#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <utility>

static constexpr std::pair<std::wstring, std::wstring> SplitString(const std::wstring_view& src, std::wstring::size_type offset, std::wstring::size_type substrLen = 0)
{ using t = std::pair<std::wstring, std::wstring>; return ((offset >= src.length()) ? t{ src, std::wstring{} } : ((offset <= 0 || ((src.length() - offset) < substrLen)) ? t{ std::wstring{}, src } : t{ src.substr(0, offset), src.substr(offset + substrLen) })); }
static constexpr std::pair<std::wstring, std::wstring> SplitString(const std::wstring_view& src, const std::wstring_view& substr) { const auto offset = src.find(substr); return SplitString(src, offset, substr.length()); }

// NOTE: The compiler succeeds here, but Intellisense fails, so these asserts might be marked as errors, even though they shouldn't be.
static_assert(SplitString(L"FooBar", 3) == std::pair<std::wstring, std::wstring>{L"Foo", L"Bar"});
static_assert(SplitString(L"FooBar", 9) == std::pair<std::wstring, std::wstring>{L"FooBar", {}});
static_assert(SplitString(L"FooBar", 0) == std::pair<std::wstring, std::wstring>{ {}, L"FooBar" });

static_assert(SplitString(L"FooBar", L"B") == std::pair<std::wstring, std::wstring>{L"Foo", L"ar"});
static_assert(SplitString(L"FooBar", L"Bar") == std::pair<std::wstring, std::wstring>{L"Foo", {}});

using coordPoint = int64_t;
using coord = std::pair<coordPoint, coordPoint>;
inline constexpr coord operator+(const coord& a, const coord& b)
{
	return { a.first + b.first, a.second + b.second };
}
inline constexpr coord operator-(const coord& a, const coord& b)
{
	return { a.first - b.first, a.second - b.second };
}
inline constexpr coord operator*(const coord& a, const coord& b)
{
	return { a.first * b.first, a.second * b.second };
}
inline constexpr coord operator/(const coord& a, const coord& b)
{
	return { a.first / b.first, a.second / b.second };
}

// Grid type. Accessed by g[row][col], or g[y][x]
template <typename T>
using grid = std::vector<std::vector<T>>;

template <typename T>
constexpr void ForGridOnOtherGrid(
	  grid<T>& recipient
	, const grid<T>& source
	, const coord& location
	, const std::function<bool (const T&, const T&)>& condition
	, const std::function<void (T&, const T&)>& operation
){
	if (source.size() + location.second > recipient.size())
	{
		__debugbreak();
		return;
	}
	if (source.size() == 0)
	{
		__debugbreak();
		return;
	}
	if (source[0].size() + location.first > recipient[0].size())
	{
		__debugbreak();
		return;
	}

	for (size_t row = 0; row < source.size(); ++row)
	{
		for (size_t col = 0; col < source[row].size(); ++col)
		{
			if (condition(recipient[row + location.second][col + location.first], source[row][col]))
			{
				operation(recipient[row + location.second][col + location.first], source[row][col]);
			}
		}
	}
}
