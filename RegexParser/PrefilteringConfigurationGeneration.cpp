#include "PrefilteringConfigurationGeneration.h"
#include <set>

bool IsAllDigits(const IntervalSet& characterSet)
{
	auto count = characterSet.size();
	wchar_t first, last;
	first = characterSet.get_nth_element(0);
	last = characterSet.get_nth_element(count - 1);

	if (first >= L'0' && first <= L'9' && last >= L'0' && last <= L'9')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool HasDigits(const IntervalSet& characterSet)
{
	for (wchar_t digit = L'0'; digit <= L'9'; ++digit)
	{
		if (characterSet.is_present(digit))
		{
			return true;
		}
	}
	return false;
}

PreFilteringConfiguration GeneratePreFilteringConfiguration(const std::shared_ptr<ParseTree>& parseTree, bool& alldigits)
{
	PreFilteringConfiguration result;

	if (parseTree->content == Regex)
	{
		for (const auto& child : parseTree->children)
		{
			GeneratePreFilteringConfiguration(child, alldigits);
		}
	}
	else if (parseTree->content == SimpleRegex)
	{
		alldigits = true;

		bool* child_alldigits = new bool[parseTree->children.size()];
		std::vector<PreFilteringConfiguration> child_configs(parseTree->children.size());
		
		for (size_t iter = 0; iter < parseTree->children.size(); ++iter)
		{
			const auto& child = parseTree->children[iter];
			child_configs[iter] = GeneratePreFilteringConfiguration(child, *(child_alldigits + iter));
		}

		result.m_skipFilter = false;
		result.m_maxLength = 0;
		result.m_deprecatedMaxConsecutiveDigits = 0;

		bool digits_done = false;

		for (size_t iter = 0; iter < parseTree->children.size(); ++iter)
		{
			alldigits = alldigits && child_alldigits[iter];
			result.m_skipFilter = result.m_skipFilter || child_configs[iter].m_skipFilter;
			if (result.m_maxLength != -1 && child_configs[iter].m_maxLength != -1)
			{
				result.m_maxLength += child_configs[iter].m_maxLength;
			}
			else
			{
				result.m_maxLength = -1;
			}
			if (iter == 0)
			{
				result.m_expectedDigits = child_configs[iter].m_expectedDigits;
			}
			else if (child_alldigits[iter] == true && !digits_done)
			{
				std::set<size_t> ans;
				for (int i = 0; i < result.m_expectedDigits.size(); i++)
				{
					for (int j = 0; j < child_configs[iter].m_expectedDigits.size(); j++)
					{
						ans.insert(result.m_expectedDigits[i] + child_configs[iter].m_expectedDigits[j]);
					}
				}
				result.m_expectedDigits = std::vector<size_t>(ans.begin(), ans.end());
			}

			if (child_alldigits[iter] == false)
			{
				digits_done = true;
			}
		}
	}
	else
	{
		if (!parseTree->children.empty())
		{
			bool single_alldigits;
			PreFilteringConfiguration config = GeneratePreFilteringConfiguration(parseTree->children[0], single_alldigits);

			result.m_skipFilter = config.m_skipFilter;
			result.m_deprecatedMaxConsecutiveDigits = config.m_deprecatedMaxConsecutiveDigits;
			if (config.m_maxLength != -1)
			{
				result.m_maxLength = config.m_maxLength * parseTree->repetition.second;
			}
			if (config.m_expectedDigits.empty() || config.m_maxLength > config.m_expectedDigits.back())
			{
				result.m_expectedDigits = config.m_expectedDigits;
			}
			else
			{
				for (int iter = parseTree->repetition.first; iter <= parseTree->repetition.second; ++iter)
				{
					if (iter == 0)
					{
						continue;
					}

					for (auto& digit : config.m_expectedDigits)
					{
						result.m_expectedDigits.push_back(result.m_maxLength * (iter - 1) + digit);
					}
				}
			}

			return result;
		}

		alldigits = IsAllDigits(parseTree->characterSet);
		bool hasdigits;

		if (!alldigits)
		{
			hasdigits = HasDigits(parseTree->characterSet);
		}
		else
		{
			hasdigits = alldigits;
		}

		if (alldigits)
		{
			if (parseTree->repetition.second != -1)
			{
				for (int iter = parseTree->repetition.first; iter <= parseTree->repetition.second; ++iter)
				{
					result.m_expectedDigits.push_back(iter);
				}
				result.m_maxLength = parseTree->repetition.second;
				result.m_deprecatedMaxConsecutiveDigits = parseTree->repetition.first;
				result.m_skipFilter = false;
			}
			else
			{
				result.m_deprecatedMaxConsecutiveDigits = parseTree->repetition.first;
			}
		}
		else if (parseTree->repetition.second != -1)
		{
			if (hasdigits)
			{
				for (int iter = 0; iter <= parseTree->repetition.second; ++iter)
				{
					result.m_expectedDigits.push_back(iter);
				}
			}
			result.m_skipFilter = false;
			result.m_maxLength = parseTree->repetition.second;
		}
	}

	return result;
}