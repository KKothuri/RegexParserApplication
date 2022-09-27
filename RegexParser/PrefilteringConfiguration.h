///////////////////////////////////////////////////////////////////
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.   //
///////////////////////////////////////////////////////////////////
#pragma once

#include <vector>

/// <summary> 
/// enum to store different types of boundary characters.
/// </summary>
enum DigitBoundary : unsigned char
{
	Alphabet = 1,                  // ~ 1
	Delimiter = 1 << 1,            // ~ 2
	Any = Alphabet | Delimiter,    // ~ 3
};

/// <summary>
/// Class to set-up pre-filtering configuration for regex and function processors.
/// </summary>
class PreFilteringConfiguration
{
public:
	/// <summary>
	/// Default Constructor
	/// </summary>
	PreFilteringConfiguration() :
		m_maxLength(-1),
		m_digitBoundaryOnLeft(Any),
		m_digitBoundaryOnRight(Any),
		m_skipFilter(true),
		m_deprecatedMaxConsecutiveDigits(0)
	{

	}

	/// <summary> 
	/// A vector to contain all the possible consecutive digits that a processor can expect e.g. the processor "\d{8,9}" can expect {8, 9} digits.
	/// </summary>
	std::vector<size_t> m_expectedDigits;

	/// <summary>
	/// Maximum length of an SIT e.g. "\d{8,9} has a max length of 9. -1, if max length depends on the content type. 
	/// </summary>
	int m_maxLength;

	/// <summary> 
	/// The type of character on the left boundary of the consecutive digits. 
	/// </summary>
	DigitBoundary m_digitBoundaryOnLeft;

	/// <summary> 
	/// The type of character on the right boundary of the consecutive digits. 
	/// </summary>
	DigitBoundary m_digitBoundaryOnRight;

	/// <summary> 
	/// Boolean to indicate pre-filtering status. True for skipping pre-filtering else false. 
	/// </summary>
	bool m_skipFilter;

	/// <summary> 
	///  To keep the previous functionality intact i.e. to store expected consecutive
	///  digits of each function and regex processor
	/// </summary>
	size_t m_deprecatedMaxConsecutiveDigits;
};