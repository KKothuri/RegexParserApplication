#pragma once

#include "IntervalSet.h"
#include <memory>

enum ParseNodeContent
{
	Regex,
	SimpleRegex,
	Unit
};

struct ParseTree
{
	ParseNodeContent content;
	wchar_t symbol;
	std::pair<int, int> repetition;
	IntervalSet characterSet;

	std::vector<std::shared_ptr<ParseTree>> children;
};