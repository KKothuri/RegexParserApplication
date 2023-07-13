// RegexParserApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Parser.h"
#include "RegexQuality.h"
#include "PrefilteringConfigurationGeneration.h"

int main()
{
	std::wcout << L"Hello World!\n";

	auto parseTree = Parser::parse(L"([a-z0-9]+[a-z][a-z][a-z\\d]{30, 500})");

	for (int i = 0; i < 10; i++)
	{
		std::wcout << L"TC " << i + 1 << L": " <<  GenerateRandomMatchString(parseTree) << "\n\n";
	}

	if (VerifySequentialQuantifiers(parseTree))
	{
		std::wcout << "Sequential Quantifiers not present" << "\n";
	}
	else
	{
		std::wcout << "Sequential Quantifiers detected" << "\n";
	}

	auto parseTree2 = Parser::parse(L"([2-9][0-9]{3}([ \\-]?[0-9]{4}){2})"); //IndiaUniqueIdCardNumber

	bool alldigits;
	PreFilteringConfiguration config = GeneratePreFilteringConfiguration(parseTree2, alldigits);
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
