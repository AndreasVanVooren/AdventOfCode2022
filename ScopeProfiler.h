#pragma once

#include <locale>
#include <chrono>
#include <iostream>

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
		const auto oldLocale = std::cout.getloc();
		std::cout.imbue(std::locale(""));
		std::cout << "Took " << (end - _start) << " to execute [" << _name << "]\n";
		std::cout.imbue(oldLocale);
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
