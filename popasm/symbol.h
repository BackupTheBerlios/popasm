/***************************************************************************
                          symbol.h  -  description
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

#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>
#include <string>
#include <vector>
#include "lexnum.h"
#include "defs.h"
#include "express.h"

class MultidefinedSymbol : public exception
{
	string WhatString;

	public:
	MultidefinedSymbol (const string &name) throw ()
		: WhatString (string ("Symbol already defined elsewhere: ") + name) {}
	~MultidefinedSymbol () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

class UndefinedSymbol : public exception
{
	string WhatString;

	public:
	UndefinedSymbol (const string &name) throw ()
		: WhatString (string ("Undefined symbol: ") + name) {}
	~UndefinedSymbol () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

// Contains data necessary to describe a basic symbol (i.e. one which has no type)
class BasicSymbol
{
	string Name;
	unsigned int DefinitionPass;

	public:
	BasicSymbol (const string &n) throw ();
	virtual ~BasicSymbol () throw () {}

	const string &GetName () const throw () {return Name;}
	unsigned int GetDefinitionPass () const throw () {return DefinitionPass;}
	string Print () const throw () {return Name;}

	virtual BasicSymbol *Clone() const throw () {cout << "Not implemented: BasicSymbol::Clone()" << endl; return 0; }

	// This method is used to compare symbols' names, to put them in lexicographical order in a set
	virtual bool operator< (const BasicSymbol &s) const throw () {return Name < s.Name;}
};

class HashFunctor
{
	public:
	unsigned int operator() (const BasicSymbol * const &sd);
};

#endif
