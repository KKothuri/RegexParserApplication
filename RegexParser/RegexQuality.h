#pragma once

#include "ParseTree.h"
#include <string>

std::wstring GenerateRandomMatchString(const std::shared_ptr<ParseTree>& parseTree);

bool VerifySequentialQuantifiers(std::shared_ptr<ParseTree> parseTree);

std::wstring GenerateAdversarialInput(std::shared_ptr<ParseTree> parseTree);