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

#include <typeinfo>

#include "segment.h"
#include "lexsym.h"
#include "asmer.h"
#include "defs.h"

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
		// Checks if the existing entry in the symbol table is provisory (forward reference)
		if (typeid (**sym) == typeid (BasicSymbol))
		{
			// Yes, it is. Replace it for the new definition unless it is also provisory
			if (typeid (*s) != typeid (BasicSymbol))
			{
				UndefineSymbol (*sym);
				DefineSymbol (s);
			}
		}
		else
		{
			// No, it is a full-featured symbol. Check whether we are in the same pass
			// the previous symbol was defined. If so we have a multidefined symbol.
			if ((*sym)->GetDefinitionPass() == CurrentAssembler->GetCurrentPass())
				throw MultidefinedSymbol (s->GetName());
			else
			{
				// If the symbol changed since last pass request another one
				if ((*sym)->Changed (s))
					CurrentAssembler->RequestNewPass ();

				delete s;
			}
		}
	}
}

void Segment::UndefineSymbol (BasicSymbol *s) throw ()
{
	SymbolTable.Remove (s);
	delete (s);
}

BasicSymbol *Segment::Find (const string &name)
{
	BasicSymbol bs (name);
	BasicSymbol * const *sd = SymbolTable.Find (&bs);

	if (sd != 0)
		return *sd;

	return 0;
}
