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

BasicSymbol *Directive::Read (const string &str, InputFile &inp)
{
	Directive s (str);
	Directive * const *d = DirectiveTable.Find (&s);
	return (d == 0) ? 0 : *d;
}

HashTable <Directive *, HashFunctor, PointerComparator<BasicSymbol> > Directive::DirectiveTable =
	HashTable <Directive *, HashFunctor, PointerComparator<BasicSymbol> > ();

void FunctionBITS (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding)
{
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

	const Immediate *i = dynamic_cast<const Immediate *> (Arguments[0]->GetData());
   if (i == 0)
	{
		cout << "Must be a constant expression." << endl;
		return;
	}

	CurrentAssembler->SetCurrentMode (static_cast<IntegerNumber> (i->GetValue()).GetValue(false));
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
