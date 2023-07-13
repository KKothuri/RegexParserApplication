#pragma once

#include <stack>
#include "ParseTree.h"

class MatchIterator
{
	std::stack<std::shared_ptr<ParseTree>> pathStack;
	int startIndex;
	bool match;
	bool dead;
	int iterationCounter;
	
public:
	std::vector<MatchIterator> consume(wchar_t input);
	int getStartIndex();
	bool isMatch();
	bool isDead();
};
