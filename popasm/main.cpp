/***************************************************************************
                          popasm.cpp  -  description
                             -------------------
    begin                : Tue May 28 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//---------------------------------------------------------------------------
// Main source file
//---------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string>
#include <iostream>
#include <vector>
#include <exception>
#include <typeinfo>

#include "inp_file.h"
#include "lexical.h"
#include "parser.h"
#include "asmer.h"
#include "argument.h"

bool ParseLine (InputFile &inp)
{
	Token *t;
	vector<Token *> v;
	Symbol *s;
	Expression *e;
	Argument *a;

	while (true)
	{
		t = Token::GetToken(inp);

		if (t == 0) return false;

		s = dynamic_cast<Symbol *> (t);
		if (s != 0)
		{
			if (s->GetName() == "\n")
			{
				if (v.empty()) break;

				e = Parser::EvaluateExpression (v);
				a = Argument::MakeArgument (*e, 16);
				cout << a->Print() << endl;
				break;
			}
		}

		v.push_back (t);
	}

	return true;
}

int main (int argc, char **argv)
{
	Assembler *a = new PopAsm();
	if (argc != 2) return 1;
	InputFile inp (argv[1]);

	if (!inp)
	{
		cout << "File not found" << endl;
		return 1;
	}

	do
	{
		try
		{
			if (!ParseLine (inp)) break;
		} catch (exception &e) {cout << e.what() << endl;}
	} while (true);

	delete a;
	return 0;
}
