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

#include "inp_file.h"
#include "lexical.h"
#include "parser.h"
#include "asmer.h"

bool EndOfLine (Token *t)
{
	Token *tt = dynamic_cast<UnknownToken *> (t);
	return (tt != 0);
}

string *ShowString (InputFile &inp)
{
	string *s;

	cout << "Reading from line " << inp.GetCurrentLine() << endl;
	s = inp.GetString();

	cout << "String read: ";
	if (*s == "\n")
	{
		cout << "End of line." << endl;
	}
	else
	{
		cout << *s << endl;
	}

	return s;
}

int main (int argc, char **argv)
{
	Assembler *a = new PopAsm();
	if (argc != 2) return 1;
	InputFile inp (argv[1]);

	if (!inp)
	{
		cout << "File not found" << endl;
	}

	vector<Token *> v;
	Token *t;

	do
	{
		t = GetToken (inp);
		if (EndOfLine (t)) break;
		v.push_back (t);
	} while (true);

	try
	{
		Expression *e = Parser::EvaluateExpression (v);
		e->Print();
		delete e;
	}
	catch (exception &e) {cout << e.what() << endl;}

	delete a;
	return 0;
}
