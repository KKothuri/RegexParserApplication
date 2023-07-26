#include "MatchIterator.h"

MatchIterator::MatchIterator(std::shared_ptr<ParseTree> root, int start)
{
	pathStack.push(root);
	iterStack.push(0);
	startIndex = start;
	match = false;
	dead = false;
	iterationCounter = 0;
}

std::vector<MatchIterator> MatchIterator::consume(wchar_t input)
{

}

int MatchIterator::getStartIndex()
{
	return startIndex;
}

bool MatchIterator::isMatch()
{
	return match;
}

bool MatchIterator::isDead()
{
	return dead;
}