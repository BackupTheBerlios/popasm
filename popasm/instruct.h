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
	static HashTable<Instruction *, HashFunctor, PointerComparator<BasicSymbol> > InstructionTable;

	typedef multiset <const Syntax *, PointerComparator<Syntax> > ContainerType;
	ContainerType Syntaxes;

	protected:
	void AddSyntax (const Syntax *s) throw () {Syntaxes.insert (s);}

	public:
	Instruction (const string &n) throw () : Command (n) {}
	Instruction (const string &n, const Syntax *s) throw () : Command (n) {AddSyntax(s);}
	Instruction (const string &n, const Syntax *s, const Syntax *s2) throw () : Command (n) {AddSyntax(s); AddSyntax(s2);}
	Instruction (const string &n, const Syntax *s, const Syntax *s2, const Syntax *s3) throw () : Command (n)
		{AddSyntax(s); AddSyntax(s2); AddSyntax(s3);}
	Instruction (const string &n, const Syntax *s, const Syntax *s2, const Syntax *s3, const Syntax *s4) throw () : Command (n)
		{AddSyntax(s); AddSyntax(s2); AddSyntax(s3); AddSyntax(s4);}
	~Instruction () throw ();

	static void SetupInstructionTable () throw ();
	static BasicSymbol *Read (const string &str, InputFile &inp);

	void Assemble (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding, unsigned int CurrentMode) const;
};

// Instructions that take two arguments. Any combination of register, memory and immediate are allowed.
// Byte immediate values can be signed extended and accumulators have special optimized opcode.
// Classical examples are ADC, ADD, AND, etc.
class OptimizedBinaryInstruction : public Instruction
{
	public:
	OptimizedBinaryInstruction (const string &nm, const Opcode &Accum, const Opcode &Immed,
		const Opcode &Immed8, const Opcode &RegMem) throw ();
	~OptimizedBinaryInstruction () throw () {}
};

#endif
