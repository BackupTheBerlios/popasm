/***************************************************************************
                          lexnum.cpp  -  description
                             -------------------
    begin                : Mon Jun 3 2002
    copyright            : (C) 2002 by Helcio Mello
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

#include "lexnum.h"

CastFailed::CastFailed (const RealNumber &n, unsigned int s, Number::NumberType t = Number::ANY) throw ()
	: WhatString ("Could not cast ")
{
	WhatString += n.Print() + " to be ";

	if ((t == Number::SIGNED) || (Number::UNSIGNED))
		WhatString += Number::PrintNumberType (t) + " ";

	WhatString += Print(s) + " bits wide.";
}

void Number::SetSize (unsigned int s, NumberType t = ANY) throw (InvalidSize, CastFailed)
{
	// New size MUST be multiple of 8
	if ((s & 7) != 0) throw InvalidSize(s);

	// Zero is a valid size. It means: "Forget the Size restrictions for this number."
	if (s != 0)
	{
		// Is it an integer number?
		if (n.IsInteger())
		{
			// limit now holds the greatest possible number+1 for its type: 2^n for unsigned or any,
			// and 2^(n-1) for signed numbers
			IntegerNumber i = static_cast<IntegerNumber> (n);
			IntegerNumber limit(1);
			limit <<= (t == SIGNED) ? (s - 1) : s;

			if (i >= limit) throw CastFailed (n, s, t);

			// The lower bound is zero for unsigned numbers and -2^(n-1) for signed or any. -2^n for raw
			if (t == UNSIGNED)
				limit = 0;
			else
				limit.ChangeSign();

			if (i < limit) throw CastFailed (n, s, t);
		}
		else
		{
			// IEEE format requires at least 32 bits to store real numbers
			if (s < 32) throw CastFailed (n, s, t);
		}
	}

	Size = s;
}

Number *Number::Read (const string &str, InputFile &inp)
{
	if (str.size() == 0) return 0;
	char c = str[0];

	// if the first char is a '$', the string is a number only if the second character is a digit
	if (c == '$')
	{
		if (str.size() == 1) return 0;
		c = str[1];
	}

	// The first character must be a digit (0-9)
	if (!InputFile::Digit (c)) return 0;

	string s(str);

	// Checks for exceptional conditions
	switch (inp.GetNextChar())
	{
		// If immediatelly followed by a decimal point, concatenate the strings
		case '.':
			// Appends the decimal point to the original string.
			delete inp.GetString();
			s += '.';

			// If there's something just after the point, append it.
			if (InputFile::Space (inp.GetNextChar()))
			{
				// User has typed a number followed by a decimal point. Eg.: 123. = 123.0
				break;
			}
			else
			{
				string *t = inp.GetString();

				// Watches out for premature end-of-line
				if (*t != "\n")
				{
					// Appends fractional part of float point number
					s += *t;
					delete t;
					if ((inp.GetNextChar() != '+') && (inp.GetNextChar() != '-')) break;
				}
				else
				{
					inp.UngetString (t);
					break;
				}
			}

		case '+':
		case '-':
		{
			// Numbers like 6.02e-23 are only allowed if default base is 10.
			Word Base;
			string aux (SimplifyString (s, Base));
			if (Base != 10) break;

			// Checks if last character is an 'e'
			char c = *(aux.rbegin());
			if ((c == 'e') || (c == 'E'))
			{
				// t contains either '+' or '-'
				string *t = inp.GetString();

				// Watches out for premature end-of-line
				if ((inp.GetNextChar() != '\n') && (!InputFile::Space (inp.GetNextChar())))
				{
					string *u = inp.GetString();

					if (*u == "\n")
					{
						// User has broken line just after exponent sign. Unget what was got...
						inp.UngetString (u);
						inp.UngetString (t, (*u)[0]);
					}
					else
					{
						// Now s holds the whole numeric string
						s += *t + *u;
						delete t;
						delete u;
					}
				}
				else inp.UngetString (t);
			}

			break;
		}

		default:
			break;
	}

	Number *n = new Number (s);
	return n;
}

Number &Number::operator/= (const Number &x)
{
	// Both numbers are integers?
	if (n.IsInteger() && x.n.IsInteger())
	{
		// Integer division
		n = RealNumber (static_cast<IntegerNumber> (n) / static_cast<IntegerNumber> (x.n));
	}
	else
	{
		// Real division
		n /= x.n;
	}

	Size = 0;
	return *this;
}

Number &Number::operator%= (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n) % static_cast<IntegerNumber> (x.n));
	Size = 0;
	return *this;
}

Number &Number::operator&= (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n) & static_cast<IntegerNumber> (x.n));
	Size = 0;
	return *this;
}

Number &Number::operator|= (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n) | static_cast<IntegerNumber> (x.n));
	Size = 0;
	return *this;
}

Number &Number::operator^= (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n) ^ static_cast<IntegerNumber> (x.n));
	Size = 0;
	return *this;
}

Number &Number::operator<<= (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n) << static_cast<IntegerNumber> (x.n));
	Size = 0;
	return *this;
}

Number &Number::operator>>= (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n) >> static_cast<IntegerNumber> (x.n));
	Size = 0;
	return *this;
}

Number &Number::BinaryShiftRight (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n).BinaryShiftRight (static_cast<IntegerNumber> (x.n)));
	Size = 0;
	return *this;
}

Number &Number::UnsignedDivision (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n).BinaryShiftRight (static_cast<IntegerNumber> (x.n)));
	Size = 0;
	return *this;
}

Number &Number::UnsignedModulus (const Number &x)
{
	// Both numbers are integers?
	if (!n.IsInteger()) throw IntegerExpected (n);
	if (!x.n.IsInteger()) throw IntegerExpected (x.n);

	// Performs operation
	n = RealNumber (static_cast<IntegerNumber> (n).BinaryShiftRight (static_cast<IntegerNumber> (x.n)));
	Size = 0;
	return *this;
}

Number &Number::operator~ ()
{
	// Number must be integer for One's Complement
	if (!n.IsInteger()) throw IntegerExpected(n);

	// Performs the operation
	n = RealNumber (~static_cast<IntegerNumber> (n));
	Size = 0;
	return *this;
}
