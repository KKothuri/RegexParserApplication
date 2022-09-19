#pragma once

#include <vector>
#include <utility>

using Interval = std::pair<wchar_t, wchar_t>;
using IntervalArray = std::vector<Interval>;

class IntervalSet
{
public:
	IntervalSet() {};
	
	// Insert an element into the set.
	void insert(wchar_t element);

	// Insert an interval of elements into the set.
	void insert_interval(wchar_t start, wchar_t inclusive_end);

	// Remove an element from the set.
	void remove(wchar_t element);

	// Remove an interval of elements from the set.
	void remove_interval(wchar_t start, wchar_t inclusive_end);

	// Return true if the set contains the input element.
	bool is_present(wchar_t element) const;

	// Union this set with the input set.
	void union_with(const IntervalSet& _other);

	// Invert the current set
	void invert();

	// Calls the union function
	IntervalSet& operator+=(const IntervalSet& _right);

	friend IntervalSet operator+(IntervalSet _left, const IntervalSet& _right)
	{
		_left += _right;
		return _left;
	}

	friend inline bool operator==(const IntervalSet& _left, const IntervalSet& _right)
	{
		return _left.m_intervals == _right.m_intervals;
	}

	friend inline bool operator!=(const IntervalSet& _left, const IntervalSet& _right)
	{
		return !(_left == _right);
	}

private:

	// Return iterator for the interval containing the input element. Return end() if not found.
	IntervalArray::const_iterator find_element(wchar_t element) const;

	// Return iterator for the interval containing the highest element <= input element. Return end() if set is empty or all elements are higher than element.
	IntervalArray::iterator find_lower(wchar_t element);

	// Array to store the set elements as intervals.
	IntervalArray m_intervals;
};