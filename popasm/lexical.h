/***************************************************************************
                          lexical.h  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Lexical analyser. Returns tokens from the input file.
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LEXICAL_H
#define LEXICAL_H

#include <string>
#include <exception>
#include <vector>

#include "inp_file.h"

// Tokens base class
class Token
{
	public:
	Token () throw () {}
	virtual ~Token () throw () = 0;

	// Translates the token into a string
	virtual string Print () const throw () = 0;

	// Reads a token from the given file
	static Token *GetToken (InputFile &inp);
	static void ReadLine (vector<Token *> &Tokens, InputFile &Input) throw ();
};
/*
// Thrown when the string got is neither a number, nor an operator, nor a symbol
class UnknownToken : public exception
{
	string WhatString;

	public:
	// Constructor deletes the string given as argument because the caller cannot do it himself.
	UnknownToken (const string *s) throw () : WhatString ("Unknown token: ") {WhatString += *s; delete s;}
	~UnknownToken () {}

	const char *what () const throw() {return WhatString.c_str();}
};
*/
#endif
