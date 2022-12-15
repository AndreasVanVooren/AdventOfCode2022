#pragma once

#include <string>
#include <vector>
#include <map>
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