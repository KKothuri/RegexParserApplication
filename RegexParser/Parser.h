#pragma once

#include "ParseTree.h"
#include <string>
#include <map>
#include <set>

enum SymbolType
{
	Normal,
	Special,
	Escaped
};

struct Symbol
{
	Symbol(wchar_t symbol, SymbolType type) :
		symbol(symbol),
		type(type) {}

	wchar_t symbol;
	SymbolType type;
};

using SymbolArray = std::vector<Symbol>;

using OptionsList = std::vector<std::vector<size_t>>;

class Parser
{
	static const std::set<wchar_t> regex_special_characters;

public:
	static std::shared_ptr<ParseTree> parse(const std::wstring& regex);

private:
	static std::shared_ptr<ParseTree> parse_regex(const SymbolArray& symbols, const std::vector<size_t>& bracket_pairs, const OptionsList& options_list, size_t start = 0, size_t end = 0);

	static std::shared_ptr<ParseTree> parse_simple_regex(const SymbolArray& symbols, const std::vector<size_t>& bracket_pairs, const OptionsList& options_list, size_t start, size_t end);

	static std::shared_ptr<ParseTree> parse_unit(const SymbolArray& symbols, const std::vector<size_t>& bracket_pairs, const OptionsList& options_list, size_t unit_start, size_t unit_end, size_t op_start = 0, size_t op_end = 0);

	static std::pair<int, int> parse_operator(const SymbolArray& symbols, size_t start, size_t end, bool& nullable);

	static IntervalSet parse_character_class(const SymbolArray& symbols, size_t start, size_t end);

	static IntervalSet get_named_character_class(const Symbol& symbol);

	static void process_escapes(const std::wstring& regex, SymbolArray& symbols);
};