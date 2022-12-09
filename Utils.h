#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <chrono>
#include <iostream>

static constexpr std::pair<std::string, std::string> SplitString(const std::string& src, std::string::size_type offset, std::string::size_type substrLen = 0) { using t = std::pair<std::string, std::string>; return ((offset >= src.length()) ? t{ src, {} } : ((offset <= 0 || ((src.length() - offset) < substrLen)) ? t{ {}, src } : t{ src.substr(0, offset), src.substr(offset + substrLen) })); }
static constexpr std::pair<std::string, std::string> SplitString(const std::string& src, const std::string& substr) { const auto offset = src.find(substr); return SplitString(src, offset, substr.length()); }

// NOTE: The compiler succeeds here, but Intellisense fails, so these asserts might be marked as errors, even though they shouldn't be.
static_assert(SplitString("FooBar", 3) == std::pair<std::string, std::string>{"Foo", "Bar"});
static_assert(SplitString("FooBar", 9) == std::pair<std::string, std::string>{"FooBar", {}});
static_assert(SplitString("FooBar", 0) == std::pair<std::string, std::string>{ {}, "FooBar" });

static_assert(SplitString("FooBar", "B") == std::pair<std::string, std::string>{"Foo", "ar"});
static_assert(SplitString("FooBar", "Bar") == std::pair<std::string, std::string>{"Foo", {}});

class ScopeProfiler
{
public:
	ScopeProfiler(const std::string& name)
		: _start{ std::chrono::high_resolution_clock::now() }
		, _name{ name }
	{}

	~ScopeProfiler()
	{
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "Took " << (end - _start).count() << "ns to execute [" << _name << "]\n";
	}
	ScopeProfiler(ScopeProfiler&&) = default;
	ScopeProfiler& operator=(ScopeProfiler&&) = default;
private:
	ScopeProfiler() = delete;
	ScopeProfiler(const ScopeProfiler&) = delete;
	ScopeProfiler& operator=(const ScopeProfiler&) = delete;
	std::chrono::high_resolution_clock::time_point _start;
	std::string _name;
};