/***************************************************************************
                          lexsym.cpp  -  description
                             -------------------
    begin                : Tue Jun 4 2002
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

#include "lexsym.h"
#include "register.h"
#include "command.h"

// Symbol table starts empty
HashTable<BasicSymbol *, HashFunctor, PointerComparator<BasicSymbol> > Symbol::SymbolTable;

Token *Symbol::Read (const string &str, InputFile &inp) throw ()
{
	// Tests for registers
	BasicSymbol *t = Register::Read (str, inp);
	if (t != 0) return new Symbol (t, false);

	// Tests for symbols defined in the symbol table
	BasicSymbol bs (str);
	BasicSymbol * const *sd = SymbolTable.Find (&bs);
	if (sd != 0) return new Symbol (*sd, false);

	t = Command::Read (str, inp);
	if (t != 0) return new Symbol (t, false);

	// Returns zero if not found
	return 0;
}

void Symbol::SetData (BasicSymbol *bs, bool own) throw ()
{
	if (Owner) delete s;
	s = bs;
	Owner = own;
}

void Symbol::DefineSymbol (BasicSymbol *s) throw (MultidefinedSymbol)
{
	BasicSymbol * const * sym = SymbolTable.Find (s);

	if (sym == 0)
	{
		SymbolTable.Insert (new Label (s->GetName()));
	}
	else
	{
		// Symbol defined two or more times
		throw MultidefinedSymbol (s->GetName());
	}
}
