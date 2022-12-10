// AdventOfCode2022.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>

#include "Days.h"

int main()
{
	std::wcout << L"Currently in " << std::filesystem::current_path() << L"\n\n";

	// Check if files don't exist, otherwise create them.
	for (int i = 0; i < 25; i++)
	{
		std::filesystem::path hPath = std::filesystem::current_path();
		std::wstringstream pathStream{};
		pathStream << "Day" << std::setfill(L'0') << std::setw(2) << i + 1;
		hPath /= pathStream.str();
		hPath /= pathStream.str();
		std::filesystem::path cppPath = hPath;
		cppPath += ".cpp";
		hPath += ".h";

		if (!std::filesystem::exists(hPath))
		{
			std::filesystem::create_directories(hPath.parent_path());
			std::wofstream headerStream{hPath};
			headerStream << L"#pragma once\n";
			headerStream << L"#include \"../BaseDay.h\"\n";
			headerStream << L"class " << pathStream.str() << L" : public BaseDay\n";
			headerStream << L"{\n";
			headerStream << L"public:\n";
			headerStream << L"\tvirtual std::wstring GetResultStr();\n";
			headerStream << L"\tvirtual std::wstring GetIdStr();\n";
			headerStream << L"};\n";
		}

		if (!std::filesystem::exists(cppPath))
		{
			//std::filesystem::create_directories(cppPath.parent_path());
			std::wofstream cppStream{ cppPath };
			cppStream << L"#include \"" << pathStream.str() << L".h\"\n";
			cppStream << L"std::wstring " << pathStream.str() << L"::GetResultStr()\n";
			cppStream << L"{\n";
			cppStream << L"\treturn L\"---"<< pathStream.str() << L" is not yet implemented! ---\\n\";\n";
			cppStream << L"};\n";
			cppStream << L"std::wstring " << pathStream.str() << L"::GetIdStr()\n";
			cppStream << L"{\n";
			cppStream << L"\treturn L\"" << pathStream.str() << L"\";\n";
			cppStream << L"};\n";
		}
	}
	IterateDays();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
