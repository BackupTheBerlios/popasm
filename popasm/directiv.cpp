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
			for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
				delete *x;

			cout << "Expected constant expression." << endl;
		}

		if ((!immed->IsInteger()) && (!AcceptFloat))
		{
			RealNumber temp (immed->GetValue());

			// Performs cleanup
			for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
				delete *x;

			throw IntegerExpected (temp);
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
	if (sym != 0)
	{
		Symbol::DefineSymbol (new Label (sym->GetName()));
	}

	// Converts the tokens into arguments.
	vector<Argument *> Arguments;
	Parser::ParseArguments (i, j, Arguments);

	if (Arguments.size() != 1)
	{
		for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
			delete *x;

		cout << "Wrong number of arguments." << endl;
		return;
	}

	const Immediate *immed = dynamic_cast<const Immediate *> (Arguments[0]->GetData());
   if (immed == 0)
	{
		for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
			delete *x;

		cout << "Must be a constant expression." << endl;
		return;
	}

	CurrentAssembler->SetCurrentMode (immed->GetUnsignedLong());
		for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
			delete *x;
}

void FunctionEQU (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	Constant *c;

	if (sym == 0)
		throw NameMissing ();

	BasicSymbol *bs = Symbol::Find (sym->GetName());
	if (bs != 0)
	{
		c = dynamic_cast<Constant *> (bs);
		if (c == 0)
			throw MultidefinedSymbol (bs->GetName());

		throw RedefinedConstant (bs->GetName());
	}

	c = new Constant (sym->GetName(), Parser::EvaluateExpression (vector<Token *> (i, j)));
	Symbol::DefineSymbol (c);
}

void FunctionEQUAL (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	Constant *c;

	if (sym == 0)
		throw NameMissing ();

	BasicSymbol *bs = Symbol::Find (sym->GetName());
	if (bs != 0)
	{
		c = dynamic_cast<Constant *> (bs);
		if (c == 0)
			throw MultidefinedSymbol (bs->GetName());
	}
	else
	{
		c = new Constant (sym->GetName(), 0, true);
		Symbol::DefineSymbol (c);
	}

	c->SetValue (Parser::EvaluateExpression (vector<Token *> (i, j)));
}

void FunctionSEGMENT (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	Segment *seg;

	if (sym == 0)
		throw NameMissing ();

	if (i != j)
	{
		cout << "Extra characters on line" << endl;
		throw 0;
	}

	seg = new Segment (sym->GetName());
	CurrentAssembler->AddSegment (seg);
}

void FunctionENDS (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	if (sym == 0)
		throw NameMissing ();

	if (i != j)
	{
		cout << "Extra characters on line" << endl;
		throw 0;
	}

	CurrentAssembler->CloseSegment (sym->GetName());
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
      new Directive ("BITS", FunctionBITS),
      new Directive ("EQU", FunctionEQU),
      new Directive ("=", FunctionEQUAL),
		new Directive ("SEGMENT", FunctionSEGMENT),
		new Directive ("ENDS", FunctionENDS)
	};

	for (unsigned int i = 0; i < sizeof (Directives) / sizeof (Directive *); i++)
		DirectiveTable.Insert (Directives[i]);
}

char NameMissing::WhatString[] = "Symbol name missing.";
