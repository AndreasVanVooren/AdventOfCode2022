#pragma once

#include <iostream>
#include "Day01/Day01.h"
#include "Day02/Day02.h"
#include "Day02/Day02Supplement.h"
#include "Day03/Day03.h"
#include "Day04/Day04.h"
#include "Day05/Day05.h"
#include "Day06/Day06.h"
#include "Day07/Day07.h"
#include "Day08/Day08.h"
#include "Day09/Day09.h"
#include "Day10/Day10.h"
#include "Day11/Day11.h"
#include "Day12/Day12.h"
#include "Day13/Day13.h"
#include "Day14/Day14.h"
#include "Day15/Day15.h"
#include "Day16/Day16.h"
#include "Day17/Day17.h"
#include "Day18/Day18.h"
#include "Day19/Day19.h"
#include "Day20/Day20.h"
#include "Day21/Day21.h"
#include "Day22/Day22.h"
#include "Day23/Day23.h"
#include "Day24/Day24.h"
#include "Day25/Day25.h"
#include "ScopeProfiler.h"

static void IterateDays()
{
	{
		ScopeProfiler prof{ L"Day01" };
		std::wcout << Day01{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day02" };
		std::wcout << Day02{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day02Supplement" };
		std::wcout << Day02Supplement{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day03" };
		std::wcout << Day03{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day04" };
		std::wcout << Day04{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day05" };
		std::wcout << Day05{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day06" };
		std::wcout << Day06{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day07" };
		std::wcout << Day07{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day08" };
		std::wcout << Day08{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day09" };
		std::wcout << Day09{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day10" };
		std::wcout << Day10{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day11" };
		std::wcout << Day11{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day12" };
		std::wcout << Day12{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day13" };
		std::wcout << Day13{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day14" };
		std::wcout << Day14{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day15" };
		std::wcout << Day15{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day16" };
		std::wcout << Day16{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day17" };
		std::wcout << Day17{}.GetResultStr();
	}
	std::wcout << L"\n";
	//{
	//	ScopeProfiler prof{ L"Day18" };
	//	std::wcout << Day18{}.GetResultStr();
	//}
	//std::wcout << L"\n";
	//{
	//	ScopeProfiler prof{ L"Day19" };
	//	std::wcout << Day19{}.GetResultStr();
	//}
	//std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day20" };
		std::wcout << Day20{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day21" };
		std::wcout << Day21{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day22" };
		std::wcout << Day22{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day23" };
		std::wcout << Day23{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day24" };
		std::wcout << Day24{}.GetResultStr();
	}
	std::wcout << L"\n";
	{
		ScopeProfiler prof{ L"Day25" };
		std::wcout << Day25{}.GetResultStr();
	}
	std::wcout << L"\n";
}