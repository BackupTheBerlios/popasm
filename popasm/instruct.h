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
#include <typeinfo>

#include "argument.h"
#include "command.h"
#include "hashtab.h"
#include "inp_file.h"
#include "memory.h"
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

	void Assemble (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding) const;
};

// Instructions that take no arguments
class ZeraryInstruction : public Instruction
{
	public:
	ZeraryInstruction (const string &nm, unsigned int n, Byte b0, Byte b1, Byte b2) throw ();
	~ZeraryInstruction () throw () {}
};

// Instructions that take one argument, but have a default form with no arguments
class ZeraryUnaryInstruction : public Instruction
{
	public:
	ZeraryUnaryInstruction (const string &nm,
		unsigned int n0, Byte b00, Byte b01, Byte b02,
		unsigned int n1, Byte b10, Byte b11, Byte b12, const type_info *t1) throw ();
	~ZeraryUnaryInstruction () throw () {}
};

// Instructions that take two arguments. Any combination of register, memory and immediate are allowed.
class BinaryInstruction : public Instruction
{
	public:
	BinaryInstruction (const string &nm, Byte Accum, Byte Immed, Byte Immed8, Byte ConstReg, Byte RegMem) throw ();
	~BinaryInstruction () throw () {}
};

#endif
