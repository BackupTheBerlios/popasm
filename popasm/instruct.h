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
	~Instruction () throw ();

	static void SetupInstructionTable () throw ();
	static BasicSymbol *Read (const string &str, InputFile &inp);

	void Assemble (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding, unsigned int CurrentMode) const;
};

// Instructions that take no arguments
class ZeraryInstruction : public Instruction
{
	public:
	ZeraryInstruction (const string &nm, const Opcode &op) throw ();
	~ZeraryInstruction () throw () {}
};

// Instructions that take one argument, but have a default form with no arguments
class ZeraryUnaryInstruction : public Instruction
{
	public:
	ZeraryUnaryInstruction (const string &nm, const Opcode &op0, const Opcode &op1, const BasicArgument * const t0) throw ();
	~ZeraryUnaryInstruction () throw () {}
};

// ARPL-like instruction
class ARPLInstruction : public Instruction
{
	public:
	ARPLInstruction (const string &nm, const Opcode &op) throw ();
	~ARPLInstruction () throw () {}
};

// Instructions that take two arguments. Any combination of register, memory and immediate are allowed.
// Byte immediate values can be signed extended and accumulators have special optimized opcode.
// Classical examples are ADC, ADD, AND, etc.
class OptimizedBinaryInstruction : public Instruction
{
	public:
	OptimizedBinaryInstruction (const string &nm, const Opcode &Accum, const Opcode &Immed,
		const Opcode &Immed8, const Opcode &RegMem, Byte ConstReg) throw ();
	~OptimizedBinaryInstruction () throw () {}
};

#endif
