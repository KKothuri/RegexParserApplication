#include "IntervalSet.h"
#include <random>
#include <chrono>

void IntervalSet::insert(wchar_t element)
{
	// Find containing or previous interval
	auto iter = find_lower(element);

	// Element already present
	if (iter != m_intervals.end() && iter->first <= element && iter->second >= element)
	{
		return;
	}

	// Element doesn't have a lower interval
	if (iter == m_intervals.end())
	{
		if (!m_intervals.empty() && m_intervals.begin()->first == element + 1) // Element + 1 exists so add it to that interval
		{
			m_intervals.begin()->first = element;
		}
		else // Empty set or first element
		{
			m_intervals.insert(m_intervals.begin(), Interval(element, element));
		}
		return;
	}

	auto next = iter + 1;

	if (element == iter->second + 1 && next != m_intervals.cend() && element + 1 == next->first) // Element - 1  and + 1 exist causing merging of intervals
	{
		iter->second = next->second;
		m_intervals.erase(next);
	}
	else if (element == iter->second + 1) // Element - 1 exists
	{
		iter->second = element;
	}
	else if (next != m_intervals.cend() && element + 1 == next->first) // Element + 1 exists
	{
		next->first = element;
	}
	else
	{
		m_intervals.insert(iter + 1, Interval(element, element));
	}
}

void IntervalSet::insert_interval(wchar_t start, wchar_t inclusive_end)
{
	// Empty set
	if (m_intervals.empty())
	{
		m_intervals.push_back(Interval(start, inclusive_end));
		return;
	}

	// Find first interval affected by new interval
	auto start_iter = m_intervals.begin();
	while (start_iter != m_intervals.end() && start_iter->second < start - 1)
	{
		start_iter++;
	}

	// All current elements are lesser than start
	if (start_iter == m_intervals.end())
	{
		m_intervals.push_back(Interval(start, inclusive_end));
		return;
	}

	auto end_iter = start_iter;

	// Find last interval affected by new interval
	while (end_iter != m_intervals.end() && end_iter->second < inclusive_end)
	{
		end_iter++;
	}

	// Remove all elements till end and change the end of the first element
	if (end_iter == m_intervals.end())
	{
		start_iter->first = std::min(start_iter->first, start);
		start_iter->second = inclusive_end;
		m_intervals.erase(start_iter + 1, m_intervals.end());
		return;
	}

	start_iter->first = std::min(start, start_iter->first);
	if (end_iter->first > inclusive_end + 1)
	{
		start_iter->second = inclusive_end;
		if (start_iter + 1 <= end_iter - 1)
		{
			m_intervals.erase(start_iter + 1, end_iter - 1);
		}
	}
	else // End + 1 exists
	{
		start_iter->second = end_iter->second;
		if (start_iter + 1 <= end_iter)
		{
			m_intervals.erase(start_iter + 1, end_iter);
		}
	}

}

void IntervalSet::remove(wchar_t element)
{

}

void IntervalSet::remove_interval(wchar_t start, wchar_t inclusive_end)
{

}

bool IntervalSet::is_present(wchar_t element) const
{
	return find_element(element) != m_intervals.cend();
}

void IntervalSet::union_with(const IntervalSet& _other)
{
	// This set is empty
	if (m_intervals.empty())
	{
		m_intervals = _other.m_intervals;
		return;
	}

	// Other set is empty
	if (_other.m_intervals.empty())
	{
		return;
	}

	IntervalArray temp_vector;

	auto iter = m_intervals.begin();
	auto other_iter = _other.m_intervals.begin();

	// Insert first element before loop for convenience
	if (iter->first < other_iter->first)
	{
		temp_vector.push_back(*iter);
		iter++;
	}
	else
	{
		temp_vector.push_back(*other_iter);
		other_iter++;
	}

	// Merge loop
	while (iter != m_intervals.end() && other_iter != _other.m_intervals.end())
	{
		if (iter->first < other_iter->first)
		{
			if (iter->first <= temp_vector.back().second + 1) // Interval merge condition
			{
				temp_vector.back().second = iter->second;
			}
			else
			{
				temp_vector.push_back(*iter);
			}
			iter++;
		}
		else
		{
			if (other_iter->first <= temp_vector.back().second + 1) // Interval merge condition
			{
				temp_vector.back().second = other_iter->second;
			}
			else
			{
				temp_vector.push_back(*other_iter);
			}
			other_iter++;
		}
	}

	while (iter != m_intervals.end()) // Remaining elements processing
	{
		if (iter->first <= temp_vector.back().second + 1)
		{
			temp_vector.back().second = iter->second;
		}
		else
		{
			temp_vector.push_back(*iter);
		}
		iter++;
	}

	while (other_iter != _other.m_intervals.end()) // Remaining elements processing
	{
		if (other_iter->first <= temp_vector.back().second + 1)
		{
			temp_vector.back().second = other_iter->second;
		}
		else
		{
			temp_vector.push_back(*other_iter);
		}
		other_iter++;
	}

	m_intervals = temp_vector;
}

IntervalSet IntervalSet::intersection(const IntervalSet& _right) const
{
	IntervalSet result;

	auto iter = m_intervals.begin();
	auto other_iter = _right.m_intervals.begin();

	while (iter != m_intervals.end() && other_iter != _right.m_intervals.end())
	{
		while (iter->second < other_iter->first && iter != m_intervals.end())
		{
			iter++;
		}

		if (iter == m_intervals.end())
		{
			break;
		}
		
		while (other_iter->second < iter->first && other_iter != _right.m_intervals.end())
		{
			other_iter++;
		}

		if (other_iter == _right.m_intervals.end())
		{
			break;
		}

		result.insert_interval(std::max(iter->first, other_iter->first), std::min(iter->second, other_iter->second));

		if (iter->second > other_iter->second)
		{
			other_iter++;
		}
		else
		{
			iter++;
		}
	}

	return result;
}

void IntervalSet::invert()
{
	IntervalArray result;
	size_t start = 0, end = WCHAR_MAX;

	// Add all the gaps to the new set
	for (auto interval : m_intervals)
	{
		if (interval.first != start)
		{
			result.push_back(Interval(start, interval.first - 1));
		}
	}

	// Need to add additional checks
	result.push_back(Interval(start, end));

	m_intervals = result;
}

IntervalSet& IntervalSet::operator+=(const IntervalSet& _right)
{
	union_with(_right);
	return *this;
}

IntervalArray::const_iterator IntervalSet::find_element(wchar_t element) const
{
	// Empty set
	if (m_intervals.empty())
	{
		return m_intervals.cend();
	}

	// Binary search to find the interval
	size_t low = 0, high = m_intervals.size() - 1;

	while (low <= high)
	{
		size_t mid = (low + high) / 2;

		if (element <= m_intervals[mid].second && element >= m_intervals[mid].first) // Element found.
		{
			return m_intervals.cbegin() + mid;
		}
		else if (element > m_intervals[mid].second) // Element greater than the ones in current interval
		{
			low = mid + 1;
		}
		else if (element < m_intervals[mid].first) // Element lesser than the ones in current interval
		{
			high = mid - 1;
		}
	}

	// Not found
	return m_intervals.cend();
}

IntervalArray::iterator IntervalSet::find_lower(wchar_t element)
{
	// Empty set or all elements are greater tahn given element
	if (m_intervals.empty() || m_intervals[0].first > element)
	{
		return m_intervals.end();
	}

	// Binary search to find <= interval
	size_t low = 0, high = m_intervals.size() - 1;

	// If they're equal, we return low
	while (low < high)
	{
		// Take the higher mean because we don't want to be stuck in loop
		size_t mid = (low + high + 1) / 2;

		if (element <= m_intervals[mid].second && element >= m_intervals[mid].first) // Element found
		{
			return m_intervals.begin() + mid;
		}
		else if (element > m_intervals[mid].second) // Element greater than the ones in current interval
		{
			low = mid; // Since its a possible candidate, we make low = mid instead of mid + 1
		}
		else if (element < m_intervals[mid].first) // Element lesser than the ones in current interval
		{
			high = mid - 1;
		}
	}

	return m_intervals.begin() + low;
}

bool IntervalSet::empty() const
{
	return m_intervals.empty();
}

size_t IntervalSet::size() const
{
	size_t count = 0;
	for (const auto& iter : m_intervals)
	{
		count += iter.second - iter.first + 1;
	}

	return count;
}

wchar_t IntervalSet::get_nth_element(size_t index) const
{
	for (const auto& iter : m_intervals)
	{
		if (iter.second - iter.first + 1 < index)
		{
			index -= iter.second - iter.first + 1;
		}
		else
		{
			return iter.first + (wchar_t)index;
		}
	}

	throw;
}