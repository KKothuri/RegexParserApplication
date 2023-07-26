#pragma once

#include <stack>
#include "ParseTree.h"

class MatchIterator
{
	std::stack<std::shared_ptr<ParseTree>> pathStack;
	std::stack<int> iterStack;
	int startIndex;
	bool match;
	bool dead;
	int iterationCounter;
	
public:
	MatchIterator(std::shared_ptr<ParseTree> root, int start);
	std::vector<MatchIterator> consume(wchar_t input);
	int getStartIndex();
	bool isMatch();
	bool isDead();
};
