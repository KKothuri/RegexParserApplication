#include "Parser.h"
#include <stack>

const std::set<wchar_t> Parser::regex_special_characters = {L'(', L')', L'{', L'}', L'-', L'[', L']', L'|', L'\\', L'?', L'*', L'+' , L'*'};

void Parser::process_escapes(const std::wstring& regex, SymbolArray& symbols)
{
	bool escaped = false;

	for (auto symbol : regex)
	{
		if (escaped)
		{
			symbols.push_back(Symbol(symbol, SymbolType::Escaped));
			escaped = false;
			continue;
		}
		if (symbol == L'\\')
		{
			escaped = true;
		}
		else
		{
			symbols.push_back(Symbol(symbol, SymbolType::Normal));
		}
	}
}

std::shared_ptr<ParseTree> Parser::parse(const std::wstring& regex)
{
	SymbolArray symbols;

	process_escapes(regex, symbols);

	std::vector<size_t> bracket_pairs(symbols.size());

	OptionsList options_list(symbols.size() + 1);

	std::stack<size_t> bracket_stack;

	size_t last_bracket = symbols.size();

	for (size_t iter = 0; iter < symbols.size(); ++iter)
	{
		const Symbol& symbol = symbols[iter];

		size_t bracket_close;

		if (symbol.type == SymbolType::Normal)
		{
			switch (symbol.symbol)
			{
			case L'(':
				bracket_stack.push(iter);
				break;
			case L'[':
				bracket_close = iter + 1;
				while (symbols[bracket_close].symbol != L']')
				{
					bracket_close++;
				}
				bracket_pairs[iter] = bracket_close;
				iter = bracket_close;
				break;
			case L'{':
				bracket_close = iter + 1;
				while (symbols[bracket_close].symbol != L'}')
				{
					bracket_close++;
				}
				bracket_pairs[iter] = bracket_close;
				iter = bracket_close;
				break;
			case L'|':
				if (bracket_stack.empty())
				{
					options_list[symbols.size()].push_back(iter);
				}
				else
				{
					options_list[bracket_stack.top()].push_back(iter);
				}
				break;
			case L')':
				bracket_pairs[bracket_stack.top()] = iter;
				bracket_stack.pop();
				break;
			default:
				break;
			}
		}
	}

	return parse_regex(symbols, bracket_pairs, options_list);
}

std::shared_ptr<ParseTree> Parser::parse_regex(const SymbolArray& symbols, const std::vector<size_t>& bracket_pairs, const OptionsList& options_list, size_t start, size_t end)
{
	if (start == end)
	{
		end = symbols.size();
	}

	if (bracket_pairs[start] == end - 1)
	{
		end--;
	}

	auto parseTree = std::make_shared<ParseTree>();
	parseTree->content = Regex;

	size_t options_index = start;
	if (start == 0 && end == symbols.size())
	{
		options_index = symbols.size();
	}

	const std::vector<size_t>& options = options_list[options_index];
	if (options.empty())
	{
		return parse_simple_regex(symbols, bracket_pairs, options_list, start, end);
	}
	else
	{
		auto option_start = options_index == symbols.size() ? start : start + 1;
		for (auto option : options)
		{
			parseTree->children.push_back(parse_simple_regex(symbols, bracket_pairs, options_list, option_start, option));
			option_start = option + 1;
		}
		parseTree->children.push_back(parse_simple_regex(symbols, bracket_pairs, options_list, option_start, end));
	}

	return parseTree;
}

std::shared_ptr<ParseTree> Parser::parse_simple_regex(const SymbolArray& symbols, const std::vector<size_t>& bracket_pairs, const OptionsList& options_list, size_t start, size_t end)
{
	if (bracket_pairs[start] == end)
	{
		start++;
	}

	auto parseTree = std::make_shared<ParseTree>();
	parseTree->content = SimpleRegex;

	size_t unit_start, unit_end, op_start, op_end;

	std::shared_ptr<ParseTree> child;

	for (size_t iter = start; iter < end; ++iter)
	{
		const Symbol& symbol = symbols[iter];

		if (symbol.type == SymbolType::Normal)
		{
			switch (symbol.symbol)
			{
			case L'(':
			case L'[':
				unit_start = iter;
				unit_end = bracket_pairs[iter];
				iter = unit_end;
				break;
			default:
				unit_start = unit_end = iter;
			}
		}
		else
		{
			unit_start = unit_end = iter;
		}

		if (iter + 1 != symbols.size() &&
			symbols[iter + 1].type == SymbolType::Normal &&
			(symbols[iter + 1].symbol == L'*' ||
				symbols[iter + 1].symbol == L'+' ||
				symbols[iter + 1].symbol == L'{' ||
				symbols[iter + 1].symbol == L'?'))
		{
			op_start = iter + 1;
			op_end = iter + 1;

			if (symbols[iter + 1].symbol == L'{')
			{
				op_end = bracket_pairs[iter + 1];
			}

			iter = op_end;

			child = parse_unit(symbols, bracket_pairs, options_list, unit_start, unit_end, op_start, op_end);
		}
		else
		{
			child = parse_unit(symbols, bracket_pairs, options_list, unit_start, unit_end);
		}

		if (child)
		{
			parseTree->children.push_back(child);
		}
	}

	return parseTree;
}

std::shared_ptr<ParseTree> Parser::parse_unit(const SymbolArray& symbols, const std::vector<size_t>& bracket_pairs, const OptionsList& options_list, size_t unit_start, size_t unit_end, size_t op_start, size_t op_end)
{
	auto parseTree = std::make_shared<ParseTree>();
	parseTree->content = Unit;

	const Symbol& symbol = symbols[unit_start];
	std::shared_ptr<ParseTree> child;

	if (symbol.type == SymbolType::Normal)
	{
		if (symbol.symbol == L'(')
		{
			child = parse_regex(symbols, bracket_pairs, options_list, unit_start, unit_end);

			if (child)
			{
				parseTree->children.push_back(child);
			}
		}
		else if (symbol.symbol == L'[')
		{
			parseTree->characterSet = parse_character_class(symbols, unit_start, unit_end);
		}
		else
		{
			parseTree->characterSet.insert(symbol.symbol);
		}
	}
	else
	{
		if (symbol.symbol == L'd' || symbol.symbol == L'D')
		{
			parseTree->characterSet = get_named_character_class(symbol);
		}
		else
		{
			parseTree->characterSet.insert(symbol.symbol);
		}
	}

	parseTree->repetition = parse_operator(symbols, op_start, op_end);

	return parseTree;
}

std::pair<int, int> Parser::parse_operator(const SymbolArray& symbols, size_t start, size_t end)
{
	if (start == 0 && end == 0)
	{
		return std::pair<int, int>(1, 1);
	}
	else if (start == end)
	{
		switch (symbols[start].symbol)
		{
		case L'*':
			return std::pair<int, int>(0, -1);
		case L'+':
			return std::pair<int, int>(1, -1);
		case L'?':
			return std::pair<int, int>(0, 1);
		default:
			throw;
		}
	}
	else
	{
		int min_rep = 0, max_rep = 0;
		bool min_done = false;
		for (size_t iter = start; iter <= end; ++iter)
		{
			if (symbols[iter].symbol >= L'0' && symbols[iter].symbol <= L'9')
			{
				if (min_done)
				{
					max_rep = max_rep * 10 + symbols[iter].symbol - L'0';
				}
				else
				{
					min_rep = min_rep * 10 + symbols[iter].symbol - L'0';
				}
			}
			else if (symbols[iter].symbol == L',')
			{
				min_done = true;
			}
		}

		if (min_done)
		{
			return std::pair<int, int>(min_rep, max_rep);
		}
		else
		{
			return std::pair<int, int>(min_rep, min_rep);
		}
	}
}

IntervalSet Parser::parse_character_class(const SymbolArray& symbols, size_t start, size_t end)
{
	IntervalSet result;
	for (size_t iter = start; iter < end; ++iter)
	{
		const Symbol& symbol = symbols[iter];

		if (symbol.type == SymbolType::Normal)
		{
			switch (symbol.symbol)
			{
			case L'[':
			case L']':
				break;
			case L'-':
				result.insert_interval(symbols[iter - 1].symbol, symbols[iter + 1].symbol);
				break;
			default:
				result.insert(symbol.symbol);
				break;
			}
		}
		else
		{
			if (symbol.symbol == L'd' || symbol.symbol == L'D')
			{
				result += get_named_character_class(symbol);
			}
			else
			{
				result.insert(symbol.symbol);
			}
		}
	}

	return result;
}

IntervalSet Parser::get_named_character_class(const Symbol& symbol)
{
	IntervalSet result;

	switch (symbol.symbol)
	{
	case L'd':
		result.insert_interval(L'0', L'9');
		break;
	case L'D':
		result.insert_interval(L'0', L'9');
		result.invert();
		break;
	}

	return result;
}