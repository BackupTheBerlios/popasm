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
#include "asmer.h"
#include "type.h"
#include "lexop.h"
#include "defs.h"

BasicSymbol *Directive::Read (const string &str, InputFile &inp)
{
	Directive s (str);
	Directive * const *d = DirectiveTable.Find (&s);
	return (d == 0) ? 0 : *d;
}

HashTable <Directive *, HashFunctor, PointerComparator<BasicSymbol> > Directive::DirectiveTable =
	HashTable <Directive *, HashFunctor, PointerComparator<BasicSymbol> > ();

void Directive::Assemble (const Symbol *sym, Parser &p, vector<Byte> &Encoding) const
{
	vector<Token *> Tokens;
	p.ReadLine (Tokens);

	vector<Token *>::iterator i = Tokens.begin();
	vector<Token *>::iterator j = Tokens.end();
	(*Function) (sym, i, j, Encoding);

	for (; i != j; i++)
		delete *i;
}

void DefinitionDirective::Assemble (const Symbol *sym, Parser &p, vector<Byte> &Encoding) const
{
	// Converts the tokens into arguments.
	vector<Argument *> Arguments;
	p.ParseArguments (Arguments);
	unsigned int length = 0;
	int NumericalType;

	if (Arguments.size() == 0)
	{
		cout << "Expected at least one argument" << endl;
		return;
	}

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

		if ((immed->GetNumericalType() == FLOAT) && (!AcceptFloat))
		{
			RealNumber temp (immed->GetValue());

			// Performs cleanup
			for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
				delete *x;

			throw IntegerExpected (temp);
		}

		// The numerical type of this variable is the same as the first of its arguments
		if (x == Arguments.begin())
			NumericalType = immed->GetNumericalType();
		else
			if (NumericalType != immed->GetNumericalType())
			{
				cout << "Cannot mix integer and floating-point values" << endl;
			}

		immed->SetSize (Size);
		immed->Write (Encoding);
		length++;
	}

	if (sym != 0)
	{
		Variable *newvar = new Variable (sym->GetName(), Type(Size, Type::WEAK_MEMORY, UNDEFINED, NumericalType), length);
		CurrentAssembler->DefineSymbol (newvar);
	}

	for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
		delete *x;
}

void FunctionBITS (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	if (sym != 0)
	{
		CurrentAssembler->DefineSymbol (new Label (sym->GetName()));
	}

	// Converts the tokens into arguments.
	vector<Argument *> Arguments;
	Parser::ParseArguments (Arguments, i, j);

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

void FunctionEQU (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	Constant *c;

	if (sym == 0)
		throw NameMissing ();

	c = new Constant (sym->GetName(), Parser::EvaluateExpression (vector<Token *> (i, j)));
	CurrentAssembler->DefineSymbol (c);
}

void FunctionEQUAL (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	Constant *c;

	if (sym == 0)
		throw NameMissing ();

	BasicSymbol *bs = CurrentAssembler->Find (sym->GetName());
	if (bs != 0)
	{
		c = dynamic_cast<Constant *> (bs);
		if (c == 0)
			throw MultidefinedSymbol (bs->GetName());
	}
	else
	{
		c = new Constant (sym->GetName(), 0, true);
		CurrentAssembler->DefineSymbol (c);
	}

	c->SetValue (Parser::EvaluateExpression (vector<Token *> (i, j)));
}

void FunctionSEGMENT (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
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

void FunctionENDS (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
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

void FunctionPROC (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	if (sym == 0)
		throw NameMissing ();

	Procedure *NewProc;

	if (i == j)
	{
		NewProc = new Procedure (sym->GetName());
	}
	else
	{
		if (i + 1 != j)
		{
			cout << "Extra characters on line" << endl;
			throw 0;
		}

		Operator *op = dynamic_cast<Operator *> (*i);
		if (op == 0)
		{
			cout << "Procedures must be either NEAR or FAR." << endl;
			throw 0;
		}

		if (op->GetName() == "NEAR")
		{
			NewProc = new Procedure (sym->GetName(), NEAR);
		}
		else if (op->GetName() == "FAR")
		{
			NewProc = new Procedure (sym->GetName(), FAR);
		}
		else
		{
			cout << "Procedures must be either NEAR or FAR." << endl;
			throw 0;
		}
	}

	CurrentAssembler->AddProcedure (NewProc);
}

void FunctionENDP (const Symbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j, vector<Byte> &Encoding)
{
	if (sym == 0)
		throw NameMissing ();

	BasicSymbol *temp = CurrentAssembler->Find (sym->GetName());
	if (temp == 0)
	{
		cout << "Procedure not found: " << sym->GetName() << endl;
		return;
	}

	Procedure *proc = dynamic_cast<Procedure *> (temp);
	if (proc == 0)
	{
		cout << "Not a procedure name" << endl;
		return;
	}

	if (i != j)
	{
		cout << "Extra characters on line" << endl;
		throw 0;
	}

	CurrentAssembler->CloseProcedure(sym->GetName());
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
		new Directive ("ENDS", FunctionENDS),
		new Directive ("PROC", FunctionPROC),
		new Directive ("ENDP", FunctionENDP)
	};

	for (unsigned int i = 0; i < sizeof (Directives) / sizeof (Directive *); i++)
		DirectiveTable.Insert (Directives[i]);
}

char NameMissing::WhatString[] = "Symbol name missing.";
