// AdventOfCode2022.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>

#include "Days.h"

int main()
{
    std::cout << "Currently in " << std::filesystem::current_path() << "\n\n";

	// Check if files don't exist, otherwise create them.
	for (int i = 0; i < 25; i++)
	{
		std::filesystem::path hPath = std::filesystem::current_path();
		std::stringstream pathStream{};
		pathStream << "Day" << std::setfill('0') << std::setw(2) << i + 1;
		hPath /= pathStream.str();
		hPath /= pathStream.str();
		std::filesystem::path cppPath = hPath;
		cppPath += ".cpp";
		hPath += ".h";

		if (!std::filesystem::exists(hPath))
		{
			std::filesystem::create_directories(hPath.parent_path());
			std::ofstream headerStream{hPath};
			headerStream << "#pragma once\n";
			headerStream << "#include \"../BaseDay.h\"\n";
			headerStream << "class " << pathStream.str() << " : public BaseDay\n";
			headerStream << "{\n";
			headerStream << "public:\n";
			headerStream << "\tvirtual std::string GetResultStr();\n";
			headerStream << "\tvirtual std::string GetIdStr();\n";
			headerStream << "};\n";
		}

		if (!std::filesystem::exists(cppPath))
		{
			//std::filesystem::create_directories(cppPath.parent_path());
			std::ofstream cppStream{ cppPath };
			cppStream << "#include \"" << pathStream.str() << ".h\"\n";
			cppStream << "std::string " << pathStream.str() << "::GetResultStr()\n";
			cppStream << "{\n";
			cppStream << "\treturn \"---"<< pathStream.str() <<" is not yet implemented! ---\\n\";\n";
			cppStream << "};\n";
			cppStream << "std::string " << pathStream.str() << "::GetIdStr()\n";
			cppStream << "{\n";
			cppStream << "\treturn \"" << pathStream.str() << "\";\n";
			cppStream << "};\n";
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
