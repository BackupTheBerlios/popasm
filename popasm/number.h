/** @file

	@brief Huge numbers declarations
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
class Overflow;
class DivisionByZero;
class LeadingZero;
class PrecisionLoss;
class NegativeShift;
class InvalidFormat;
class IntegerExpected;

/** \brief Preprocess a string and get its base and sign */
string SimplifyString (const string &s, Word &Base, bool *Negative = 0) throw (InvalidNumber);

/**
	Natural number is a vector of words capable of storing any number, no matter how huge they are.
	@warning No leading zeroes are allowed! Remember that if you plan to add any new function. If
	(back() == 0) the number is invalid and unpredictable behavior may occur.
*/
class NaturalNumber : vector <Word>
{
	/// Current default base for all numbers, specified by .RADIX; Default is 10.
	static Word DefaultBase;

	protected:
	/// Shifts the vector n elements (words) to the left
	void WordShiftLeft (NaturalNumber n) throw ();
	/// Shifts the vector n elements (words) to the right
	void WordShiftRight (NaturalNumber n) throw ();

	public:
	/// Builds a NaturalNumber from a Dword
	NaturalNumber (Dword d = 0) throw ();
	/// Builds a NaturalNumber from a string
	NaturalNumber (const string &n, Word ForcedBase = 0) throw (InvalidNumber, InvalidDigit);
	virtual ~NaturalNumber () throw () {}

	/** Gets the size of the underlying vector in Words.

		@returns How many words are currently stored within the underlying vector.
		This method does nothing more than calling vector<Word>::size() method().
	*/
	Dword Size() const throw () {return size();}
	/// Returns the size of the underlying vector in bits
	Dword SizeInBits() const throw (LeadingZero);
	/// Returns the size of the underlying vector in bytes
	Dword SizeInBytes() const throw ();

	/// Adds leading zeroes to make the underlying vector as long as the vector of the other number
	void MatchSize (const NaturalNumber &n, Word w) throw ();

	/// Converts a character to its value. Case is ignored. Eg.: 'F' = 'f' = 15
	static Word DecodeDigit (char c, Word Base) throw (InvalidDigit);
	/// Converts a digit to a character. Eg.: 15 to 'F' (hex)
	static char EncodeDigit (Byte d) throw ();
	/// Tests if the given character is a valid digit in the specified base
	static bool ValidDigit (char c, Word Base) throw ();
	/// Checks if a given character is a radix. If so, returns the base the character refers to.
	static Word IsRadix (char c) throw ();

	/// Gets the current default base.
	static Word GetDefaultBase () throw () {return DefaultBase;}
	/// Sets the current default base. Accepted values are 2, 8, 10 and 16.
	static void SetDefaultBase (Word NewBase) throw (InvalidBase);

	/// Compares two numbers. The result is an integer 0, 1 or -1.
	int Compare (const NaturalNumber &n) const throw ();

	/// Divides two numbers. Calculates also the remainer if requested.
	const NaturalNumber Divide (const NaturalNumber &n, NaturalNumber *Remainer = 0) const throw (DivisionByZero);

	/// Multiplies a number by a Word
	void BaseShiftLeft (Word Base) throw ();
	/// Divides a number by a Word
	Word BaseShiftRight (Word Base) throw ();

	/// Returns whether a number is even
	bool Even () const throw ();
	/// Returns whether a number is odd
	bool Odd () const throw () {return !Even();}

	/// Returns a number raised to nth power.
	const NaturalNumber Power (const NaturalNumber &n) const throw ();

	const NaturalNumber operator+  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator-  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator*  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator/  (const NaturalNumber &n) const throw (DivisionByZero);
	const NaturalNumber operator%  (const NaturalNumber &n) const throw (DivisionByZero);
	const NaturalNumber operator<< (const NaturalNumber &n) const throw ();
	const NaturalNumber operator>> (const NaturalNumber &n) const throw ();
	const NaturalNumber operator&  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator|  (const NaturalNumber &n) const throw ();
	const NaturalNumber operator^  (const NaturalNumber &n) const throw ();

	NaturalNumber &operator+=  (const NaturalNumber &n) throw ();
	NaturalNumber &operator-=  (const NaturalNumber &n) throw (Underflow);
	NaturalNumber &operator*=  (const NaturalNumber &n) throw ();
	NaturalNumber &operator/=  (const NaturalNumber &n) throw (DivisionByZero);
	NaturalNumber &operator%=  (const NaturalNumber &n) throw (DivisionByZero);
	NaturalNumber &operator<<= (const NaturalNumber &n) throw ();
	NaturalNumber &operator>>= (const NaturalNumber &n) throw ();
	NaturalNumber &operator&=  (const NaturalNumber &n) throw ();
	NaturalNumber &operator|=  (const NaturalNumber &n) throw ();
	NaturalNumber &operator^=  (const NaturalNumber &n) throw ();

	/// Prefix increment operator
	NaturalNumber &operator++ () throw ();
	/// Postfix increment operator
	const NaturalNumber operator++ (int) throw ();
	/// Prefix decrement operator
	NaturalNumber &operator-- () throw (Underflow);
	/// Postfix decrement operator
	const NaturalNumber operator-- (int) throw (Underflow);

	bool operator>  (const NaturalNumber &n) const throw ();
	bool operator>= (const NaturalNumber &n) const throw ();
	bool operator<  (const NaturalNumber &n) const throw ();
	bool operator<= (const NaturalNumber &n) const throw ();
	bool operator== (const NaturalNumber &n) const throw ();
	bool operator!= (const NaturalNumber &n) const throw ();

	NaturalNumber &OnesComplement (Dword sz = 0) throw (Overflow);
	NaturalNumber &TwosComplement (Dword sz = 0) throw (Overflow);
	/// Replaces the number with its one's complement
	NaturalNumber &operator~ () throw ();

	void SetBit (Dword n) throw ();
	void ClearBit (Dword n) throw ();
	bool TestBit (Dword n) throw ();

	/// Prints the number in any base to a string. Default base is 10.
	string Print (Word Base = 10) const throw ();
	/// Writes the number in little-endian form to an output vector
	void Write (vector<Byte> &Output, unsigned int n, Byte pad = 0) const;
	/// Detects if a number is zero.
	bool Zero() const throw (LeadingZero);
	/// Returns the value of the number as an unsigned long int
	unsigned long int GetUnsignedLong () const throw (Overflow);
	/// Returns the value of the number as a long int
	long int GetLong () const throw (Overflow);

	/// Performs basic tests to check if the class is working without bugs
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
	const IntegerNumber operator<< (const IntegerNumber &n) const throw (NegativeShift);
	const IntegerNumber operator>> (const IntegerNumber &n) const throw (NegativeShift);
	const IntegerNumber operator&  (const IntegerNumber &n) const throw ();
	const IntegerNumber operator|  (const IntegerNumber &n) const throw ();
	const IntegerNumber operator^  (const IntegerNumber &n) const throw ();

	IntegerNumber &operator+=  (const IntegerNumber &n) throw ();
	IntegerNumber &operator-=  (const IntegerNumber &n) throw ();
	IntegerNumber &operator*=  (const IntegerNumber &n) throw ();
	IntegerNumber &operator/=  (const IntegerNumber &n) throw (DivisionByZero);
	IntegerNumber &operator%=  (const IntegerNumber &n) throw (DivisionByZero);
	IntegerNumber &operator<<= (const IntegerNumber &n) throw (NegativeShift);
	IntegerNumber &operator>>= (const IntegerNumber &n) throw (NegativeShift);
	IntegerNumber &operator&=  (IntegerNumber n) throw ();
	IntegerNumber &operator|=  (IntegerNumber n) throw ();
	IntegerNumber &operator^=  (IntegerNumber n) throw ();

	// Unary operators
	const IntegerNumber operator- () const throw ();
	const IntegerNumber operator~ () const throw ();
	IntegerNumber &operator++ () throw ();
	const IntegerNumber operator++ (int) throw ();
	IntegerNumber &operator-- () throw ();
	const IntegerNumber operator-- (int) throw ();

	// Speeds up comparisson with zero
	bool GreaterThanZero () const throw () {return !Negative && !Zero();}
	bool LesserThanZero () const throw () {return Negative && !Zero();}
	bool GetNegativeBit () const throw () {return Negative;}

	// Sign extends *this and n to be the same size
	void SignExtend (IntegerNumber &n) throw ();

	// Performs a binary shift right assuming (*this) is sz bytes long
	IntegerNumber &BinaryShiftRight (const IntegerNumber &n, Dword sz = 0) throw (NegativeShift, Overflow);
	IntegerNumber &UnsignedDivision (const IntegerNumber &n, Dword sz = 0) throw (Overflow);
	IntegerNumber &UnsignedModulus (const IntegerNumber &n, Dword sz = 0) throw (Overflow);

	void ChangeSign () throw () {Negative = !Negative;}
	IntegerNumber &OnesComplement () throw ();

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

	// Returns the number in its string form
	string Print (Word Base = 10) const throw ();
	// Writes the number in little-endian form, using n bytes
	void Write (vector<Byte> &Output, unsigned int n) const;
	// Detects if the number is zero
	bool Zero() const throw (LeadingZero);
	// Gets the absolute value of the number
	const NaturalNumber &Abs() const throw () {return AbsoluteValue;}

	// Returns the value of the number as an unsigned long int
	unsigned long int GetUnsignedLong () const throw (Overflow);
	// Returns the value of the number as a long int
	long int GetLong () const throw (Overflow);

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
	RealNumber (long int n) throw () : Mantissa (IntegerNumber(n)), Integer (true) {}
	~RealNumber () throw () {}

	static unsigned long int GetPrecisionIncrement () throw () {return PrecisionIncrement;}
	static void SetPrecisionIncrement (const unsigned long int n) throw () {PrecisionIncrement = n;}
	static unsigned long int GetMaximumSize () throw () {return MaximumSize;}
	static void SetMaximumSize (const unsigned long int n) throw () {MaximumSize = n;}
	static bool GetWatchPrecisionLoss () throw () {return WatchPrecisionLoss;}
	static void SetWatchPrecisionLoss (const bool yes) throw () {WatchPrecisionLoss = yes;}

	bool IsInteger () const throw () {return Integer;}
	void SetIntegerBit (bool NewInteger) throw () {Integer = NewInteger;}

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
	void Write (vector<Byte> &Output, unsigned int n) const;

	// Unary minus
	const RealNumber operator- () const throw () {return RealNumber (-Mantissa, Exponent);}
	void ChangeSign () throw () {Mantissa.ChangeSign();}

	bool operator>  (const RealNumber &n) const throw ();
	bool operator>= (const RealNumber &n) const throw ();
	bool operator<  (const RealNumber &n) const throw ();
	bool operator<= (const RealNumber &n) const throw ();
	bool operator== (const RealNumber &n) const throw ();
	bool operator!= (const RealNumber &n) const throw ();

	bool Zero () const throw () {return Mantissa.Zero();}
	bool GreaterThanZero () const throw () {return Mantissa.GreaterThanZero();}
	bool LesserThanZero () const throw () {return Mantissa.LesserThanZero();}

	operator IntegerNumber () const throw (PrecisionLoss);

	// Returns the value of the number as an unsigned long int
	unsigned long int GetUnsignedLong () const throw (Overflow, IntegerExpected);
	// Returns the value of the number as a long int
	long int GetLong () const throw (Overflow, IntegerExpected);

	// Performs tests to check if the class is working without bugs
	static bool Test () throw ();
};

//----------------- Exceptions related to numbers

class NumberException : public exception
{
	protected:
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
		NumberException (string ("Invalid base: ") + Print (w) +
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
		NumberException (string ("Invalid number: ") + s), AttemptedNumber (s) {}
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

// The caller tried to convert a number to a long int, but the number was too big
class Overflow : public NumberException
{
	public:
	Overflow () : NumberException ("Overflow.") {}
	~Overflow () {}
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

class NegativeShift : public NumberException
{
	IntegerNumber a, b;

	public:
	NegativeShift (const IntegerNumber &aa, const IntegerNumber &bb) :
		NumberException ("Shift attempted by negative amount."), a (aa), b (bb) {}
	~NegativeShift () {}
};

// Thrown when a real number is requested to be written in a non-supported format (e.g. 128 bits)
class InvalidFormat : public NumberException
{
	unsigned int Format;

	public:
	InvalidFormat (unsigned int f) throw () :
		NumberException ("Unsuported floating point format: "), Format (f) {WhatString += Print (f) + " bits.";}
	~InvalidFormat () {}
};

// Thrown when a non-integer real number is converted to integer
class IntegerExpected : public NumberException
{
	RealNumber n;

	public:
	IntegerExpected (const RealNumber &nn) throw ()
		: NumberException ("Integer number expected, got "), n(nn) {WhatString += nn.Print();}
	~IntegerExpected () throw () {}
};

#endif
