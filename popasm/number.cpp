/***************************************************************************
                          number.cpp  -  description
                             -------------------
    begin                : Fri Sep 7 2001
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "number.h"
#include <iostream>
#include <algorithm>

// Numbers default to be decimal
Word NaturalNumber::DefaultBase = 10;

// Enables the construction of a number given its initial value
NaturalNumber::NaturalNumber (const Dword d = 0) throw ()
{
	// Null initial values require no storage
	if (d != 0) push_back (d);
}

// Translates the contents of the string to a possibly huge number
NaturalNumber::NaturalNumber (const string &s, const Word ForcedBase = 0) throw (InvalidNumber, InvalidDigit)
{
	// Assures no empty strings get past this point
	if (s.empty()) return;

	Word Base;
	string n;
  	unsigned int RadixPosition;

	// Size must be at least 2 if 0x or $n are used
	if (s.size() > 1)
	{
		// Did the user say 0x...?
		if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X')))
		{
			// "0x" and "0X" are not numbers themselves
			if ((s.size() == 2) || (ForcedBase != 0)) throw InvalidNumber (s);

			Base = 16;
			n = string (s.begin() + 2, s.end());
			goto CalculateNumber;
		}

		// A $ followed by a number is hex as well
		if (s[0] == '$')
		{
			if ((s[1] < '0') || (s[1] > '9') || (ForcedBase != 0)) throw InvalidNumber (s);
			Base = 16;
			n = string (s.begin() + 1, s.end());
			goto CalculateNumber;
		}
	}

	// Checks if the first character is a digit; otherwise it's not a number
	if ((s[0] < '0') || (s[0] > '9')) throw InvalidNumber (s);

	// Find the radix (if any). Any number can be followed by as many underscores
	// as the programmer wishes, so 123h___ is a valid heximal number
	RadixPosition = s.find_last_not_of ('_');

	// Translates the trailing radix to its base (i.e. h to 16, b to 2, etc)
	Base = IsRadix (s[RadixPosition]);
	if (Base == 0)
	{
		// If no radix was specified, use the forced base. If none is forced, use the default one
		Base = (ForcedBase == 0) ? GetDefaultBase () : ForcedBase;
		n = string (s, 0, RadixPosition + 1);
	}
	else
	{
		// It is an error to force a base and specify a radix simultaneously
		if (ForcedBase != 0) throw InvalidNumber (s);
		n = string (s, 0, RadixPosition);
	}

CalculateNumber:
	// Initializes the number to zero. Don't worry, any leading zeroes will be skipped later
	push_back (0);

	// Now, it's time to convert the string to a number
	for (string::iterator i = n.begin(); i != n.end(); i++)
	{
		// If the separator is found, it must be ignored
		if (*i == '_') continue;

		// Multiply the current value by its base and add the new digit,
		// which is found by decoding its ASCII code to its value. Eg: 'F' to 15
		BaseShiftLeft (Base);

		Dword carry = DecodeDigit (*i, Base);
		for (unsigned int i = 0; carry != 0; i++)
		{
			carry += ZeroExtend ((*this)[i]);
			(*this)[i] = static_cast<Word> (carry);
			carry = SHR (carry, 16);
		}
	}

	// Prevents leading zero
	if (size() == 1)
	{
		if (back() == 0) pop_back();
	}
}

// Prints the number
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

// Multiply a number by its own base
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

// Divide the number by its own base and return the remainer
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

bool NaturalNumber::Even () const throw ()
{
	if (Zero()) return true;
	return (front() & 1) == 0;
}

const NaturalNumber NaturalNumber::Power (const NaturalNumber &n) const throw ()
{
	NaturalNumber answer (1), Multiplier (*this), count(n);

	// Russian Peasant Algorithm. See documentation for details
	while (!count.Zero())
	{
		if (count.Odd()) answer *= Multiplier;
		Multiplier *= Multiplier;
		count >>= 1;
	}

	return answer;
}

// Shifts left whole words n times.
void NaturalNumber::WordShiftLeft (Dword n) throw ()
{
	// Something to shift?
	if (empty()) return;

	// Makes room for the most significant words
	resize (size() + n, 0);

	iterator x = end();
	iterator y = x - n;

	// Moves each word n positions left
	while (y > begin())
	{
		x--; y--;
		*x = *y;
	}

	// Fill all opened positions with zero
	while (x > begin())
	{
		x--;
		*x = 0;
	}
}

// Shifts right whole words n times.
void NaturalNumber::WordShiftRight (Dword n) throw ()
{
	unsigned int i, j;

	iterator x = begin();
	iterator y = x + n;

	// Shifts right each word by n positions
	for (; y < end(); x++, y++) *x = *y;

	// Pops the quantity of shifted words or the whole vector, whichever is lesser
	j = n < size() ? n : size();
	for (i = 0; i < j; i++) pop_back();
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

NaturalNumber &NaturalNumber::Increment () throw()
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

NaturalNumber &NaturalNumber::Decrement () throw (Underflow)
{
	if (Zero())
	{
		// If number is zero then throws -1 in two's complement
		push_back (0xFFFF);
		throw Underflow(*this);
	}

	// Decrements each word until borrows no longer occurs
	for (unsigned int i = 0;; i++)
	{
		(*this)[i]--;
		if ((*this)[i] != 0xFFFF) break;
	}

	// Prevents leading zeroes, in case the decrement operation causes the most significant word to disappear
	if (back() == 0) pop_back();
	return *this;
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

// Prefix
NaturalNumber &NaturalNumber::operator++ () throw ()
{
	return Increment();
}

const NaturalNumber NaturalNumber::operator++ (int) throw ()
{
	NaturalNumber copy(*this);
	Increment();
	return copy;
}

// Prefix
NaturalNumber &NaturalNumber::operator-- () throw ()
{
	return Decrement();
}

const NaturalNumber NaturalNumber::operator-- (int) throw ()
{
	NaturalNumber copy(*this);
	Decrement();
	return copy;
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
		if (!(carry == 0xFFFF) && (back() & 0x8000 != 0))
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

NaturalNumber &NaturalNumber::operator<<= (Dword n) throw ()
{
	Dword carry = 0;

	// Assures all shifts will be within the Word boundary
	WordShiftLeft (SHR (n, 4));
	n &= 0xF;

	// Shifts all words
	for (iterator i = begin(); i != end(); i++)
	{
		carry |= ZeroExtend (*i) << n;
		*i = static_cast <Word> (carry);
		carry = SHR (carry, 16);
	}

	if (carry != 0) push_back (static_cast<Word> (carry));
	return *this;
}

NaturalNumber &NaturalNumber::operator>>= (Dword n) throw ()
{
	Dword carry = 0;

	// Assures all shifts will be within the Word boundary
	WordShiftRight (SHR (n, 4));
	n &= 0xF;

	// Shifts each word
	iterator i = end();
	while (i != begin())
	{
		i--;
		carry |= *i;
		*i = SHR (carry, n);
		carry &= (1 << n) - 1;
		carry <<= 16;
	}

	// The last element can be examined only if the vector is non-empty
	if (!empty())
	{
		if (back() == 0) pop_back();
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

const NaturalNumber NaturalNumber::operator<< (Dword n) const throw ()
{
	NaturalNumber answer(*this);
	return answer <<= n;
}

const NaturalNumber NaturalNumber::operator>> (Dword n) const throw ()
{
	NaturalNumber answer(*this);
	return answer >>= n;
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

//--- Integer Numbers

IntegerNumber::IntegerNumber (const string &s) throw (InvalidNumber, InvalidDigit)
{
	// Assumes positive initiallly
	Negative = false;
	if (s.empty()) return;

	try
	{
		switch (s[0])
		{
			case '$':
				// "$" is an invalid number
				if (s.size() == 1) throw InvalidNumber (s);
				if (s[1] == '-')
				{
					if (s.size() <= 2) throw InvalidNumber (s);
					Negative = true;
					AbsoluteValue = NaturalNumber (string (s, 2, string::npos), 16);
				}
				else
				{
					AbsoluteValue = NaturalNumber (string (s, 1, string::npos), 16);
				}

				break;

			case '0':
				if (s.size() >= 4)
				{
					if ((s[1] == 'x') || (s[1] == 'X'))
					{
						if (s[2] == '-')
						{
							Negative = true;
							AbsoluteValue = NaturalNumber (string (s, 3, string::npos), 16);
							break;
						}
					}
				}

				AbsoluteValue = NaturalNumber (s);
				break;

			case '-':
				Negative = true;

			case '+':
				AbsoluteValue = NaturalNumber (string (s.begin() + 1, s.end()));
				break;

			default:
				AbsoluteValue = NaturalNumber (s);
		}
	}
	catch (InvalidNumber iv)
	{
		throw InvalidNumber (s);
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

const IntegerNumber IntegerNumber::operator<< (Dword n) const throw ()
{
	IntegerNumber answer (*this);
	answer <<= n;
	return answer;
}

const IntegerNumber IntegerNumber::operator>> (Dword n) const throw ()
{
	IntegerNumber answer (*this);
	answer >>= n;
	return answer;
}

IntegerNumber &IntegerNumber::operator<<= (Dword n) throw ()
{
	// Sign is left unchanged
	AbsoluteValue <<= n;
	return *this;
}

IntegerNumber &IntegerNumber::operator>>= (Dword n) throw ()
{
	// Sign is left unchanged
	AbsoluteValue >>= n;
	return *this;
}

const IntegerNumber IntegerNumber::operator- () const throw ()
{
	IntegerNumber answer (*this);
	answer.Negative = !Negative;
	return answer;
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

//--- Real Numbers

Dword RealNumber::PrecisionIncrement = 8;
Dword RealNumber::MaximumSize = 8;
bool RealNumber::WatchPrecisionLoss = false;

NaturalNumber DigitCount (const string &m, const string &s, NaturalNumber &Multiplicator) throw ()
{
	NaturalNumber count;
	Multiplicator = NaturalNumber ();
	Word Base;

	if (s.empty ()) return count;

	string::const_iterator i = s.begin();
	string::const_iterator j = s.end();

	if (m[0] == '$')
	{
		Base = 16;
	}
	else
	{
		// Gets the base the number is written at
		Base = NaturalNumber::IsRadix (*(j - 1));

		// If no radix was specified, assume the default base. If a radix was used, skip it
		if (Base == 0)
			Base = NaturalNumber::GetDefaultBase ();
		else
			j--;
	}

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
	// will be caught elsewhere.
	for (; i != j; i++)
	{
		if (NaturalNumber::ValidDigit (*i, 16)) count++;
	}

	Multiplicator *= count;

	if (Base != 10) return Multiplicator;

	return count;
}

RealNumber::RealNumber (const string &s) throw (InvalidNumber, InvalidDigit)
{
	unsigned int DotPosition;

	// Search for the decimal point (if any)
	DotPosition = s.find('.');

	// Verifies if there is a fractional part
	if (DotPosition == string::npos)
	{
		Integer = true;
		Mantissa = IntegerNumber (s);
		return;
	}

	// TemporaryMantissa holds the string up to the decimal point (exclusive); TemporaryFraction holds the
	// rest (except the decimal point)
	Integer = false;
	string TemporaryMantissa (s, 0, DotPosition);
	string TemporaryFraction (s, DotPosition + 1, s.size());

	unsigned int RadixPosition = TemporaryFraction.find_last_not_of ('_');

	// If there is nothing after the decimal point except underscores, all we must do is to calculate the mantissa
	if (RadixPosition == string::npos)
	{
		try
		{
			Mantissa = IntegerNumber (TemporaryMantissa);
			return;
		}
		catch (...)
		{
			throw InvalidNumber (s);
		}
	}

	unsigned int LastDigit;

	// Appends the significant part of the fraction to the mantissa
	if (NaturalNumber::IsRadix (TemporaryFraction[RadixPosition]))
	{
		char Radix = TemporaryFraction[RadixPosition];

		// Finds the position after the last significant digit
		LastDigit = TemporaryFraction.find_last_not_of ("0_", RadixPosition - 1);
		if (LastDigit == string::npos)
			LastDigit = 0;
		else
			LastDigit++;

		// Skips any trailing underscores and zeroes; Puts the radix into position again
		TemporaryFraction.resize (LastDigit);
		TemporaryFraction += Radix;
	}
	else
	{
		// Finds the position after the last significant digit
		LastDigit = TemporaryFraction.find_last_not_of ("0_", RadixPosition + 1);
		if (LastDigit == string::npos)
			LastDigit = 0;
		else
			LastDigit++;

		// Skips any trailing underscores and zeroes
		TemporaryFraction.resize (LastDigit);
	}

	try
	{
		NaturalNumber Multiplicator;

		// Calculates the mantissa and the exponent of the number
		Mantissa = IntegerNumber (TemporaryMantissa + TemporaryFraction);
		Exponent = DigitCount (TemporaryMantissa, TemporaryFraction, Multiplicator);
		Exponent = -Exponent;
		Mantissa *= (IntegerNumber (5)).Power (Multiplicator);
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
	return *this;
}

RealNumber &RealNumber::operator-= (const RealNumber &n) throw ()
{
	RealNumber r(n);

	Adjust (r);
	Mantissa -= r.Mantissa;
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

	// If we have an exact division, fine.
	if (Remainer.Zero()) return Quotient;
	IntegerNumber Multiplier = IntegerNumber (10).Power (PrecisionIncrement);

	// As long as there is some remainer and room for more digits, keep dividing
	while ((!Remainer.Zero()) && (Quotient.Size() < MaximumSize))
	{
		Remainer *= Multiplier;
		Quotient.Mantissa *= Multiplier;
		Quotient.Exponent -= PrecisionIncrement;

		Quotient.Mantissa += Remainer.Divide (n.Mantissa, &Remainer);
	}

	// If the user wants an exception due to precision loss and we have it...
	if (WatchPrecisionLoss && (!Remainer.Zero()))
	{
		RealNumber r (Remainer, Quotient.Exponent - PrecisionIncrement);
		throw PrecisionLoss (Quotient, r);
	}

	return Quotient;
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
