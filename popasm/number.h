/***************************************************************************
                          number.h  -  description
                             -------------------
    begin                : Fri Sep 7 2001
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Allows the user to create huge numbers and operate on them
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NUMBER_H
#define NUMBER_H

#include <vector>
#include <string>
#include <exception>
#include "defs.h"

class NumberException;
class InvalidBase;
class InvalidDigit;
class InvalidNumber;
class Underflow;
class DivisionByZero;
class LeadingZero;
class PrecisionLoss;

string SimplifyString (const string &s, Word &Base, bool *Negative = 0) throw (InvalidNumber);

// Natural number is a vector of words capable of storing any number, no
// matter how huge it is. IMPORTANT: No leading zeroes must be allowed!!
// Remember that if you plan to add any new function.
class NaturalNumber : vector <Word>
{
	// Current default base for all numbers, specified by .RADIX; Default is 10.
	static Word DefaultBase;

	// Increment and decrement functions. Called by operator++ and operator--
	NaturalNumber &Increment () throw ();
	NaturalNumber &Decrement () throw (Underflow);

	protected:
	// Shift each element in the array n times
	void WordShiftLeft (Dword n) throw ();
	void WordShiftRight (Dword n) throw ();

	public:
	// Builds a NaturalNumber from either a Dword or a numeric string
	NaturalNumber (const Dword d = 0) throw ();
	NaturalNumber (const string &n, Word ForcedBase = 0) throw (InvalidNumber, InvalidDigit);
	virtual ~NaturalNumber () throw () {}

	// Returns the size of the underlying vector
	Dword Size() const throw () {return size();}

	// Converts a character to its value. Case is ignored. Eg.: 'F' = 'f' = 15
	static Word DecodeDigit (char c, Word Base) throw (InvalidDigit);
	// Converts a digit to a character. Eg.: 15 to 'F' (hex)
	static char EncodeDigit (Byte d) throw ();
	// Tests if the given character is a valid digit in the specified base
	static bool ValidDigit (char c, Word Base) throw ();
	// Checks if a given character is a radix
	static Word IsRadix (char c) throw ();

	// Gets and sets the default bases. Accepted values are 2, 8, 10 and 16
	static Word GetDefaultBase () throw () {return DefaultBase;}
	static void SetDefaultBase (Word NewBase) throw (InvalidBase);

	// Returns 1 if *this > n, 0 if equal or -1 otherwise
	int Compare (const NaturalNumber &n) const throw ();

	// Returns *this / n. If Remainer is not a NULL pointer, it will hold the remainer
	const NaturalNumber Divide (const NaturalNumber &n, NaturalNumber *Remainer = 0) const throw (DivisionByZero);

	// Multiplies or divides the number by its base (given as argument)
	void BaseShiftLeft (Word Base) throw ();
	Word BaseShiftRight (Word Base) throw ();

	// Checks parity
	bool Even () const throw ();
	bool Odd () const throw () {return !Even();}

	// Returns *this raised to nth power. This number remains unchanged
	const NaturalNumber Power (const NaturalNumber &n) const throw ();

	const NaturalNumber operator+  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator-  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator*  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator/  (const NaturalNumber &n) const throw (DivisionByZero);
	const NaturalNumber operator%  (const NaturalNumber &n) const throw (DivisionByZero);
	const NaturalNumber operator<< (Dword n) const throw ();
	const NaturalNumber operator>> (Dword n) const throw ();

	NaturalNumber &operator+=  (const NaturalNumber &n) throw ();
	NaturalNumber &operator-=  (const NaturalNumber &n) throw (Underflow);
	NaturalNumber &operator*=  (const NaturalNumber &n) throw ();
	NaturalNumber &operator/=  (const NaturalNumber &n) throw (DivisionByZero);
	NaturalNumber &operator%=  (const NaturalNumber &n) throw (DivisionByZero);
	NaturalNumber &operator<<= (Dword n) throw ();
	NaturalNumber &operator>>= (Dword n) throw ();

	// Unary operators
	NaturalNumber &operator++ () throw ();
	const NaturalNumber operator++ (int) throw ();
	NaturalNumber &operator-- () throw ();
	const NaturalNumber operator-- (int) throw ();

	bool operator>  (const NaturalNumber &n) const throw ();
	bool operator>= (const NaturalNumber &n) const throw ();
	bool operator<  (const NaturalNumber &n) const throw ();
	bool operator<= (const NaturalNumber &n) const throw ();
	bool operator== (const NaturalNumber &n) const throw ();
	bool operator!= (const NaturalNumber &n) const throw ();

	// Prints the number in any base. Default is 10.
	string Print (Word Base = 10) const throw ();
	// Detects if a number is zero
	bool Zero() const throw (LeadingZero);

	// Performs tests to check if the class is working without bugs
	static bool Test () throw ();
};

class IntegerNumber
{
	NaturalNumber AbsoluteValue;
	bool Negative;	// True if the number is negative; meaningless if absolute value is zero

	public:
	// Builds an integer from a long int, a numeric string or a NaturalNumber
	IntegerNumber (long int i = 0) throw ();
	IntegerNumber (const NaturalNumber &n) throw () : AbsoluteValue(n), Negative(false) {}
	IntegerNumber (const string &s, Word ForcedBase = 0, bool ForcedSign = false) throw (InvalidNumber, InvalidDigit);
	~IntegerNumber () throw () {}

	// Returns the size of the underlying vector
	Dword Size () const throw () {return AbsoluteValue.Size();}

	// Multiplies or divides the number by its base (given as argument)
	void BaseShiftLeft (Word Base) throw () {AbsoluteValue.BaseShiftLeft (Base);}

	// Returns *this raised to nth power. This number remains unchanged
	const IntegerNumber Power (const NaturalNumber &n) const throw ();

	const IntegerNumber operator+  (const IntegerNumber &n) const throw ();
	const IntegerNumber operator-  (const IntegerNumber &n) const throw ();
	const IntegerNumber operator*  (const IntegerNumber &n) const throw ();
	const IntegerNumber operator/  (const IntegerNumber &n) const throw (DivisionByZero);
	const IntegerNumber operator%  (const IntegerNumber &n) const throw (DivisionByZero);
	const IntegerNumber operator<< (Dword n) const throw ();
	const IntegerNumber operator>> (Dword n) const throw ();

	IntegerNumber &operator+=  (const IntegerNumber &n) throw ();
	IntegerNumber &operator-=  (const IntegerNumber &n) throw ();
	IntegerNumber &operator*=  (const IntegerNumber &n) throw ();
	IntegerNumber &operator/=  (const IntegerNumber &n) throw (DivisionByZero);
	IntegerNumber &operator%=  (const IntegerNumber &n) throw (DivisionByZero);
	IntegerNumber &operator<<= (Dword n) throw ();
	IntegerNumber &operator>>= (Dword n) throw ();

	// Unary operators
	void ChangeSign () throw () {Negative = !Negative;}
	const IntegerNumber operator- () const throw ();
	IntegerNumber &operator++ () throw ();
	const IntegerNumber operator++ (int) throw ();
	IntegerNumber &operator-- () throw ();
	const IntegerNumber operator-- (int) throw ();

	// Speeds up comparisson with zero
	bool GreaterThanZero () const throw () {return !Negative && !Zero();}
	bool LesserThanZero () const throw () {return Negative && !Zero();}

	// Returns 1 if *this > n, 0 if equal or -1 otherwise
	int Compare (const IntegerNumber &n) const throw ();
	// Returns *this / n. If Remainer is not a NULL pointer, it will hold the remainer
	const IntegerNumber Divide (const IntegerNumber &n, IntegerNumber *Remainer = 0) const throw (DivisionByZero);

	bool operator>  (const IntegerNumber &n) const throw ();
	bool operator>= (const IntegerNumber &n) const throw ();
	bool operator<  (const IntegerNumber &n) const throw ();
	bool operator<= (const IntegerNumber &n) const throw ();
	bool operator== (const IntegerNumber &n) const throw ();
	bool operator!= (const IntegerNumber &n) const throw ();

	// Prints the number
	string Print (Word Base = 10) const throw ();
	// Detects if the number is zero
	bool Zero() const throw (LeadingZero);
	// Gets the absolute value of the number
	const NaturalNumber Abs() const throw () {return AbsoluteValue;}

	// Performs tests to check if the class is working without bugs
	static bool Test () throw ();
};

class RealNumber
{
	// All real numbers are represented in the form Mantissa * 10 ^ Exponent
	IntegerNumber Mantissa, Exponent;
	// Precision increment holds the power of ten the mantissa must be multiplied by in order to allow
	// divisions. Maximum size prevents infinite loops in divisions. If a division is not exact, then
	// an exception of PrecisionLoss will be thrown, unless WatchPrecisionLoss is false
	static Dword PrecisionIncrement, MaximumSize;
	static bool WatchPrecisionLoss;

	// True if number built from an integer number string. E.g. "123".
	bool Integer;

	// Given *this and another number, converts the one with greater exponent
	// to the equivalent of the lesser one
	void Adjust (RealNumber &n) throw ();

	public:
	RealNumber () throw () : Integer (true) {}
	RealNumber (const IntegerNumber &n) throw () : Mantissa (n), Integer (true) {}
	RealNumber (const IntegerNumber &m, const IntegerNumber &e) throw () : Mantissa (m), Exponent (e), Integer (!e.LesserThanZero()) {}
	RealNumber (const string &s, Word ForcedBase = 0, bool ForcedSign = false) throw (InvalidNumber, InvalidDigit);
	RealNumber (int n) throw () : Mantissa (IntegerNumber(n)), Integer (true) {}
	~RealNumber () throw () {}

	static unsigned long int GetPrecisionIncrement () throw () {return PrecisionIncrement;}
	static void SetPrecisionIncrement (const unsigned long int n) throw () {PrecisionIncrement = n;}
	static unsigned long int GetMaximumSize () throw () {return MaximumSize;}
	static void SetMaximumSize (const unsigned long int n) throw () {MaximumSize = n;}
	static bool GetWatchPrecisionLoss () throw () {return WatchPrecisionLoss;}
	static void SetWatchPrecisionLoss (const bool yes) throw () {WatchPrecisionLoss = yes;}

	bool GetInteger () const {return Integer;}
	void SetInteger (bool NewInteger) {Integer = NewInteger;}

	const RealNumber operator+ (const RealNumber &n) const throw ();
	const RealNumber operator- (const RealNumber &n) const throw ();
	const RealNumber operator* (const RealNumber &n) const throw ();
	const RealNumber operator/ (const RealNumber &n) const throw (DivisionByZero, PrecisionLoss);

	RealNumber &operator+= (const RealNumber &n) throw ();
	RealNumber &operator-= (const RealNumber &n) throw ();
	RealNumber &operator*= (const RealNumber &n) throw ();
	RealNumber &operator/= (const RealNumber &n) throw (DivisionByZero, PrecisionLoss);

	// Prints the number in the desired base. The additional argument prevents infinite loops
	string Print (Word Base = 10, Dword n = 8) const throw ();

	Dword Size () const {return Mantissa.Size();}
	const RealNumber Abs() const throw () {return RealNumber (Mantissa.Abs(), Exponent);}
	int Compare (const RealNumber &n) const throw ();

	// Unary minus
	const RealNumber operator- () const throw () {return RealNumber (-Mantissa, Exponent);}
	void ChangeSign () throw () {Mantissa.ChangeSign();}

	bool operator>  (const RealNumber &n) const throw ();
	bool operator>= (const RealNumber &n) const throw ();
	bool operator<  (const RealNumber &n) const throw ();
	bool operator<= (const RealNumber &n) const throw ();
	bool operator== (const RealNumber &n) const throw ();
	bool operator!= (const RealNumber &n) const throw ();

	bool GreaterThanZero () const throw () {return Mantissa.GreaterThanZero();}
	bool LesserThanZero () const throw () {return Mantissa.LesserThanZero();}

	operator IntegerNumber () const throw (PrecisionLoss);

	// Performs tests to check if the class is working without bugs
	static bool Test () throw ();
};

//----------------- Exceptions related to numbers

class NumberException : public exception
{
	string WhatString;

	public:
	NumberException (string s = "") : WhatString(s) {}
	~NumberException () {}

	const char *what() const {return WhatString.c_str();}
};

// Invalid base specified. Valid ones are 2, 8, 10 and 16
class InvalidBase : public NumberException
{
	Word AttemptedBase;

	public:
	InvalidBase (const Word w) :
		NumberException ("Invalid base: " + NaturalNumber(w).Print (10) +
		". Allowed ones are 2, 8, 10, and 16."), AttemptedBase (w) {}
	~InvalidBase () {}
};

// Invalid character used as digit. Eg.: '*'
class InvalidDigit : public NumberException
{
	char Digit;
	Word CurrentBase;

	public:
	InvalidDigit (char c, const Word w) :
		NumberException (string ("Invalid digit ") + c + " at base " +
		NaturalNumber(w).Print (10) + "."),  Digit(c), CurrentBase (w) {}

	~InvalidDigit () {}
};

// The given string does not represent a number. Eg. "surhquiwer"
class InvalidNumber : public NumberException
{
	string AttemptedNumber;

	public:
	InvalidNumber (const string &s) :
		NumberException (string ("Not a number: ") + s), AttemptedNumber (s) {}
	~InvalidNumber () {}
};

// The caller tried to decrement a null number
class Underflow : public NumberException
{
	// Two's complement of the number. MSB is always 1 even if that implies
	// another word to be pushed.
	NaturalNumber Complement;

	public:
	Underflow (const NaturalNumber &n) : NumberException ("Underflow."), Complement(n) {}
	~Underflow () {}
};

// Division by zero attempted
class DivisionByZero : public NumberException
{
	public:
	DivisionByZero () : NumberException ("Division by zero attempted") {}
	~DivisionByZero () {}
};

// Leading zeroed word found at underlying vector
class LeadingZero : public NumberException
{
	public:
	LeadingZero () : NumberException ("Internal error: redundant leading zero. Please send a bug report.") {}
	~LeadingZero () {}
};

class PrecisionLoss : public NumberException
{
	RealNumber Quotient, Remainer;

	public:
	PrecisionLoss (const RealNumber &q, const RealNumber &r) :
	NumberException ("Precision loss detected."), Quotient (q), Remainer (r) {}
	~PrecisionLoss () {}
};

#endif
