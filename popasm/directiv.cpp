/***************************************************************************
                          directiv.cpp  -  description
                             -------------------
    begin                : Wed Jun 19 2002
    copyright            : (C) 2002 by Helcio Mello
    email                : helcio@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "directiv.h"
#include "immed.h"
#include "parser.h"

BasicSymbol *Directive::Read (const string &str, InputFile &inp)
{
	Directive s (str);
	Directive * const *d = DirectiveTable.Find (&s);
	return (d == 0) ? 0 : *d;
}

HashTable <Directive *, HashFunctor, PointerComparator<BasicSymbol> > Directive::DirectiveTable =
	HashTable <Directive *, HashFunctor, PointerComparator<BasicSymbol> > ();

void DefinitionDirective::Assemble (const BasicSymbol *sym, vector<Token *>::iterator i,
	vector<Token *>::iterator j, vector<Byte> &Encoding) const
{
	// Converts the tokens into arguments.
	vector<Argument *> Arguments;
	Parser::ParseArguments (i, j, Arguments);
	unsigned int length = 0;

	for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
	{
		const Immediate *immed;

		immed = dynamic_cast<const Immediate *> ((*x)->GetData());
		if (immed == 0)
		{
//			for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
//				delete *x;

			cout << "Expected constant expression." << endl;
		}

		if ((!immed->IsInteger()) && (!AcceptFloat))
		{
//			for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
//				delete *x;

			throw IntegerExpected (immed->GetValue());
		}

		immed->SetSize (Size);
		immed->Write (Encoding);
		length++;
	}

	if (sym != 0)
	{
		Symbol::DefineSymbol (new Variable (sym->GetName(), Size, length));
	}

	for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
		delete *x;
}

void FunctionBITS (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	// Converts the tokens into arguments.
	vector<Argument *> Arguments;
	Parser::ParseArguments (i, j, Arguments);

	if (sym != 0)
	{
		cout << "Label definition not implemented yet." << endl;
		return;
	}

	if (Arguments.size() != 1)
	{
		cout << "Wrong number of arguments." << endl;
		return;
	}

	const Immediate *immed = dynamic_cast<const Immediate *> (Arguments[0]->GetData());
   if (immed == 0)
	{
		cout << "Must be a constant expression." << endl;
		return;
	}

	CurrentAssembler->SetCurrentMode (immed->GetUnsignedLong());
}

void Directive::SetupDirectiveTable () throw ()
{
	static Directive *Directives[] =
	{
		new DefinitionDirective ("DB",  8, false),
		new DefinitionDirective ("DW", 16, false),
		new DefinitionDirective ("DD", 32, true),
		new DefinitionDirective ("DF", 48, false),
		new DefinitionDirective ("DP", 48, false),
		new DefinitionDirective ("DQ", 64, true),
		new DefinitionDirective ("DT", 80, true),
      new Directive ("BITS", FunctionBITS)
	};

	for (unsigned int i = 0; i < sizeof (Directives) / sizeof (Directive *); i++)
		DirectiveTable.Insert (Directives[i]);
}
