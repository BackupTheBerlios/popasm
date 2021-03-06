/***************************************************************************
                          directiv.h  -  description
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

#ifndef DIRECTIV_H
#define DIRECTIV_H

#include <string>
#include <vector>
#include <exception>

#include "command.h"
#include "hashtab.h"
#include "symbol.h"
#include "lexsym.h"
#include "constant.h"

typedef void (*DirectiveFunction) (const Symbol *sym, vector<Token *> &Tokens, vector<Byte> &Encoding);

// Assembler directives, such as DB, DW, STRUCT, etc
class Directive : public Command
{
	static HashTable<Directive *, HashFunctor, PointerComparator<BasicSymbol> > DirectiveTable;

	DirectiveFunction Function;

	public:
	Directive (const string &n, DirectiveFunction df = 0) throw () : Command (n), Function (df) {}
	~Directive () throw () {}

	static void SetupDirectiveTable () throw ();
	static BasicSymbol *Read (const string &str, InputFile &inp);

	void Assemble (const Symbol *sym, Parser &p, vector<Byte> &Encoding) const;
};

// Directives that define data, like DB, DW, etc.
class DefinitionDirective : public Directive
{
	unsigned int Size;	// Size of element, in bits
	bool AcceptFloat;		// States if the command accepts floating-point arguments

	public:
	DefinitionDirective (const string &n, unsigned int sz, bool accept) throw ()
		: Directive (n), Size(sz), AcceptFloat(accept) {}
	~DefinitionDirective () throw () {}

	void Assemble (const Symbol *sym, Parser &p, vector<Byte> &Encoding) const;
};

class NameMissing : public exception
{
	static char WhatString[];

	public:
	NameMissing () throw () {}
	~NameMissing () throw () {}

	const char *what() const throw () {return WhatString;}
};

#endif
