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

#include <exception>

#include "lexical.h"
#include "symbol.h"
#include "hashtab.h"
#include "inp_file.h"
#include "functors.h"

class Symbol : public Token
{
	BasicSymbol *s;
	bool Owner;

	public:
	Symbol (BasicSymbol *bs, bool own) throw () : s(bs), Owner(own) {}
	~Symbol () throw () {if (Owner) delete s;}

	// Attempts to read a symbol from the given string.
	// Returns 0 if there's currently no symbol with that name in the SymbolTable.
	static Token *Read (const string &str, InputFile &inp) throw ();

	string Print() const throw () {return s->Print();}

	const string &GetName () const throw () {return s->GetName();}
	const BasicSymbol *GetData () const throw () {return s;}
	void SetData (BasicSymbol *bs, bool own) throw ();

	virtual Symbol *Clone () const throw () {return new Symbol (Owner ? s->Clone() : s, Owner);}
	virtual bool operator== (const Symbol &x) const throw () {return s == x.s;}
};

#endif
