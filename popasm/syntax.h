/***************************************************************************
                          syntax.h  -  description
                             -------------------
    begin                : Tue Jun 18 2002
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

#ifndef SYNTAX_H
#define SYNTAX_H

#include <vector>
#include "argument.h"
#include "opcode.h"

class Syntax
{
	Opcode Encoding;

	enum {MaximumArity = 3};
	const BasicArgument *ArgumentTypes[MaximumArity];

	// Says if the order of the types of the arguments can be exchanged
	bool Interchangeable;
	// Type check to be performed
	Argument::CheckType Check;
	// Precedence (priority) of this syntax. The ones above OptimizedPrecedence
	// should not throw exceptions if they fail
	unsigned int Precedence;
	enum {OptimizedPrecedence = 10000};

	// Bits affected by direction and word bits
	Byte dw_mask;
	// Argument that defines the operand-size prefix (0 = first, 1 = second, -1 = none)
	int SizeArgument;

	// Operating mode the instruction works on by default. Eg. PUSHA works in 16 bits mode, while PUSHAD in 32.
	// Useful only by zerary syntaxes
	unsigned int DefaultMode;

	// States the usage of the mod_reg_r/m byte. ABSENT means it does not exist at all, PARTIAL
	// means the reg field is an extension of the opcode, and PRESENT means the byte is a function
	// of the combination of the arguments.
	public:
	enum ModRegRM_Usage {ABSENT, PARTIAL, PRESENT};
	private:
	ModRegRM_Usage mrr_usage;

	bool Match (vector<Argument *> &Arguments) const throw ();
	unsigned int GetArity () const throw ();

	public:
	Syntax (unsigned int p, const Opcode &op, Byte dwm, int szarg = -1, unsigned int dm = 16) throw ();
	Syntax (unsigned int p, const Opcode &op, const BasicArgument *t0, Byte dwm, ModRegRM_Usage usage, int szarg = 0) throw ();
	Syntax (unsigned int p, const Opcode &op, const BasicArgument *t0, const BasicArgument *t1,
		Argument::CheckType ct, bool i, Byte dwm, ModRegRM_Usage usage, int szarg = 0) throw ();

	~Syntax () throw () {}

	bool operator< (const Syntax &s) const throw() {return Precedence < s.Precedence;}
	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output, unsigned int CurrentMode) const;
};

#endif
