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

unsigned int HashFunctor::operator() (BasicSymbol * const &sd)
{
	unsigned int x = 0;

	for (string::const_iterator i = (sd->GetName()).begin(); i != (sd->GetName()).end(); i++)
		x += *i;

	return x;
}

// Symbol table starts empty
HashTable<BasicSymbol *, HashFunctor, PointerComparator<BasicSymbol> > Symbol::SymbolTable;

Token *Symbol::Read (const string &str, InputFile &inp) throw ()
{
	BasicSymbol *t = Register::Read (str, inp);
	if (t != 0) return new Symbol (t);

	BasicSymbol bs (str);
	const BasicSymbol * const *sd = SymbolTable.Find (&bs);

	// Returns zero if not found
	if (sd == 0) return 0;

	return new Symbol (*sd);
}
