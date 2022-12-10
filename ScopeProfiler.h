#pragma once

#include <locale>
#include <chrono>
#include <iostream>

class ScopeProfiler
{
public:
	ScopeProfiler(const std::wstring& name)
		: _start{ std::chrono::high_resolution_clock::now() }
		, _name{ name }
	{}

	~ScopeProfiler()
	{
		auto end = std::chrono::high_resolution_clock::now();
		//const auto oldLocale = std::wcout.getloc();
		//std::wcout.imbue(std::locale("en_US.utf8"));
		std::wcout << L"Took " << (end - _start) << L" to execute [" << _name << L"]\n";
		//std::wcout.imbue(oldLocale);
	}
	ScopeProfiler(ScopeProfiler&&) = default;
	ScopeProfiler& operator=(ScopeProfiler&&) = default;
private:
	ScopeProfiler() = delete;
	ScopeProfiler(const ScopeProfiler&) = delete;
	ScopeProfiler& operator=(const ScopeProfiler&) = delete;
	std::chrono::high_resolution_clock::time_point _start;
	std::wstring _name;
};
