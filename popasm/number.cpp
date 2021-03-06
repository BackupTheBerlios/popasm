/** @file

	@brief Huge numbers definitions
	@author Helcio Mello (helcio@users.sourceforge.net)
	@since Fri Sep 7 2001

	This file contains NaturalNumber, IntegerNumber and RealNumber classes,
	which allow one to define arbitrarily large numbers and perform several
	mathematical operations on them.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
*/

#include "number.h"
#include <iostream>
#include <algorithm>

// Numbers default to be decimal
Word NaturalNumber::DefaultBase = 10;

/**
	@param d The value which must be converted to a NaturalNumber. Its 16 most significant bits
		are ignored in case they are null. If (d == 0) no storage is allocated at all.
	@return Nothing.
*/
NaturalNumber::NaturalNumber (Dword d = 0) throw ()
{
	// Null initial values require no storage
	if (d != 0) push_back (static_cast<Word> (d));
	d >>= 16;
	if (d != 0) push_back (static_cast<Word> (d));
}

/**
	@return Size of the number in bytes

	Note that if the most significant word has its most significant byte equal to zero, only the least
	significant one will be taken into account.

	Example: If the vector contents are [0x1234, 0x0056] the number is 0x561234
	and SizeInBytes() will return 3, not 4.
*/
Dword NaturalNumber::SizeInBytes() const throw ()
{
	Dword answer = size() * 2;

	if (answer == 0)
		return 0;

	// Ignores the most significant byte if it is zero
	if (back() < 256)
		answer--;

	return answer;
}

/**
	@return Size of the number in bits

	Note that only bits 0 thru the most significant one are taken into account.

	Example: If the vector contents are [0x1234, 0x0056] the number is 0x561234 =
	10101100001001000110100B and SizeInBits() will return 23, not 24, nor 32.
*/
Dword NaturalNumber::SizeInBits() const throw (LeadingZero)
{
	if (size() == 0) return 0;

	// Seeks the most significant bit within the most significant word.
	Word msw = back();
	for (Dword i = 16; i != 0;)
	{
		i--;
		if ((msw & (1 << i)) != 0) return (size() - 1) * 16 + i + 1;
	}

	throw LeadingZero();
}

/**
	@param s String to be handled
	@param Base A Word where this method will return the base of the number
	@param Negative A pointer to bool where this method will write true if the number is negative; false
		otherwise. If the pointer is NULL (0) then it is assumed the caller does not care about the sign
		and thus the pointer is not used.
	@throw InvalidNumber if the string is not a valid number
	@return Preprocessed string

	This method strips a numeric string from its sign, radix and leading or trailing underscores. It will also
	detect which base the number uses (DefautBase if not specified by means of a radix) and the size of the
	number.

	@attention Only leading and trailing underscores are removed. Underscores found between two digits are
		left untouched.

	Example: Preprocessing "-0x56_1234__" returns "56_1234" with Base = 16 and Negative
*/
string SimplifyString (const string &s, Word &Base, bool *Negative = 0) throw (InvalidNumber)
{
	// Find the radix (if any). Any number can be followed by as many underscores
	// as the programmer wishes, so 123h___ is a valid heximal number
	unsigned int i = 0, j = s.find_last_not_of ('_');
	Base = 0;

	// A string solely of underscores is not a number
	if (j == string::npos) throw InvalidNumber (s);
	j++;

	// Gets the sign
	if ((s[0] == '+') || (s[0] == '-'))
	{
		if (Negative != 0)
			*Negative = s[0] == '-';
		i++;
	}
	else
	{
		if (Negative != 0)
			*Negative = false;
	}

	if (i == j)
		throw InvalidNumber (s);

	// Leading '$' means hex...
	if (s[i] == '$')
	{
		i++;
		Base = 16;
		if (i == j)
			throw InvalidNumber (s);
		// ... if followed by at least one decimal digit
		if ((s[i] < '0') || (s[i] > '9'))
			throw InvalidNumber (s);
	}
	else
	{
		if (s[i] == '0')
		{
			i++;

			if (i != j)
			{
				if ((s[i] == 'x') || (s[i] == 'X'))
				{
					i++;

					// User has typed either '0x' or '0X'
					Base = 16;

					// "0x______" is an invalid number
					unsigned int k = s.find_first_not_of ("_", i);
					if ((k >= j) || (k == string::npos))
						throw InvalidNumber(s);
					if (!(((s[k] >= '0') && (s[k] <= '9')) || (s[k] == '_')))
						throw InvalidNumber(s);
					i = k;
				}
				else
					i--;
			}
			else
				i--;
		}
	}

	if (Base == 0)
	{
		// If no prefix was specified, the number must be led by a 0-9 digit
		if ((s[i] < '0') || (s[i] > '9'))
			throw InvalidNumber (s);

		// Translates the trailing radix to its base (i.e. h to 16, b to 2, etc)
		Base = NaturalNumber::IsRadix (s[j-1]);

		// The last digit is not a radix; assume default base
		if (Base == 0)
			Base = NaturalNumber::GetDefaultBase ();
		else
		{
			// Last character was a radix, so skip it
			j--;
		}
	}

	return string (s, i, j - i);
}

/**
	@param s The string to be parsed.
	@param ForcedBase The base of the number. If this value is zero then DefaultBase is used instead.
	@return Nothing
	@throw InvalidNumber If the supplied string does not hold a valid number.
	@throw InvalidDigit If any of the digits are not valid in the used base.

	This constructor converts a numeric string into a NaturalNumber. Any underscores found between digits
	are ignored (and can thus be used to make the number more readable).
*/
NaturalNumber::NaturalNumber (const string &s, Word ForcedBase = 0) throw (InvalidNumber, InvalidDigit)
{
	Word Base;
	string n;

	if (ForcedBase != 0)
	{
		// If a base was specified, use it and the string in an as-is basis
		Base = ForcedBase;
		n = s;
	}
	else
	{
		bool Negative;
		n = SimplifyString (s, Base, &Negative);

		// Negative numbers cannot be NaturalNumbers
		if (Negative)
			throw InvalidNumber (s);
	}

	// Now, it's time to convert the string to a number
	for (string::iterator i = n.begin(); i != n.end(); i++)
	{
		// If the separator is found, it must be ignored
		if (*i == '_')
			continue;

		// Multiply the current value by its base and add the new digit,
		// which is found by decoding its ASCII code to its value. Eg: 'F' to 15
		BaseShiftLeft (Base);

		Dword carry = DecodeDigit (*i, Base);
		for (unsigned int i = 0; carry != 0; i++)
		{
			// If the carry makes the number grow bigger...
			if (i != size())
			{
				carry += ZeroExtend ((*this)[i]);
				(*this)[i] = static_cast<Word> (carry);
			}
			else
			{
				// ... allocates more storage.
				push_back (static_cast<Word> (carry));
			}

			carry = SHR (carry, 16);
		}
	}

	// Prevents leading zero
	if (size() == 1)
	{
		if (back() == 0)
			pop_back();
	}
}

/**
	@param Base The base to be used to print the number. If ommitted, 10 will be assumed (i.e. the number
		will be printed in decimal.
	@return A string containing the number the way it would look if printed to screen.

	@attention The name of this method is confusing indeed. It prints nothing to the screen; actually it
	writes the number to a string. The string contains the number as it would be seen if printed in the
	specified base.
*/
string NaturalNumber::Print (Word Base = 10) const throw ()
{
	string s;
	Word d;

	NaturalNumber temp (*this);

	// Pushes each digit to a temporary string.
	do
	{
		d = temp.BaseShiftRight (Base);
		s += (EncodeDigit(d));
	} while (!temp.Zero());

	// Digits came from least to most significant. Order must be adjusted.
	reverse(s.begin(), s.end());
	return s;
}

/**
	@param Output The vector which the data must be sent to.
	@param n The length of output in bytes.
	@param pad The value to be used to pad the output to be as large as n. If not specified, 0 will be used.
	@return Nothing.

	@attention If the output vector is not empty the bytes output to the vector will be appended to it.

	Writes the number in little-endian form to an output, using n bytes, padding using the given byte.
*/
void NaturalNumber::Write (vector<Byte> &Output, unsigned int n = 0, Byte pad = 0) const
{
	NaturalNumber temp(*this);

	while ((!temp.Zero()) && (n != 0))
	{
		Output.push_back (static_cast<Byte> (*temp.begin()));
		temp.BaseShiftRight (256);
		n--;
	}

	Output.insert (Output.end(), n, pad);
}

/**
	@param Base The value the NaturalNumber is to be multiplied by.
	@return Nothing.

	This method is often used when one wishes to build a number from its algarisms.
	Eg. 123 = (((1 * 10) + 2) * 10) + 3
*/
void NaturalNumber::BaseShiftLeft (Word Base) throw ()
{
	Dword carry = 0;

	for (iterator i = begin(); i != end(); i++)
	{
		// Get the word multiplied by base and adds the carry
		// from the previous multiplication
		carry += ZeroExtend(*i) * ZeroExtend(Base);

		// The new word is the low word from the result above. The upper word
		// will be propagated ahead in the form of a carry
		*i = static_cast<Word> (carry);
		carry = SHR (carry, 16);
	}

	if (carry != 0) push_back (carry);
}

/**
	@param Base The value the NaturalNumber is to be divided by.
	@return The remainer of that division

	This method is often used when one wishes to get the digits of a number
	Eg. 3 = 123 MOD 10
	    2 = 12  MOD 10
		 1 = 1   MOD 10
*/
Word NaturalNumber::BaseShiftRight (Word Base) throw ()
{
	Dword carry = 0;

	// A right-shifted null value returns zero, of course
	if (empty()) return 0;

	for (iterator i = end(); i != begin();)
	{
		--i;

		// The dividend for the next division is a dword whose upper word is the
		// remainer of the previous division and the lower word the current one
		carry <<= 16;
		carry += ZeroExtend(*i);

		// Calculates new word and the remainer
		*i = carry / ZeroExtend(Base);
		carry = carry % ZeroExtend(Base);
	}

	if (back() == 0) pop_back();

	// Returns the final remainer
	return static_cast<Word> (carry);
}

/**
	@return true if number is even.

	The test is performed at the least significant word of the number, which can only be accessed if the number is not zero.
	We thus have a boundary condition: if the number is zero, return true, otherwise get its least significant word and test it.
*/
bool NaturalNumber::Even () const throw ()
{
	if (Zero()) return true;
	return (front() & 1) == 0;
}

/**
	@param n The exponent the number must be raised to
	@return The number raised to the nth power

	This method implements the russian peasant algorithm.
*/
const NaturalNumber NaturalNumber::Power (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer (1), Multiplier (*this), count(n);

	while (!count.Zero())
	{
		if (count.Odd()) answer *= Multiplier;
		Multiplier *= Multiplier;
		count >>= 1;
	}

	return answer;
}

/**
	@param n How many words (positions in the vector) the number will be shifted.
	@return Nothing.
*/
void NaturalNumber::WordShiftLeft (NaturalNumber n) throw ()
{
	// Something to shift?
	if (Zero() || n.Zero()) return;

	// Shifts the first n[0] words
	if (n[0] != 0)
		insert (begin(), static_cast<long int> (n[0]), 0);

	n.erase(n.begin());

	// Shifts the remaining words
	while (!n.Zero())
	{
		insert (begin(), static_cast<long int> (65536), 0);
		n--;
	}
}

/**
	@param n How many words (positions in the vector) the number will be shifted.
	@return Nothing.
*/
void NaturalNumber::WordShiftRight (NaturalNumber n) throw ()
{
	// Shifting more words than we have?
	if (n >= size())
	{
		clear();
		return;
	}

	// Something to shift?
	if (n.Zero()) return;

	// Shifts the first n[0] words
	if (n[0] != 0) erase (begin(), begin() + n[0]);
	n.erase(n.begin());

	// Shifts the remaining words
	while (!n.Zero())
	{
		remove (begin(), begin() + 65536, 0);
		n--;
	}
}

// Compares two numbers and returns +1 if *this is the greatest one, 0 if equal and -1 otherwise
int NaturalNumber::Compare (const NaturalNumber &n) const throw ()
{
	// If a number has more words it will always be greater
	if (size() > n.size()) return 1;
	if (size() < n.size()) return -1;

	const_iterator i = end();
	const_iterator j = n.end();

	// Compares each word from the most to the least significant one
	while (i != begin())
	{
		i--;
		j--;
		if (*i > *j) return 1;
		if (*i < *j) return -1;
	}

	// Identical numbers
	return 0;
}

// Returns if the number is zero
bool NaturalNumber::Zero() const throw (LeadingZero)
{
	// A zeroed NaturalNumber is an empty vector. Any leading zero is an internal error
	if (empty()) return true;
	if (back() != 0) return false;
	throw LeadingZero();
}

// Changes the default base for all numbers read from now on
void NaturalNumber::SetDefaultBase (Word NewBase) throw (InvalidBase)
{
	// Valid bases are 2, 8, 10 and 16
	if ((NewBase != 2) && (NewBase != 8) && (NewBase != 10) && (NewBase != 16))
		throw InvalidBase (NewBase);
	DefaultBase = NewBase;
}

Word NaturalNumber::DecodeDigit (char c, Word Base) throw (InvalidDigit)
{
	// Backup c in case an exception occurs
	char copy = c;

	// Convert the character to uppercase
	if ((c >= 'a') && (c <= 'z')) c -= 32;

	// Decodes digits from '0' to '9'.
	c -= '0';
	if (c > 9) c -= 7;

	// Checks for out of range digits
	if ((c < 0) || (c >= Base)) throw InvalidDigit (copy, Base);

	return c;
}

bool NaturalNumber::ValidDigit (char c, Word Base) throw ()
{
	// Convert the character to uppercase
	if ((c >= 'a') && (c <= 'z')) c -= 32;

	// Decodes digits from '0' to '9'.
	c -= '0';
	if (c > 9) c -= 7;

	// Checks for out of range digits
	return (c >= 0) && (c < Base);
}

char NaturalNumber::EncodeDigit (Byte d) throw ()
{
	if (d > 9) d += 7;
	return d + '0';
}

bool NaturalNumber::operator>  (const NaturalNumber &n) const throw ()
{
	return Compare (n) == 1;
}

bool NaturalNumber::operator>= (const NaturalNumber &n) const throw ()
{
	return Compare (n) != -1;
}

bool NaturalNumber::operator<  (const NaturalNumber &n) const throw ()
{
	return Compare (n) == -1;
}

bool NaturalNumber::operator<= (const NaturalNumber &n) const throw ()
{
	return Compare (n) != 1;
}

bool NaturalNumber::operator== (const NaturalNumber &n) const throw ()
{
	return Compare (n) == 0;
}

bool NaturalNumber::operator!= (const NaturalNumber &n) const throw ()
{
	return Compare (n) != 0;
}

// Prefix increment
NaturalNumber &NaturalNumber::operator++ () throw ()
{
	for (unsigned int i = 0;; i++)
	{
		// Checks if the increment causes a new word to be included
		if (i == size()) push_back (0);

		(*this)[i]++;
		// Checks for carry
		if ((*this)[i] != 0) break;
	}

	return *this;
}

// Postfix increment
const NaturalNumber NaturalNumber::operator++ (int) throw ()
{
	NaturalNumber copy(*this);
	return ++copy;
}

// Prefix
NaturalNumber &NaturalNumber::operator-- () throw (Underflow)
{
	if (Zero())
	{
		// If number is zero then throws -1 in two's complement
		push_back (0xFFFF);
		throw Underflow(*this);
	}

	// Decrements each word until borrows no longer occur
	for (unsigned int i = 0;; i++)
	{
		(*this)[i]--;
		if ((*this)[i] != 0xFFFF) break;
	}

	// Prevents leading zeroes, in case the decrement operation causes the most significant word to disappear
	if (back() == 0) pop_back();
	return *this;
}

const NaturalNumber NaturalNumber::operator-- (int) throw (Underflow)
{
	NaturalNumber copy(*this);
	return --copy;
}

void NaturalNumber::MatchSize (const NaturalNumber &n, Word w) throw ()
{
	if (size() < n.size())
		resize (n.size(), w);
}

NaturalNumber &NaturalNumber::operator~ () throw ()
{
	NaturalNumber answer (*this);
	return answer.OnesComplement();
}

NaturalNumber &NaturalNumber::OnesComplement (Dword sz = 0) throw (Overflow)
{
	// If the user accepts the default size (0), replace it for the real size in bytes
	if (sz == 0)
		sz = empty() ? 1 : SizeInBytes();
	else
		if (SizeInBytes() > sz) throw Overflow();

	// szw is the size in words
	Dword szw = sz / 2;
	if ((sz & 1) != 0) szw++;

	// Fills the number with leading zeroes so it get get to the desiref size
	resize (szw, 0);

	// Toggles all bits
	for (iterator i = begin(); i != end(); i++)
		*i ^= 0xFFFF;

	// If the number of bytes is odd, retoggles the most significant one
	if ((sz & 1) != 0) back() ^= 0xFF00;

	// Skips leading zeroes
	while (!empty())
	{
		if (back() == 0) pop_back();
		else break;
	}

	return *this;
}

NaturalNumber &NaturalNumber::TwosComplement (Dword sz = 0) throw (Overflow)
{
	if (empty())
		return *this;

	return ++OnesComplement(sz);
}

// Divides this by Divisor and returns the result. Optionally the remainer can
// be returned by a pointer specified by the caller
const NaturalNumber NaturalNumber::Divide (const NaturalNumber &n, NaturalNumber *Remainer = 0) const throw (DivisionByZero)
{
	int bits;
	NaturalNumber answer;
	NaturalNumber Dividend (*this);
	NaturalNumber Divisor (n);

	// Checks for division by zero
	if (Divisor.Zero()) throw DivisionByZero ();

	// Calculates how many bits wide the quotient will be
	for (bits = 1; Divisor < Dividend; bits++, Divisor <<= 1);

	// Performs division by sucessive subtractions
	for (int i = 0; i < bits; i++)
	{
		answer <<= 1;

		if (Dividend >= Divisor)
		{
			++answer;
			Dividend -= Divisor;
		}

		Divisor >>= 1;
	}

	// If the caller wishes to know the remainer...
	if (Remainer != 0) *Remainer = Dividend;
	return answer;
}

NaturalNumber &NaturalNumber::operator+= (const NaturalNumber &n) throw ()
{
	Dword carry;

	// Pads the first term with leading zeroes if necessary
	if (size() < n.size())
	{
		resize (n.size(), 0);
	}

	iterator i;
	const_iterator j;

	// Adds the corresponding words from *this and n
	for (i = begin(), j = n.begin(), carry = 0; j != n.end(); i++, j++)
	{
		carry += ZeroExtend(*i) + ZeroExtend(*j);

		// Sum the words and the carry from the last addition
		*i = static_cast<Word> (carry);

		// Updates carry value
		carry = SHR (carry, 16);
	}

	// Add any remaining carry
	for (i = begin() + n.size(); (i != end()) && (carry != 0); i++)
	{
		// Sum the words and the carry from the last addition
		carry += ZeroExtend(*i);
		*i = static_cast<Word> (carry);

		// Updates carry value
		carry = SHR (carry, 16);
	}

	if (carry != 0) push_back (static_cast<Word> (carry));
	return *this;
}

NaturalNumber &NaturalNumber::operator-= (const NaturalNumber &n) throw (Underflow)
{
	Dword carry = 0;

	// Pads the first term with leading zeroes if necessary
	if (size() < n.size()) resize (n.size(), 0);

	iterator i;
	const_iterator j;
	// Subtracts the corresponding words from *this and n
	for (i = begin(), j = n.begin(), carry = 0; j != n.end(); i++, j++)
	{
		// Sum the words and the carry from the last addition
		carry += ZeroExtend(*i) - ZeroExtend(*j);
		*i = static_cast<Word> (carry);

		// Updates carry value
		carry = SAR (carry, 16);
	}

	// Subtracts any remaining carry
	for (i = begin() + n.size(); (i != end()) && (carry != 0); i++)
	{
		// Sum the words and the carry from the last addition
		carry += ZeroExtend(*i);
		*i = static_cast<Word> (carry);

		// Updates carry value
		carry = SAR (carry, 16);
	}

	// Checks for underflow
	if (carry != 0)
	{
		// Underflow has occured. Checks the most significant bit, because if it zero and the carry is
		// 0xFFFF, its placement would be redundant
		if (!(carry == 0xFFFF) && ((back() & 0x8000) != 0))
			push_back (static_cast<Word> (carry));

		throw Underflow(*this);
	}

	// Skip leading zeroes
	while (!empty())
	{
		if (back() == 0) pop_back();
		else break;
	}

	return *this;
}

NaturalNumber &NaturalNumber::operator*= (const NaturalNumber &n) throw ()
{
	*this = *this * n;
	return *this;
}

NaturalNumber &NaturalNumber::operator/= (const NaturalNumber &n) throw (DivisionByZero)
{
	*this = *this / n;
	return *this;
}

NaturalNumber &NaturalNumber::operator%= (const NaturalNumber &n) throw (DivisionByZero)
{
	*this = *this % n;
	return *this;
}

NaturalNumber &NaturalNumber::operator<<= (const NaturalNumber &n) throw ()
{
	if (n.Zero()) return *this;

	Dword carry = 0;
	Word p = n[0] & 0xF;

	// First Shifts all words n % 16
	for (iterator i = begin(); i != end(); i++)
	{
		carry |= ZeroExtend (*i) << p;
		*i = static_cast <Word> (carry);
		carry = SHR (carry, 16);
	}

	if (carry != 0) push_back (static_cast<Word> (carry));

	// Now shifts the remaining n & 0xFF...FF0 bits
	WordShiftLeft (n >> 4);
	return *this;
}

NaturalNumber &NaturalNumber::operator>>= (const NaturalNumber &n) throw ()
{
	if (n.Zero()) return *this;

	Dword carry = 0;
	Word p = n[0] & 0xF;

	// Shifts each word n % 16
	iterator i = end();
	while (i != begin())
	{
		i--;
		carry |= *i;
		*i = SHR (carry, p);
		carry &= (1 << p) - 1;
		carry <<= 16;
	}

	// The last element can be examined only if the vector is non-empty
	if (!empty())
	{
		if (back() == 0) pop_back();
	}

	// Now shifts the remaining n & 0xFF...FF0 bits
	if ((n.Size() > 1) || (n[0] > 15)) WordShiftRight (n >> 4);
	return *this;
}

NaturalNumber &NaturalNumber::operator&= (const NaturalNumber &n) throw ()
{
	// If the first term is larger (in 16-bit words), resize it to be the same size of the other number
	if (size() > n.size()) resize (n.size(), 0);

	// Performs bitwise AND operation
	const_iterator j = n.begin();
	for (iterator i = begin(); i != end(); i++, j++)
		*i &= *j;

	// Skip leading zeroes
	while (!empty())
	{
		if (back() == 0) pop_back();
		else break;
	}

	return *this;
}

NaturalNumber &NaturalNumber::operator|= (const NaturalNumber &n) throw ()
{
	// If the first term is smaller (in 16-bit words), resize it to be the same size of the other number
	if (size() < n.size()) resize (n.size(), 0);

	// Performs the bitwise OR operation
	iterator i = begin();
	for (const_iterator j = n.begin(); j != n.end(); i++, j++)
		*i |= *j;

	// No possible leading zeroes to skip, so return now
	return *this;
}

NaturalNumber &NaturalNumber::operator^= (const NaturalNumber &n) throw ()
{
	// If the first term is smaller (in 16-bit words), resize it to be the same size of the other number
	if (size() < n.size()) resize (n.size(), 0);

	// Performs the bitwise XOR operation
	iterator i = begin();
	for (const_iterator j = n.begin(); j != n.end(); i++, j++)
		*i ^= *j;

	// Skip leading zeroes
	while (!empty())
	{
		if (back() == 0) pop_back();
		else break;
	}

	return *this;
}

const NaturalNumber NaturalNumber::operator+ (const NaturalNumber &n) const throw ()
{
	NaturalNumber temp (*this);
	return temp += n;
}

const NaturalNumber NaturalNumber::operator- (const NaturalNumber &n) const throw ()
{
	NaturalNumber temp (*this);
	return temp -= n;
}

const NaturalNumber NaturalNumber::operator* (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer;
	Dword carry;

	// Initializes space for the worst-case size
	answer.resize (size() + n.size(), 0);

	// Performs word-by-word multiplication
	for (unsigned int j = 0; j < n.size(); j++)
	{
		carry = 0;

		for (unsigned int i = 0; i < size(); i++)
		{
			carry += ZeroExtend ((*this)[i]) * ZeroExtend (n[j]) + ZeroExtend (answer[i+j]);
			answer[i+j] = static_cast <Word> (carry);
			carry = SHR (carry, 16);
		}

		if (carry != 0) answer[size() + j] += static_cast<Word> (carry);
	}

	// Skeep leading zeroes.
	while (!answer.empty())
	{
		if (answer.back() == 0) answer.pop_back();
		else break;
	}

	return answer;
}

const NaturalNumber NaturalNumber::operator/ (const NaturalNumber &n) const throw (DivisionByZero)
{
	return Divide(n);
}

const NaturalNumber NaturalNumber::operator% (const NaturalNumber &n) const throw (DivisionByZero)
{
	NaturalNumber Remainer;
	Divide (n, &Remainer);
	return Remainer;
}

const NaturalNumber NaturalNumber::operator<< (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer(*this);
	return answer <<= n;
}

const NaturalNumber NaturalNumber::operator>> (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer(*this);
	return answer >>= n;
}

const NaturalNumber NaturalNumber::operator&  (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer(*this);
	return answer &= n;
}

const NaturalNumber NaturalNumber::operator|  (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer(*this);
	return answer |= n;
}

const NaturalNumber NaturalNumber::operator^  (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer(*this);
	return answer ^= n;
}

void NaturalNumber::SetBit (Dword n) throw ()
{
	// Left pads with zeroes if needed.
	if (n / 16 >= size())
		resize (n / 16 + 1, 0);

	(*this)[n / 16] |= 1 << (n & 15);
}

void NaturalNumber::ClearBit (Dword n) throw ()
{
	// First check if the bit is beyond vactor bounds
	if (n / 16 < size())
		(*this)[n / 16] &= ~(1 << (n & 15));
}

bool NaturalNumber::TestBit (Dword n) throw ()
{
	// First check if the bit is beyond vactor bounds
	if (n / 16 < size())
		return ((*this)[n / 16] & (1 << (n & 15))) != 0;

	return false;
}

bool NaturalNumber::Test () throw ()
{
	try
	{
		NaturalNumber a ("516874561354986435213687967456h");
		NaturalNumber b (             "46851354984561320h");

		if ((a + b) * (a + b) != a * a + a * b * 2 + b * b) throw 0;
		if ((a + b) * (a - b) != a * a - b * b) throw 0;
		if ((a - b) * (a - b) != a * a - a * b * 2 + b * b) throw 0;
		if ((a + b) * (a + b) * (a + b) != a * a * a + a * a * b * 3 + a * b * b * 3 + b * b * b) throw 0;
		if ((a - b) * (a - b) * (a - b) != a * a * a - a * a * b * 3 + a * b * b * 3 - b * b * b) throw 0;
		if (a != (a / b) * b + (a % b)) throw 0;
	}
	catch (...)
	{
		return false;
	}

	return true;
}

Word NaturalNumber::IsRadix (char c) throw ()
{
	switch (c)
	{
		// Binary number, if default base is not 16
		case 'b':
		case 'B':
			// If the default radix is 16, the B is a digit, not a radix
			return (GetDefaultBase() == 16) ? 0 : 2;

		// Binary number, no matter what the default base is
		case 'y':
		case 'Y':
			return 2;

		// Octal number
		case 'o':
		case 'O':
		case 'q':
		case 'Q':
			return 8;

		// Hexadecimal number
		case 'h':
		case 'H':
			return 16;

		// Decimal number, if default base is not 16
		case 'd':
		case 'D':
			// If the default radix is 16, the D is a digit, not a radix
			return (GetDefaultBase() == 16) ? 0 : 10;

		// Decimal number no matter what the default base is
		case 't':
		case 'T':
			return 10;
	}

	return 0;
}

unsigned long int NaturalNumber::GetUnsignedLong () const throw (Overflow)
{
	unsigned long int value = 0;

	switch (Size())
	{
		case 2:
			value += static_cast<unsigned long int> ((*this)[1]) << 16;
		case 1:
			value += static_cast<unsigned long int> ((*this)[0]);
		case 0:
			break;

		default:
			throw Overflow ();
	}

	return value;
}

long int NaturalNumber::GetLong () const throw (Overflow)
{
	unsigned long int value = GetUnsignedLong();

	// Tests for overflow
	if (value >= 0x80000000)
		throw Overflow ();

	return static_cast<long int> (value);
}

//--- Integer Numbers

IntegerNumber::IntegerNumber (const string &s, Word ForcedBase = 0, bool ForcedSign = false) throw (InvalidNumber, InvalidDigit)
{
	if (ForcedBase == 0)
	{
		try
		{
			Word Base;
			string n (SimplifyString (s, Base, &Negative));
			AbsoluteValue = NaturalNumber (n, Base);
		}
		catch (...)
		{
			throw InvalidNumber (s);
		}
	}
	else
	{
		AbsoluteValue = NaturalNumber (s, ForcedBase);
		Negative = ForcedSign;
	}
}

IntegerNumber::IntegerNumber (long int i = 0) throw ()
{
	AbsoluteValue = NaturalNumber (static_cast<Dword> (abs(i)));
	Negative = i < 0;
}

const IntegerNumber IntegerNumber::Power (const NaturalNumber &n) const throw ()
{
	IntegerNumber answer (this->AbsoluteValue.Power (n));
	answer.Negative = Negative && n.Odd();

	return answer;
}

const IntegerNumber IntegerNumber::operator+  (const IntegerNumber &n) const throw ()
{
	IntegerNumber answer (*this);
	answer += n;
	return answer;
}

const IntegerNumber IntegerNumber::operator-  (const IntegerNumber &n) const throw ()
{
	IntegerNumber answer (*this);
	answer -= n;
	return answer;
}

const IntegerNumber IntegerNumber::operator*  (const IntegerNumber &n) const throw ()
{
	IntegerNumber answer (*this);
	answer *= n;
	return answer;
}

const IntegerNumber IntegerNumber::operator/  (const IntegerNumber &n) const throw (DivisionByZero)
{
	IntegerNumber answer (*this);
	answer /= n;
	return answer;
}

const IntegerNumber IntegerNumber::operator%  (const IntegerNumber &n) const throw (DivisionByZero)
{
	IntegerNumber answer (*this);
	answer %= n;

	return answer;
}

const IntegerNumber IntegerNumber::operator& (const IntegerNumber &n) const throw ()
{
	IntegerNumber answer (*this);
	return answer &= n;
}

const IntegerNumber IntegerNumber::operator| (const IntegerNumber &n) const throw ()
{
	IntegerNumber answer (*this);
	return answer |= n;
}

const IntegerNumber IntegerNumber::operator^ (const IntegerNumber &n) const throw ()
{
	IntegerNumber answer (*this);
	return answer ^= n;
}

IntegerNumber &IntegerNumber::operator+=  (const IntegerNumber &n) throw ()
{
	// Both of the same sign?
	if (Negative == n.Negative)
	{
		// Yes, keep their sign and add their absoluta values
		AbsoluteValue += n.AbsoluteValue;
	}
	else
	{
		// No, subtract them and use the sign of the higher absolute value
		if (AbsoluteValue > n.AbsoluteValue)
		{
			// (*this) has the greatest absolute value, so keep its sign
			AbsoluteValue -= n.AbsoluteValue;
		}
		else
		{
			// n has the the greatest absolute value, so keep its sign
			Negative = n.Negative;
			AbsoluteValue = n.AbsoluteValue - AbsoluteValue;
		}
	}

	return *this;
}

IntegerNumber &IntegerNumber::operator-=  (const IntegerNumber &n) throw ()
{
	// Both of the same sign?
	if (Negative != n.Negative)
	{
		// No, keep the sign of the first one and add their absoluta values
		AbsoluteValue += n.AbsoluteValue;
	}
	else
	{
		// Yes, so who's the greatest absolute value?
		if (AbsoluteValue > n.AbsoluteValue)
		{
			// The first has the greatest absolute value. Sign is left unchanged.
			AbsoluteValue -= n.AbsoluteValue;
		}
		else
		{
			// The second has the greatest absolute value. Sign must be inverted.
			Negative = !Negative;
			AbsoluteValue = n.AbsoluteValue - AbsoluteValue;
		}
	}

	return *this;
}

IntegerNumber &IntegerNumber::operator*=  (const IntegerNumber &n) throw ()
{
	// The result is negative if their signs are different
	AbsoluteValue *= n.AbsoluteValue;
	Negative = Negative != n.Negative;
	return *this;
}

IntegerNumber &IntegerNumber::operator/=  (const IntegerNumber &n) throw (DivisionByZero)
{
	// The result is negative if their signs are different
	AbsoluteValue /= n.AbsoluteValue;
	Negative = Negative != n.Negative;
	return *this;
}

IntegerNumber &IntegerNumber::operator%=  (const IntegerNumber &n) throw (DivisionByZero)
{
	// The result always hold the sign of the dividend
	AbsoluteValue %= n.AbsoluteValue;
	return *this;
}

const IntegerNumber IntegerNumber::operator<< (const IntegerNumber &n) const throw (NegativeShift)
{
	IntegerNumber answer (*this);
	answer <<= n;
	return answer;
}

const IntegerNumber IntegerNumber::operator>> (const IntegerNumber &n) const throw (NegativeShift)
{
	IntegerNumber answer (*this);
	answer >>= n;
	return answer;
}

IntegerNumber &IntegerNumber::operator<<= (const IntegerNumber &n) throw (NegativeShift)
{
	// Sign is left unchanged
	if (n.LesserThanZero())
		throw NegativeShift (*this, n);
	else
		AbsoluteValue <<= n.AbsoluteValue;

	return *this;
}

IntegerNumber &IntegerNumber::operator>>= (const IntegerNumber &n) throw (NegativeShift)
{
	// Sign is left unchanged
	if (n.LesserThanZero())
		throw NegativeShift (*this, n);
	else
	{
		// Performs arithmetic shift. Eg.: -3 >> 1 = -2, NOT -1
		if (Negative && AbsoluteValue.Odd()) AbsoluteValue++;
		AbsoluteValue >>= n.AbsoluteValue;
	}

	return *this;
}

// Sign-extends either *this or n so they have the same size. Assumes both numbers are in two`s complement notation
void IntegerNumber::SignExtend (IntegerNumber &n) throw ()
{
	if (AbsoluteValue.Size() < n.AbsoluteValue.Size())
	{
		AbsoluteValue.MatchSize (n.AbsoluteValue, LesserThanZero() ? 0xFFFF : 0);
	}
	else
	{
		n.AbsoluteValue.MatchSize (AbsoluteValue, n.LesserThanZero() ? 0xFFFF : 0);
	}
}

IntegerNumber &IntegerNumber::BinaryShiftRight (const IntegerNumber &n, Dword sz = 0) throw (NegativeShift, Overflow)
{
	// Prevents negative shifts
	if (n.LesserThanZero()) throw NegativeShift (*this, n);

	// If this number is negative convert it to its two's complement first
	if (LesserThanZero())
	{
		AbsoluteValue.TwosComplement(sz);
		// Checks sign bit. If not set, the absolute value was too large
		if (sz != 0)
			if (!AbsoluteValue.TestBit (sz * 8 - 1)) throw Overflow();
	}

	Negative = false;
	AbsoluteValue >>= n.AbsoluteValue;
	return *this;
}

IntegerNumber &IntegerNumber::UnsignedDivision (const IntegerNumber &n, Dword sz = 0) throw (Overflow)
{
	// If this number is negative convert it to its two's complement first
	if (LesserThanZero())
	{
		AbsoluteValue.TwosComplement(sz);
		// Checks sign bit. If not set, the absolute value was too large
		if (sz != 0)
			if (!AbsoluteValue.TestBit (sz * 8 - 1)) throw Overflow();
	}

	Negative = false;
	return (*this) /= n;
}

IntegerNumber &IntegerNumber::UnsignedModulus (const IntegerNumber &n, Dword sz = 0) throw (Overflow)
{
	// If this number is negative convert it to its two's complement first
	if (LesserThanZero())
	{
		AbsoluteValue.TwosComplement(sz);
		// Checks sign bit. If not set, the absolute value was too large
		if (sz != 0)
			if (!AbsoluteValue.TestBit (sz * 8 - 1)) throw Overflow();
	}

	Negative = false;
	return (*this) %= n;
	return *this;
}

IntegerNumber &IntegerNumber::operator&= (IntegerNumber n) throw ()
{
	// Converts any negative numbers to their two's complement notation
	if (Negative)
		AbsoluteValue.TwosComplement();

	if (n.Negative)
		n.AbsoluteValue.TwosComplement();

	// Sign extends the smaller number and performs the bitwise AND operation
	SignExtend (n);
	AbsoluteValue &= n.AbsoluteValue;
	Negative = Negative && n.Negative;

	// Converts the result back to its original form
	if (Negative) AbsoluteValue.TwosComplement();
	return *this;
}

IntegerNumber &IntegerNumber::operator|= (IntegerNumber n) throw ()
{
	// Converts any negative numbers to their two's complement notation
	if (Negative)
		AbsoluteValue.TwosComplement();

	if (n.Negative)
		n.AbsoluteValue.TwosComplement();

	// Sign extends the smaller number and performs the bitwise OR operation
	SignExtend (n);
	AbsoluteValue |= n.AbsoluteValue;
	Negative = Negative || n.Negative;

	// Converts the result back to its original form
	if (Negative) AbsoluteValue.TwosComplement();
	return *this;
}

IntegerNumber &IntegerNumber::operator^= (IntegerNumber n) throw ()
{
	// Converts any negative numbers to their two's complement notation
	if (Negative)
		AbsoluteValue.TwosComplement();

	if (n.Negative)
		n.AbsoluteValue.TwosComplement();

	// Sign extends the smaller number and performs the bitwise XOR operation
	SignExtend (n);
	AbsoluteValue ^= n.AbsoluteValue;
	Negative = Negative != n.Negative;

	// Converts the result back to its original form
	if (Negative) AbsoluteValue.TwosComplement();
	return *this;
}

const IntegerNumber IntegerNumber::operator- () const throw ()
{
	IntegerNumber answer (*this);
	answer.Negative = !Negative;
	return answer;
}

IntegerNumber &IntegerNumber::OnesComplement () throw ()
{
	// OnesComplement(+2) = -2 - 1 = -3
	// OnesComplement(-2) = +2 - 1 = +1
	ChangeSign();
	return --*this;
}

const IntegerNumber IntegerNumber::operator~ () const throw ()
{
	IntegerNumber answer (*this);
	return answer.OnesComplement();
}

IntegerNumber &IntegerNumber::operator++ () throw ()
{
	if (!Negative)
	{
		// Non-negative numbers can be incremented by incrementing their absolute values
		++AbsoluteValue;
	}
	else
	{
		if (AbsoluteValue.Zero())
		{
			// The increment of -0 is +1
			AbsoluteValue = NaturalNumber (1);
			Negative = false;
		}
		else
		{
			// Incrementing negative numbers consists in decrementing their absolute value
			--AbsoluteValue;
		}
	}

	return *this;
}

const IntegerNumber IntegerNumber::operator++ (int) throw ()
{
	IntegerNumber previous (*this);
	++(*this);
	return previous;
}

IntegerNumber &IntegerNumber::operator-- () throw ()
{
	if (Negative)
	{
		// Negative numbers can be decremented by incrementing their absolute values
		++AbsoluteValue;
	}
	else
	{
		if (AbsoluteValue.Zero())
		{
			// The decrement of +0 is -1
			AbsoluteValue = NaturalNumber (1);
			Negative = true;
		}
		else
		{
			// Decrementing negative numbers consists in incrementing their absolute value
			--AbsoluteValue;
		}
	}

	return *this;
}

const IntegerNumber IntegerNumber::operator-- (int) throw ()
{
	IntegerNumber previous (*this);
	--(*this);
	return previous;
}

int IntegerNumber::Compare (const IntegerNumber &n) const throw ()
{
	if (Zero() && n.Zero()) return 0;

	if (Negative)
	{
		if (n.Negative)
		{
			// Both are negetive numbers
			return -(AbsoluteValue.Compare (n.AbsoluteValue));
		}
		else
		{
			// First number is negative, but the second one is positive
			return -1;
		}
	}
	else
	{
		if (n.Negative)
		{
			// First number is positive, but the second one is negative
			return 1;
		}
		else
		{
			// Both are positive
			return AbsoluteValue.Compare (n.AbsoluteValue);
		}
	}
}

bool IntegerNumber::operator>  (const IntegerNumber &n) const throw ()
{
	return Compare (n) > 0;
}

bool IntegerNumber::operator>= (const IntegerNumber &n) const throw ()
{
	return Compare (n) >= 0;
}

bool IntegerNumber::operator<  (const IntegerNumber &n) const throw ()
{
	return Compare (n) < 0;
}

bool IntegerNumber::operator<= (const IntegerNumber &n) const throw ()
{
	return Compare (n) <= 0;
}

bool IntegerNumber::operator== (const IntegerNumber &n) const throw ()
{
	return Compare (n) == 0;
}

bool IntegerNumber::operator!= (const IntegerNumber &n) const throw ()
{
	return Compare (n) != 0;
}

string IntegerNumber::Print (Word Base = 10) const throw ()
{
	if (Negative) return string("-") + AbsoluteValue.Print (Base);
	return AbsoluteValue.Print (Base);
}

bool IntegerNumber::Zero() const throw (LeadingZero)
{
	return AbsoluteValue.Zero();
}

void IntegerNumber::Write (vector<Byte> &Output, unsigned int n) const
{
	NaturalNumber x (Abs());
	if (n != 10)
	{
		if (LesserThanZero()) x.TwosComplement();

		// Writes the number in two's complement if negative
		x.Write (Output, n, LesserThanZero() ? 0xFF : 0);
	}
	else
	{
		vector<Byte> Temp;
		Byte Next;

		// Converts all but the most significant pair of digits into a packed BCD byte
		for (unsigned int i = 0; i < 9; i++)
		{
			Next = static_cast<Byte> (x.BaseShiftRight (10));
			Next |= static_cast<Byte> (x.BaseShiftRight (10) << 4);
			Temp.push_back (Next);
		}

		// Writes the most significant byte (whose most significant bit is the size bit)
		Temp.push_back (Negative ? 0x80 : 0);

		if (!x.Zero()) throw Overflow();
		Output.insert (Output.end(), Temp.begin(), Temp.end());
	}
}

const IntegerNumber IntegerNumber::Divide (const IntegerNumber &n, IntegerNumber *Remainer = 0) const throw (DivisionByZero)
{
	IntegerNumber Quotient;

	if (Remainer != 0)
	{
		Quotient.AbsoluteValue = AbsoluteValue.Divide (n.AbsoluteValue, &(Remainer->AbsoluteValue));
		Remainer->Negative = Negative;
	}
	else
	{
		Quotient.AbsoluteValue = AbsoluteValue.Divide (n.AbsoluteValue, 0);		
	}

	// Quotient is positive if dividend and divisor have the same sign
	Quotient.Negative = Negative != n.Negative;
	return Quotient;
}

bool IntegerNumber::Test () throw ()
{
	try
	{
		IntegerNumber a ("-516874561354986435213687967456h");
		IntegerNumber b (             "-46851354984561320h");

		if ((a + b) * (a + b) != a * a + a * b * 2 + b * b) throw 0;
		if ((a + b) * (a - b) != a * a - b * b) throw 0;
		if ((a - b) * (a - b) != a * a - a * b * 2 + b * b) throw 0;
		if ((a + b) * (a + b) * (a + b) != a * a * a + a * a * b * 3 + a * b * b * 3 + b * b * b) throw 0;
		if ((a - b) * (a - b) * (a - b) != a * a * a - a * a * b * 3 + a * b * b * 3 - b * b * b) throw 0;
		if (a != (a / b) * b + (a % b)) throw 0;
	}
	catch (...)
	{
		return false;
	}

	return true;
}

// Returns the value of the number as an unsigned long int
unsigned long int IntegerNumber::GetUnsignedLong () const throw (Overflow)
{
	unsigned long int value = AbsoluteValue.GetUnsignedLong();

	// Throws overflow if number is negative
	if (Negative && (value != 0))
		throw Overflow();

	return value;
}

// Returns the value of the number as a long int
long int IntegerNumber::GetLong () const throw (Overflow)
{
	unsigned long int value = AbsoluteValue.GetUnsignedLong();

	// Checks valid range
	if (value > (Negative ? 0x80000000 : 0x7FFFFFFF)) throw Overflow();

	if (Negative)
		return -static_cast<long int> (value);
	else
		return static_cast<long int> (value);
}

//--- Real Numbers

Dword RealNumber::PrecisionIncrement = 8;
Dword RealNumber::MaximumSize = 8;
bool RealNumber::WatchPrecisionLoss = false;

NaturalNumber DigitCount (const string &s, NaturalNumber &Multiplicator, Word Base) throw ()
{
	NaturalNumber count;
	Multiplicator = NaturalNumber ();

	if (s.empty ()) return count;

	// Calculates how many times the number must be multiplied by 10 to make the mantissa an integer
	switch (Base)
	{
		case 2:
			Multiplicator = NaturalNumber (1);
			break;

		case 8:
			Multiplicator = NaturalNumber (3);
			break;

		case 16:
			Multiplicator = NaturalNumber (4);
			break;
	}

	// Let's count all possible digits. Let the current base be 16. If it is not, an InvalidDigit exception
	// will be caught elsewhere if an invalid digit is found.
	for (string::const_iterator i = s.begin(); i != s.end(); i++)
	{
		if (NaturalNumber::ValidDigit (*i, 16)) count++;
	}

	Multiplicator *= count;
	if (Base != 10) return Multiplicator;
	return count;
}

RealNumber::RealNumber (const string &s, Word ForcedBase = 0, bool ForcedSign = false) throw (InvalidNumber, InvalidDigit)
{
	Word Base;
	bool Negative;
	string n;

	if (ForcedBase == 0)
	{
		n = SimplifyString (s, Base, &Negative);
	}
	else
	{
		Base = ForcedBase;
		Negative = ForcedSign;
		n = s;
	}

	// Search for the decimal point (if any)
	unsigned int DotPosition = n.find('.');

	// Checks if the user has typed something like 1e4 (= 1.0e+4)
	unsigned int EPosition = n.find_first_of ("eE");

	try
	{
		if ((Base == 10) && (EPosition != string::npos))
		{
			// The user has typed something like 12.24e-3, for example
			RealNumber temp (string (n, 0, EPosition), Base, Negative);
			Mantissa = temp.Mantissa;
			Integer = false;

			// Searches the next digit after the 'e'
			unsigned int i = n.find_first_not_of ('_', EPosition + 1);
			if (i == string::npos) throw (InvalidNumber (s));

			bool Negative = false;
			if ((n[i] == '-') || (n[i] == '+'))
			{
				Negative = n[i] == '-';
				i++;
			}

			Exponent = temp.Exponent + IntegerNumber (string (n, i), Base, Negative);
		}
		else
		{
			if (DotPosition == string::npos)
			{
				// Integer number in a non-10 base
				Mantissa = IntegerNumber (n, Base, Negative);
				Integer = true;
			}
			else
			{
				// Fraction in a non-10 base
				n[DotPosition] = '_';
				Mantissa = IntegerNumber (n, Base, Negative);
				Integer = false;

				NaturalNumber Multiplicator;

				// Calculates the mantissa and the exponent of the number
				Exponent = DigitCount (string (n, DotPosition + 1), Multiplicator, Base);
				Exponent.ChangeSign();
				Mantissa *= (IntegerNumber (5)).Power (Multiplicator);
			}
		}
	}
	catch (...)
	{
		throw InvalidNumber (s);
	}
}

string RealNumber::Print (Word Base = 10, Dword n = 8) const throw ()
{
	NaturalNumber temp (Mantissa.Abs());
	NaturalNumber divisor = NaturalNumber (10).Power (Exponent.Abs());
	NaturalNumber remainer;
	string s;

	// Prints a dash if the number is negative
	if (Mantissa.LesserThanZero()) s += '-';

	// If exponent is positive, performs the calculation Mantissa * Base ^ n. The result will be an integer
	if (Exponent >= 0)
	{
		// If exponent is greater than zero we have an integer
		temp *= divisor;
		s += temp.Print(Base);

		// Checks if the user requested the float type explicitly
		if (!Integer) s += ".0";
		return s;
	}

	s += temp.Divide (divisor, &remainer).Print(Base);
	s += '.';

	// Prints each digit as long as there are any remaining. Stop if the limit was achieved
	for (Word i = 0; i < n; i++)
	{
		remainer.BaseShiftLeft (Base);
		s += remainer.Divide (divisor, &remainer).Print(Base);
		if (remainer.Zero()) break;
	}

	return s;
}

void RealNumber::Write (vector<Byte> &Output, unsigned int n) const
{
	if (IsInteger())
	{
		(Mantissa * NaturalNumber (10).Power(Exponent.Abs())).Write (Output, n);
	}
	else
	{
		bool ImpliedOne;
		Dword MantissaSize, ExponentSize;

		// +0.0 and -0.0 have special representations
		if (Zero())
		{
			Output.insert (Output.end(), n - 1, 0);
			Output.push_back (Mantissa.GetNegativeBit() ? 0x80 : 0);
			return;
		}

		switch (n)
		{
			case 4:
				ImpliedOne = true;
				MantissaSize = 24;
				ExponentSize = 8;
				break;

			case 8:
				ImpliedOne = true;
				MantissaSize = 53;
				ExponentSize = 11;
				break;

			case 10:
				ImpliedOne = false;
				MantissaSize = 64;
				ExponentSize = 15;
				break;

			// Only 32- 48- and 80- bit formats are available
			default: throw (InvalidFormat(n));
		}

		NaturalNumber m(Mantissa.Abs());
		NaturalNumber e(NaturalNumber(10).Power(Exponent.Abs()));
		Dword count = 0;
		Dword bias = (1 << (ExponentSize - 1)) - 1;

		if (Exponent.LesserThanZero())
		{
			// Negative exponent
			Dword x = MantissaSize + e.SizeInBits();
			Dword y = m.SizeInBits();

			// If mantissa too small, shifts it left to make it larger
			if (x >= y)
			{
				x = x - y + 1;
				m <<= x;
				count = x;
			}

			// Evaluates m * 10^(-e)
			m /= e;

			// Shifts all needed bits, except the last one
			x = m.SizeInBits() - MantissaSize;
			m >>= x - 1;

			// Rounds the result up if necessary
			if (m.Odd()) m++;
			m >>= 1;
			count -= x;

			// Checkks whether the rounding generated a carry
			if (m.SizeInBits() > MantissaSize)
			{
				count--;
				m >>= 1;
			}

			// Calculates biased exponent
			count = bias + MantissaSize - count - 1;
		}
		else
		{
			// Positive exponent. Performs mantissa * 10^exponent
			m *= e;
			count = m.SizeInBits();

			if (count > MantissaSize)
			{
				// Shifts right the result to fit the mantissa size, rounding the result
				m >>= count - MantissaSize - 1;
				if (m.Odd()) m++;
				m >>= 1;

				// Checkks whether the rounding generated a carry
				if (m.SizeInBits() > MantissaSize)
				{
					count++;
					m >>= 1;
				}
			}
			else
			{
				// Shifts left the mantissa to make it as large as needed
				m <<= MantissaSize - count;
			}

			// Calculates biased exponent
			count += bias - 1;
		}

		// Checks for overflows and denormals
		if ((count >= 2 * bias + 1) || (count == 0))
		{
			// Attempts to convert the number to denormal
			count += MantissaSize - 1;
			if (count >= 2 * bias + 1) throw Overflow();

			// Shitfs the mantissa right so that the exponent can be zero
			m >>= MantissaSize - count;
			count = 0;
		}

		// Removes implicit 1 if any
		if (ImpliedOne) m.ClearBit (--MantissaSize);

		// Writes de exponent field
		m |= NaturalNumber (count) << MantissaSize;

		// Sets sign bit
		if (Mantissa.GetNegativeBit()) m.SetBit (8 * n - 1);
		m.Write (Output, n);
	}
}

// Returns the value of the number as an unsigned long int
unsigned long int RealNumber::GetUnsignedLong () const throw (Overflow, IntegerExpected)
{
	if (!Integer) throw IntegerExpected (*this);

	if (Exponent.LesserThanZero())
		return IntegerNumber (Mantissa / NaturalNumber (10).Power (Exponent.Abs())).GetUnsignedLong();
	else
		return IntegerNumber (Mantissa * NaturalNumber (10).Power (Exponent.Abs())).GetUnsignedLong();
}

// Returns the value of the number as a long int
long int RealNumber::GetLong () const throw (Overflow, IntegerExpected)
{
	if (!Integer) throw IntegerExpected (*this);

	if (Exponent.LesserThanZero())
		return IntegerNumber (Mantissa / NaturalNumber (10).Power (Exponent.Abs())).GetLong();
	else
		return IntegerNumber (Mantissa * NaturalNumber (10).Power (Exponent.Abs())).GetLong();
}

void RealNumber::Adjust (RealNumber &n) throw ()
{
	IntegerNumber b (10);
	IntegerNumber diff (Exponent - n.Exponent);

	// Multiplies the number with lesser exponent by 10^(difference between exponents)
	if (diff.GreaterThanZero())
	{
		Exponent = n.Exponent;
		Mantissa *= b.Power (diff.Abs());
	}
	else
	{
		n.Exponent = Exponent;
		n.Mantissa *= b.Power (diff.Abs());
	}
}

RealNumber &RealNumber::operator+= (const RealNumber &n) throw ()
{
	RealNumber r(n);

	Adjust (r);
	Mantissa += r.Mantissa;
	Integer = Integer && n.Integer;
	return *this;
}

RealNumber &RealNumber::operator-= (const RealNumber &n) throw ()
{
	RealNumber r(n);

	Adjust (r);
	Mantissa -= r.Mantissa;
	Integer = Integer && n.Integer;
	return *this;
}

RealNumber &RealNumber::operator*= (const RealNumber &n) throw ()
{
	Mantissa *= n.Mantissa;
	Exponent += n.Exponent;
	Integer = Integer && n.Integer;
	return *this;
}

RealNumber &RealNumber::operator/= (const RealNumber &n) throw (DivisionByZero, PrecisionLoss)
{
	*this = (*this) / n;
	return *this;
}

const RealNumber RealNumber::operator+ (const RealNumber &n) const throw ()
{
	RealNumber copy (*this);
	copy += n;
	return copy;
}

const RealNumber RealNumber::operator- (const RealNumber &n) const throw ()
{
	RealNumber copy (*this);
	copy -= n;
	return copy;
}

const RealNumber RealNumber::operator* (const RealNumber &n) const throw ()
{
	RealNumber copy (*this);
	copy *= n;
	return copy;
}

const RealNumber RealNumber::operator/ (const RealNumber &n) const throw (DivisionByZero, PrecisionLoss)
{
	IntegerNumber Remainer;
	RealNumber Quotient;

	Quotient.Exponent = Exponent - n.Exponent;
	Quotient.Mantissa = Mantissa.Divide (n.Mantissa, &Remainer);
	Quotient.Integer = Integer && n.Integer;

	// If we have an exact division, fine.
	if (Remainer.Zero()) return Quotient;
	Quotient.Integer = false;
	IntegerNumber Multiplier = IntegerNumber (10).Power (PrecisionIncrement);

	// As long as there is some remainer and room for more digits, keep dividing
	while ((!Remainer.Zero()) && (Quotient.Size() < MaximumSize))
	{
		Remainer *= Multiplier;
		Quotient.Mantissa *= Multiplier;
		Quotient.Exponent -= PrecisionIncrement;

		Quotient.Mantissa += Remainer.Divide (n.Mantissa, &Remainer);
	}

	// If the user wants an exception due to precision loss and we have one...
	if (WatchPrecisionLoss && (!Remainer.Zero()))
	{
		RealNumber r (Remainer, Quotient.Exponent - PrecisionIncrement);
		throw PrecisionLoss (Quotient, r);
	}

	return Quotient;
}

RealNumber::operator IntegerNumber () const throw (PrecisionLoss)
{
	if (Exponent.LesserThanZero())
	{
		// If exponent is negative, there may be a precision loss, as in 1230.0e-2
		IntegerNumber r;
		IntegerNumber d = IntegerNumber (10).Power (Exponent.Abs());
		IntegerNumber q = Mantissa.Divide (d, &r);

		// If the user wants an exception due to precision loss and we have one...
		if (WatchPrecisionLoss && (!r.Zero()))
		{
			throw PrecisionLoss (q, RealNumber (r, Exponent));
		}

		return q;
	}
	else
	{
		// If exponent is greater than zero, just return Mantissa * Exponent ^ 10
		return Mantissa * IntegerNumber (10).Power (Exponent.Abs());
	}
}

int RealNumber::Compare (const RealNumber &n) const throw ()
{
	if (Mantissa.GreaterThanZero())
	{
		// A positive number is always greater than a negative one
		if (n.Mantissa <= 0) return 1;

		// If signs differ, equalizes exponents and compares mantissas
		RealNumber a(*this), b(n);
		a.Adjust (b);
		return a.Mantissa.Compare (b.Mantissa);
	}
	else
	{
		// A non-positive number is always lesser than a positive one
		if (n.Mantissa.GreaterThanZero()) return -1;

		// If both are negative, the one with lesser magnitude is greater
		RealNumber a(*this), b(n);
		a.Adjust (b);
		return -(a.Mantissa.Compare (b.Mantissa));
	}
}

bool RealNumber::operator>  (const RealNumber &n) const throw ()
{
	return Compare (n) > 0;
}

bool RealNumber::operator>= (const RealNumber &n) const throw ()
{
	return Compare (n) >= 0;
}

bool RealNumber::operator<  (const RealNumber &n) const throw ()
{
	return Compare (n) < 0;
}

bool RealNumber::operator<= (const RealNumber &n) const throw ()
{
	return Compare (n) <= 0;
}

bool RealNumber::operator== (const RealNumber &n) const throw ()
{
	return Compare (n) == 0;
}

bool RealNumber::operator!= (const RealNumber &n) const throw ()
{
	return Compare (n) != 0;
}

bool RealNumber::Test () throw ()
{
	try
	{
		RealNumber a ("-51687456135498643521368796.7456h");
		RealNumber b (             "-46851354984561.320h");

		if ((a + b) * (a + b) != a * a + a * b * 2 + b * b) throw 0;
		if ((a + b) * (a - b) != a * a - b * b) throw 0;
		if ((a - b) * (a - b) != a * a - a * b * 2 + b * b) throw 0;
		if ((a + b) * (a + b) * (a + b) != a * a * a + a * a * b * 3 + a * b * b * 3 + b * b * b) throw 0;
		if ((a - b) * (a - b) * (a - b) != a * a * a - a * a * b * 3 + a * b * b * 3 - b * b * b) throw 0;
	}
	catch (...)
	{
		return false;
	}

	return true;
}
