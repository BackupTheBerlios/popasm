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
	// Precedence (priority) of this syntax. The ones above OptimizedPrecedence
	// should not throw exceptions if they fail
	const unsigned int Precedence;
	enum {OptimizedPrecedence = 10000};

	// Instruction basic encoding
	const Opcode Encoding;

	// Arguments accepted by this syntax
	vector<BasicIdFunctor *> ArgumentTypes;
	// Says if the order of the types of the arguments can be exchanged
	bool Interchangeable;

	public:
	enum OperandSizeDependsOn {FIRST_ARGUMENT, SECOND_ARGUMENT, THIRD_ARGUMENT, NOTHING, MODE_16BITS, MODE_32BITS, FULL_POINTER};

	protected:
	OperandSizeDependsOn OperandSizePrefixUsage;
	void WriteOperandSizePrefix (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ();
	void WriteOperandSizePrefix (Argument *arg, vector<Byte> &Output) const throw ();

	bool Match (vector<Argument *> &Arguments) const throw ();

	public:
	Syntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, bool i = false) throw ()
		: Precedence(p), Encoding(op), Interchangeable(i), OperandSizePrefixUsage(dep) {}
	virtual ~Syntax () throw ();

	bool operator< (const Syntax &s) const throw() {return Precedence < s.Precedence;}
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

// Syntaxes of instructions that take no arguments
class ZerarySyntax : public Syntax
{
	public:
	ZerarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep) throw () : Syntax (p, op, dep) {}
	ZerarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg) throw ()
		: Syntax (p, op, dep) {ArgumentTypes.push_back (arg);}
	ZerarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg, BasicIdFunctor *arg2) throw ()
		: Syntax (p, op, dep) {ArgumentTypes.push_back (arg); ArgumentTypes.push_back (arg2);}
	~ZerarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ();
};

// Syntaxes of instructions that take one argument
class UnarySyntax : public Syntax
{
	// Bits affected by direction and word bits
	Byte dw_mask;

	public:
	UnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg, Byte dwm = 0) throw ();
	UnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg1, BasicIdFunctor *arg2, Byte dwm = 0) throw ();
	~UnarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw (UnknownArgument);
};

// Syntaxes of instructions whose only argument is added to the basic encoding
class AdditiveUnarySyntax : public UnarySyntax
{
	public:
	AdditiveUnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg, Byte dwm = 0)
		throw () : UnarySyntax (p, op, dep, arg, dwm) {}
	AdditiveUnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg, BasicIdFunctor *arg2, Byte dwm = 0)
		throw () : UnarySyntax (p, op, dep, arg, dwm) {ArgumentTypes.push_back (arg2);}
	~AdditiveUnarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw ();
};

// Syntax used by control transfer instructions, like CALL, JMP, etc.
class RelativeUnarySyntax : public UnarySyntax
{
	public:
	RelativeUnarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, BasicIdFunctor *arg) throw () : UnarySyntax (p, op, dep, arg) {}
	~RelativeUnarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const throw (JumpOutOfRange);
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
	enum ModRegRM_Usage {ABSENT, PARTIAL, PRESENT, REPEATED};
	private:
	ModRegRM_Usage mrr_usage;

	public:
	BinarySyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, bool i,
	              Argument::CheckType chk, Byte dwm, ModRegRM_Usage usage,
	              BasicIdFunctor *arg1, BasicIdFunctor *arg2, BasicIdFunctor *arg3 = 0) throw ();
	~BinarySyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

class FPUBinarySyntax : public Syntax
{
	public:
	FPUBinarySyntax (unsigned int p, const Opcode &op, BasicIdFunctor *arg1, BasicIdFunctor *arg2) throw ();
	~FPUBinarySyntax () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

class StringSyntax : public Syntax
{
	bool Overrideable;

	// Type check to be performed
	Argument::CheckType Check;

	public:
	StringSyntax (unsigned int p, const Opcode &op, BasicIdFunctor *arg1, bool ovr) throw ();
	StringSyntax (unsigned int p, const Opcode &op, OperandSizeDependsOn dep, Argument::CheckType chk,
		BasicIdFunctor *arg1, BasicIdFunctor *arg2) throw ();
	~StringSyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

#endif
