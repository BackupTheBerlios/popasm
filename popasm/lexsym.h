/***************************************************************************
                          lexsym.h  -  description
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

#ifndef LEXSYM_H
#define LEXSYM_H

#include "lexical.h"
#include "symbol.h"
#include "hashtab.h"
#include "inp_file.h"
#include "functors.h"

class Symbol : public Token
{
	protected:
	static HashTable<BasicSymbol *, HashFunctor, PointerComparator<BasicSymbol> > SymbolTable;

	BasicSymbol *s;
	bool Owner;

	public:
	Symbol (BasicSymbol *bs, bool own) : s(bs), Owner(own) {}
	~Symbol () {if (Owner) delete s;}

	// Attempts to read a symbol from the given string.
	// Returns 0 if there's currently no symbol with that name in the SymbolTable.
	// The returned token might be a Number, if the symbol found has a value or offset
	static Token *Read (const string &str, InputFile &inp) throw ();
	static void DefineSymbol (BasicSymbol *s);

	string Print() const throw () {return s->Print();}

	const string &GetName () const throw () {return s->GetName();}
	const BasicSymbol *GetData () const throw () {return s;}
	void SetData (BasicSymbol *bs, bool own) throw ();

	virtual Symbol *Clone () const throw () {return new Symbol (Owner ? s->Clone() : s, false);}
	virtual bool operator== (const Symbol &x) const throw () {return s == x.s;}
};

#endif
