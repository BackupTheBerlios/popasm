/***************************************************************************
                          lexnum.h  -  description
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

#ifndef LEXNUM_H
#define LEXNUM_H

#include <string>
#include <vector>

#include "number.h"
#include "lexical.h"
#include "inp_file.h"

class InvalidSize;
class CastFailed;

// Numbers represent quantities (including negative and real ones)
class Number : public Token
{
	RealNumber n;
	unsigned int Size;

	public:
	Number () throw () : Size (0) {}
	Number (const RealNumber &x) throw () : n(x), Size(0) {}
	Number (const Number &x) throw () : n(x.n), Size(x.Size) {}
	~Number () throw () {}

	enum NumberType {SIGNED = 0, UNSIGNED = 1, ANY = 2, RAW = 3};
	static string PrintNumberType (NumberType nt) throw ()
	{
		switch (nt)
		{
			case SIGNED:
				return string ("signed");

			case UNSIGNED:
				return string ("unsigned");

			default:
				break;
		}

		return string();
	}

	unsigned int GetSize () const throw () {return Size;}
	void SetSize (unsigned int s, NumberType t = ANY) throw (InvalidSize, CastFailed);

	const RealNumber &GetValue () const throw() {return n;}
	bool IsInteger () const throw () {return n.IsInteger();}

	// Returns the value of the number as an unsigned long int
	unsigned long int GetUnsignedLong () const throw (IntegerExpected, Overflow) {return n.GetUnsignedLong();}
	// Returns the value of the number as a long int
	long int GetLong () const throw (IntegerExpected, Overflow) {return n.GetLong();}

	// Math operators reset Size constraints
	Number &operator+= (const Number &x) {n += x.n; Size = 0; return *this;}
	Number &operator-= (const Number &x) {n -= x.n; Size = 0; return *this;}
	Number &operator*= (const Number &x) {n *= x.n; Size = 0; return *this;}
	Number &operator/= (const Number &x);
	Number &operator%= (const Number &x);

	Number &operator&= (const Number &x);
	Number &operator|= (const Number &x);
	Number &operator^= (const Number &x);
	Number &operator<<= (const Number &x);
	Number &operator>>= (const Number &x);

	Number &BinaryShiftRight (const Number &x);
	Number &UnsignedDivision (const Number &x);
	Number &UnsignedModulus (const Number &x);

	Number &operator~ ();
	Number &operator- () {n.ChangeSign(); Size = 0; return *this;}
	bool operator== (const Number &x) const throw () {return (n == x.n);}
	bool operator!= (const Number &x) const throw () {return (n != x.n);}

	static string PrintSize (unsigned int Size) throw ()
	{
		switch (Size)
		{
			case 8: return "byte";
			case 16: return "word";
			case 32: return "dword";
			case 48: return "pword";
			case 64: return "qword";
			case 80: return "tbyte";
		}

		return string();
	}

	string Print () const throw ()
	{
		return PrintSize (Size) + " " + n.Print();
	}

	virtual Number *Clone() const throw () {return new Number (*this);}
	virtual bool Zero () const {return n.Zero();}

	// Attempts to build a Number from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Number *Read (const string &str, InputFile &inp);
	void Write (vector<Byte> &Output) const {n.Write(Output, Size / 8);}
};

// Thrown when the user attempts to set the size of an expression to a number not multiple of eight
class InvalidSize : public exception
{
	string WhatString;

	public:
	InvalidSize (unsigned int s) throw () : WhatString ("Size must be multiple of 8. Got ") {WhatString += Print(s);}
	~InvalidSize () throw () {}

	const char *what() const throw() {return WhatString.c_str();}
};

// Thrown when the user attempts to cast a number to a storage space that cannot hold it. Eg. BYTE 1234
class CastFailed : public exception
{
	string WhatString;

	public:
	CastFailed (const RealNumber &n, unsigned int s, Number::NumberType t = Number::ANY) throw ();
	~CastFailed () throw () {}

	const char *what() const throw() {return WhatString.c_str();}
};

#endif
