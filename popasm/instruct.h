/***************************************************************************
                          instruct.h  -  description
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

#ifndef INSTRUCT_H
#define INSTRUCT_H

#include <set>
#include <string>
#include <vector>
#include <exception>

#include "argument.h"
#include "command.h"
#include "hashtab.h"
#include "inp_file.h"
#include "memory.h"
#include "opcode.h"
#include "register.h"
#include "syntax.h"

// Machine instructions
class Instruction : public Command
{
	static HashTable<Instruction *, HashFunctor, PointerComparator<Instruction> > InstructionTable;

	vector<const Syntax *> Syntaxes;

	protected:
	void AddSyntax (const Syntax *s) throw () {Syntaxes.push_back (s);}

	public:
	Instruction (const string &n,
		const Syntax *s1  = 0, const Syntax *s2  = 0, const Syntax *s3  = 0, const Syntax *s4  = 0,
		const Syntax *s5  = 0, const Syntax *s6  = 0, const Syntax *s7  = 0, const Syntax *s8  = 0,
		const Syntax *s9  = 0, const Syntax *s10 = 0, const Syntax *s11 = 0, const Syntax *s12 = 0) throw ();
	~Instruction () throw () {}

	static void SetupInstructionTable () throw ();
	static BasicSymbol *Read (const string &str, InputFile &inp);

	void Assemble (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j,
		vector<Byte> &Encoding) const;
};

class InvalidSyntax : public exception
{
	const Command *BadCommand;
	vector<Argument *> BadArguments;
	string WhatString;

	public:
	InvalidSyntax (const Command *cmd, const vector<Argument *> args) throw ();
	~InvalidSyntax () throw () {}

	const char *what() const throw ();
};

#endif
