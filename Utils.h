#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>

static constexpr std::pair<std::string, std::string> SplitString(const std::string& src, std::string::size_type offset, std::string::size_type substrLen = 0) { using t = std::pair<std::string, std::string>; return ((offset >= src.length()) ? t{ src, {} } : ((offset <= 0 || ((src.length() - offset) < substrLen)) ? t{ {}, src } : t{ src.substr(0, offset), src.substr(offset + substrLen) })); }
static constexpr std::pair<std::string, std::string> SplitString(const std::string& src, const std::string& substr) { const auto offset = src.find(substr); return SplitString(src, offset, substr.length()); }

// NOTE: The compiler succeeds here, but Intellisense fails, so these asserts might be marked as errors, even though they shouldn't be.
static_assert(SplitString("FooBar", 3) == std::pair<std::string, std::string>{"Foo", "Bar"});
static_assert(SplitString("FooBar", 9) == std::pair<std::string, std::string>{"FooBar", {}});
static_assert(SplitString("FooBar", 0) == std::pair<std::string, std::string>{ {}, "FooBar" });

static_assert(SplitString("FooBar", "B") == std::pair<std::string, std::string>{"Foo", "ar"});
static_assert(SplitString("FooBar", "Bar") == std::pair<std::string, std::string>{"Foo", {}});