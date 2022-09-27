#pragma once

#include "ParseTree.h"
#include "PrefilteringConfiguration.h"

PreFilteringConfiguration GeneratePreFilteringConfiguration(const std::shared_ptr<ParseTree>& parseTree, bool& alldigits);

bool IsAllDigits(const IntervalSet& characterSet);

bool HasDigits(const IntervalSet& characterSet);