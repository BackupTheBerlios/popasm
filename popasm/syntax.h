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

#include <exception>
#include <string>
#include <vector>

#include "argument.h"
#include "opcode.h"
#include "register.h"

class Syntax
{
	protected:
	// Instruction basic encoding
	const Opcode Encoding;

	// Arguments accepted by this syntax
	vector<BasicArgument::IdFunctor *> ArgumentTypes;
	// Says if the order of the types of the arguments can be exchanged
	bool Interchangeable;

	public:
	enum OperandSizeDependsOn {FIRST_ARGUMENT, SECOND_ARGUMENT, THIRD_ARGUMENT, NOTHING, MODE_16BITS, MODE_32BITS, FULL_POINTER};

	protected:
	OperandSizeDependsOn OperandSizePrefixUsage;
	void WriteOperandSizePrefix (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ();
	void WriteOperandSizePrefix (Argument *arg, vector<Byte> &Output) const throw ();

	bool Match (vector<Argument *> &Arguments) const;

	public:
	Syntax (const Opcode &op, OperandSizeDependsOn dep, bool i = false) throw ()
		: Encoding(op), Interchangeable(i), OperandSizePrefixUsage(dep) {}
	virtual ~Syntax () throw ();

	virtual bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw () = 0;
};

// Caused by an internal error, when an argument is neither a register, nor a number, nor a memory location nor a full pointer
class UnknownArgument : public exception
{
	string WhatString;

	public:
	UnknownArgument (const BasicArgument *a) throw () : WhatString (string("Unknown argument ") + a->Print()) {}
	~UnknownArgument () throw () {}

	const char *what() const throw () {return WhatString.c_str();}
};

// Thrown when the user attempts to transfer control to a label too far from the current position
class JumpOutOfRange : public exception
{
	static const char WhatString[];

	public:
	JumpOutOfRange () throw () {}
	~JumpOutOfRange () throw () {}

	const char *what() const throw () {return WhatString;}
};

class InvalidSegmentOverride : public exception
{
	static const char WhatString[];

	public:
	InvalidSegmentOverride () throw () {}
	~InvalidSegmentOverride () throw () {}

	const char *what() const throw () {return WhatString;}
};

class AddressSizeMix : public exception
{
	static const char WhatString[];

	public:
	AddressSizeMix () throw () {}
	~AddressSizeMix () throw () {}

	const char *what() const throw () {return WhatString;}
};

// Syntaxes of instructions that take no arguments
class ZerarySyntax : public Syntax
{
	public:
	ZerarySyntax (const Opcode &op, OperandSizeDependsOn dep = NOTHING, BasicArgument::IdFunctor *arg1 = 0, BasicArgument::IdFunctor *arg2 = 0) throw ()
		: Syntax (op, dep)
	{
		if (arg1 != 0)
			ArgumentTypes.push_back (arg1);

		if (arg2 != 0)
			ArgumentTypes.push_back (arg2);
	}

	~ZerarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

class RetSyntax : public ZerarySyntax
{
	public:
	RetSyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg) : ZerarySyntax (op, dep, arg) {}
	~RetSyntax () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

// Syntaxes of instructions that take one argument
class UnarySyntax : public Syntax
{
	// Bits affected by direction and word bits
	Byte dw_mask;

	public:
	UnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg, Byte dwm = 0) throw ();
	UnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, Byte dwm = 0) throw ();
	~UnarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

// Syntaxes of instructions whose only argument is added to the basic encoding
class AdditiveUnarySyntax : public UnarySyntax
{
	public:
	AdditiveUnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg, Byte dwm = 0)
		throw () : UnarySyntax (op, dep, arg, dwm) {}
	AdditiveUnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg, BasicArgument::IdFunctor *arg2, Byte dwm = 0)
		throw () : UnarySyntax (op, dep, arg, dwm) {ArgumentTypes.push_back (arg2);}
	~AdditiveUnarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

// Syntax used by control transfer instructions, like CALL, JMP, etc.
class RelativeUnarySyntax : public UnarySyntax
{
	public:
	RelativeUnarySyntax (const Opcode &op, OperandSizeDependsOn dep, BasicArgument::IdFunctor *arg) throw () : UnarySyntax (op, dep, arg) {}
	~RelativeUnarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

// Syntaxes of instructions that take two argument
class BinarySyntax : public Syntax
{
	// Type check to be performed
	Argument::CheckType Check;

	// Bits affected by direction and word bits
	Byte dw_mask;

	// States the usage of the mod_reg_r/m byte. ABSENT means it does not exist at all, PARTIAL
	// means the reg field is an extension of the opcode, and PRESENT means the byte is a function
	// of the combination of the arguments.
	public:
	enum ModRegRM_Usage {ABSENT, PARTIAL, PRESENT, REPEATED, EXCHANGED_REGS};
	private:
	ModRegRM_Usage mrr_usage;

	public:
	BinarySyntax (const Opcode &op, OperandSizeDependsOn dep, bool i,
	              Argument::CheckType chk, Byte dwm, ModRegRM_Usage usage,
	              BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, BasicArgument::IdFunctor *arg3 = 0) throw ();
	~BinarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

class FPUBinarySyntax : public Syntax
{
	public:
	FPUBinarySyntax (const Opcode &op, BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2) throw ();
	~FPUBinarySyntax () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

class SuffixedBinarySyntax : public BinarySyntax
{
	Byte Suffix;

	public:
	SuffixedBinarySyntax (const Opcode &op, OperandSizeDependsOn dep, bool i,
	                     Argument::CheckType chk, Byte dwm, ModRegRM_Usage usage,
	                     BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, Byte suf) throw ();
	~SuffixedBinarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

class StringSyntax : public Syntax
{
	// Specifies which argument may be segment-overrided. A value graater than 1 means none.
	unsigned int Overrideable;

	// Type check to be performed
	Argument::CheckType Check;

	public:
	StringSyntax (const Opcode &op, OperandSizeDependsOn dep, Argument::CheckType chk,
		BasicArgument::IdFunctor *arg1, BasicArgument::IdFunctor *arg2, unsigned int ovr) throw ();
	~StringSyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

#endif
