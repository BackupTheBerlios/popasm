/***************************************************************************
                          prefix.h  -  description
                             -------------------
    begin                : Mon Jan 27 2003
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

#ifndef PREFIX_H
#define PREFIX_H

#include <string>
#include <exception>

#include "opcode.h"
#include "command.h"
#include "hashtab.h"
#include "symbol.h"

// Thrown when a command is expected but something else was got
class InstructionExpected : public exception
{
	const Token *t;
	string WhatString;

	public:
	InstructionExpected (const Token *tt) throw ();
	~InstructionExpected () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};


// Prefix instructions like LOCK, REP, etc.
class PrefixInstruction : public Command
{
	static HashTable<PrefixInstruction *, HashFunctor, PointerComparator<PrefixInstruction> > PrefixTable;

	const Opcode Encoding;

	public:
	PrefixInstruction (const string &n, const Opcode &op) throw () : Command (n), Encoding(op) {}
	~PrefixInstruction () throw () {}

	void Assemble (const Symbol *sym, Parser &p, vector<Byte> &Encoding) const;
	static BasicSymbol *Read (const string &str, InputFile &inp);
	static void SetupPrefixTable ();
};

#endif
