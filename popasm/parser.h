/***************************************************************************
                          parser.h  -  description
                             -------------------
    begin                : Sun May 26 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//---------------------------------------------------------------------------
// Checks syntax and evaluates expressions respecting precedence
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <exception>
#include <string>

#include "lexical.h"
#include "lexop.h"
#include "inp_file.h"
#include "defs.h"

// Thrown when a pair of enclosers do not match. Eg. "(2 + 2]"
class EncloserMismatch : public exception
{
	const Encloser *Opener, *Closer;
	string WhatString;

	public:
	EncloserMismatch (const Encloser *op, const Encloser *cl) throw ();
	~EncloserMismatch () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

// Thrown when the user opens an encloser pair but forgets to close it. Eg. "(2 + 2"
class UnmatchedEncloser : public exception
{
	Encloser *Opener;
	string WhatString;

	public:
	UnmatchedEncloser (Encloser *op) throw (): Opener(op), WhatString ("Unmatched ") {WhatString += op->GetName();}
	~UnmatchedEncloser () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

// Thrown when a token cannot be converted to an expression. Eg. "MACRO_NAME + 2"
class UnexpectedToken : public exception
{
	const Token *t;
	string WhatString;

	public:
	UnexpectedToken (const Token *tt) throw ();
	~UnexpectedToken () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

// Thrown when the end of Tokens list (and thus the expression itself) ends unexpectedly. Eg. "1 + 2 +"
class UnexpectedEnd : public exception
{
	static const char *WhatString;

	public:
	UnexpectedEnd () throw () {}
	~UnexpectedEnd () throw () {}

	const char *what() const throw () {return WhatString;}
};

// Thrown when a command is expected but something else was got
class CommandExpected : public exception
{
	const Token *t;
	string WhatString;

	public:
	CommandExpected (const Token *tt) throw ();
	~CommandExpected () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

class Parser
{
	InputFile &Input;

	public:
	static Expression *EvaluateExpression (const vector<Token *> &v);
	vector<Byte> ParseLine ();

	Parser (InputFile &i) throw () : Input(i) {}
	~Parser () throw () {}
};

#endif
