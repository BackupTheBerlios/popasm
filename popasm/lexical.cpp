/***************************************************************************
                          lexical.cpp  -  description
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

#include "lexical.h"
#include "lexnum.h"
#include "lexop.h"
#include "lexsym.h"

Token::~Token () throw () {}

Token *Token::GetToken (InputFile &inp, Context c)
{
	// Gets the next string from file. Returns zero if EOF is reached
	string *s = inp.GetString();
	if (s == 0)
		return 0;

	// Checks for comment
	if (*s == ";")
	{
		delete s;
		inp.SkipLine ();
		return new Symbol (new BasicSymbol ("\n"), true);
	}

	Token *NextToken;

	// Tests for numbers. If successful, s will no longer be necessary
	NextToken = Number::Read(*s, inp);
	if (NextToken != 0)
	{
		delete s;
		return NextToken;
	}

	switch (c)
	{
		case ARGUMENT_EXPECTED:
			// Tests for operators. If successful, s will no longer be necessary
			NextToken = Operator::Read (*s, inp);
			if (NextToken != 0)
			{
				delete s;
				return NextToken;
			}

			// Tests for symbols. If successful, s will no longer be necessary
			NextToken = Symbol::Read (*s, inp);
			if (NextToken != 0)
			{
				delete s;
				return NextToken;
			}

			break;

		case COMMAND_EXPECTED:
			// Tests for symbols. If successful, s will no longer be necessary
			NextToken = Symbol::Read (*s, inp);
			if (NextToken != 0)
			{
				delete s;
				return NextToken;
			}

			// Tests for operators. If successful, s will no longer be necessary
			NextToken = Operator::Read (*s, inp);
			if (NextToken != 0)
			{
				delete s;
				return NextToken;
			}

			break;

		default:
			cout << "Unknown context" << endl;
			throw 0;
	}

	// If all of the above fails... who knows which token is this??
	return new Symbol (new BasicSymbol (*s), true);
}
