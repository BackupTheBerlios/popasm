/***************************************************************************
                          segment.cpp  -  description
                             -------------------
    begin                : Wed Feb 5 2003
    copyright            : (C) 2003 by Helcio Mello
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

#include "segment.h"
#include "lexsym.h"

Token *Segment::Read (const string &str, InputFile &inp) throw ()
{
	BasicSymbol *temp = Find (str);

	if (temp == 0)
		return 0;

	return new Symbol (temp, false);
}

void Segment::DefineSymbol (BasicSymbol *s) throw (MultidefinedSymbol)
{
	BasicSymbol * const * sym = SymbolTable.Find (s);

	if (sym == 0)
	{
		SymbolTable.Insert (s);
	}
	else
	{
		// Symbol defined two or more times
		throw MultidefinedSymbol (s->GetName());
	}
}

BasicSymbol *Segment::Find (const string &name)
{
	BasicSymbol bs (name);
	BasicSymbol * const *sd = SymbolTable.Find (&bs);

	if (sd != 0)
		return *sd;

	return 0;
}
