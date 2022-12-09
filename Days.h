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
		ScopeProfiler prof{ "Day01" };
		std::cout << Day01{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day02" };
		std::cout << Day02{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day02Supplement" };
		std::cout << Day02Supplement{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day03" };
		std::cout << Day03{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day04" };
		std::cout << Day04{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day05" };
		std::cout << Day05{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day06" };
		std::cout << Day06{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day07" };
		std::cout << Day07{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day08" };
		std::cout << Day08{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day09" };
		std::cout << Day09{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day10" };
		std::cout << Day10{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day11" };
		std::cout << Day11{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day12" };
		std::cout << Day12{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day13" };
		std::cout << Day13{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day14" };
		std::cout << Day14{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day15" };
		std::cout << Day15{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day16" };
		std::cout << Day16{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day17" };
		std::cout << Day17{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day18" };
		std::cout << Day18{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day19" };
		std::cout << Day19{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day20" };
		std::cout << Day20{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day21" };
		std::cout << Day21{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day22" };
		std::cout << Day22{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day23" };
		std::cout << Day23{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day24" };
		std::cout << Day24{}.GetResultStr();
	}
	std::cout << "\n";
	{
		ScopeProfiler prof{ "Day25" };
		std::cout << Day25{}.GetResultStr();
	}
	std::cout << "\n";
}