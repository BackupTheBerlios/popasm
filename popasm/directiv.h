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

#include "command.h"
#include "hashtab.h"
#include "symbol.h"

// Assembler directives, such as DB, DW, STRUCT, etc
class Directive : public Command
{
	static HashTable<Directive *, HashFunctor, PointerComparator<BasicSymbol> > DirectiveTable;

	public:
	Directive (const string &n) throw () : Command (n) {}
	~Directive () throw () {}

	static void SetupDirectiveTable () throw ();

	static BasicSymbol *Read (const string &str, InputFile &inp);
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
};

#endif

