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

#include "number.h"
#include "lexical.h"
#include "inp_file.h"

class IntegerExpected : public exception
{
	RealNumber n;
	string WhatString;

	public:
	IntegerExpected (const RealNumber &nn) throw () : n(nn), WhatString ("Integer number expected, got ")	{WhatString += nn.Print();}
	~IntegerExpected () throw () {}

	const char *what() const {return WhatString.c_str();}
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
	CastFailed (const RealNumber &n, unsigned int s) throw ();
	~CastFailed () throw () {}

	const char *what() const throw() {return WhatString.c_str();}
};

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

	enum NumberType {SIGNED = 0, UNSIGNED = 1, ANY = 2,};
	unsigned int GetSize () const throw () {return Size;}
	void SetSize (unsigned int s, NumberType t = SIGNED) throw (InvalidSize, CastFailed);

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

	Number &operator~ ();
	Number &operator- () {n.ChangeSign(); Size = 0; return *this;}
	bool operator== (const Number &x) const throw () {return (n == x.n);}
	bool operator!= (const Number &x) const throw () {return (n != x.n);}

	string Print () const throw ()
	{
		string s;

		switch (Size)
		{
			case 8: s = "byte "; break;
			case 16: s = "word "; break;
			case 32: s = "dword "; break;
			default: break;
		}

		return s + n.Print();
	}
	virtual Number *Clone() const throw () {return new Number (*this);}
	virtual bool Zero () const {return n.Zero();}

	// Attempts to build a Number from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Number *Read (const string &str, InputFile &inp);
};

#endif
