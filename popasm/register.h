/***************************************************************************
                          register.h  -  description
                             -------------------
    copyright            : (C) 2001 by Helcio Mello
    email                : helcio@users.sourceforge.net
 ***************************************************************************/

//--------------------------------------------------------------------------
// Gathers data necessary to model registers
//--------------------------------------------------------------------------

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef REGISTER_H
#define REGISTER_H

#include <string>

#include "symbol.h"
#include "inp_file.h"
#include "argument.h"

class Register : public BasicSymbol, public BasicArgument
{
	unsigned int Code;	// Register code

	public:
	Register (const string &n, unsigned int s, unsigned int c) throw () : BasicSymbol (n), BasicArgument (s), Code (c) {}
	~Register () throw () {}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
	unsigned int GetCode () const throw () {return Code;}

	// Checks whether ptr is a pointer to an instance of this class.
	static const Register * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const Register * const> (ptr) != 0;}

	virtual string Print () const throw () {return BasicSymbol::Print();}
};

// Segment Registers
class SegmentRegister : public Register
{
	static SegmentRegister RegisterTable[];

	unsigned int PrefixCode;

	public:
	SegmentRegister (const string &n, unsigned int s, unsigned int c, unsigned int p) throw ()
		: Register (n, s, c), PrefixCode (p) {}
	~SegmentRegister () throw () {}

	unsigned int GetPrefixCode () const throw () {return PrefixCode;}

	// Checks whether ptr is a pointer to an instance of this class.
	static const SegmentRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const SegmentRegister * const> (ptr) != 0;}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// General-Purpose Registers
class GPRegister : public Register
{
	public:
	GPRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	GPRegister (const GPRegister &gpr) throw () : Register (gpr) {}
	~GPRegister () throw () {};

	// Checks whether ptr is a pointer to an instance of this class.
	static const GPRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const GPRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class Accumulator : public GPRegister
{
	public:
	Accumulator (const GPRegister &gpr) : GPRegister (gpr) {if (gpr.GetCode() != 0) throw 0;}
	Accumulator () throw () : GPRegister ("", 0, 0) {}
	~Accumulator () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const Accumulator * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
	{
		const GPRegister * const gpr = dynamic_cast<const GPRegister * const> (ptr);
		if (gpr == 0) return false;
		return gpr->GetCode() == 0;
	}
};

class GPRegister8Bits : public GPRegister
{
	static GPRegister8Bits RegisterTable[];

	public:
	GPRegister8Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister8Bits () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const GPRegister8Bits * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const GPRegister8Bits * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister16Bits : public GPRegister
{
	static GPRegister16Bits RegisterTable[];

	public:
	GPRegister16Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister16Bits () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const GPRegister16Bits * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const GPRegister16Bits * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister32Bits : public GPRegister
{
	static GPRegister32Bits RegisterTable[];

	public:
	GPRegister32Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister32Bits () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const GPRegister32Bits * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const GPRegister32Bits * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class BaseRegister : public GPRegister16Bits
{
	static BaseRegister RegisterTable[];

	public:
	Byte BaseCode, BaseIndexCode;

	BaseRegister (const string &n, unsigned int s, unsigned int c, Byte bc, Byte bic) throw ()
		: GPRegister16Bits (n, s, c), BaseCode (bc), BaseIndexCode (bic) {}
	~BaseRegister () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const BaseRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const BaseRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class IndexRegister : public GPRegister16Bits
{
	static IndexRegister RegisterTable[];

	public:
	Byte BaseCode, IndexCode;

	IndexRegister (const string &n, unsigned int s, unsigned int c, Byte bc, Byte ic) throw ()
		: GPRegister16Bits (n, s, c), BaseCode (bc), IndexCode (ic) {}
	~IndexRegister () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const IndexRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const IndexRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Control Registers
class ControlRegister : public Register
{
	static ControlRegister RegisterTable[];

	public:
	ControlRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~ControlRegister () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const ControlRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const ControlRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Test Registers
class TestRegister : public Register
{
	static TestRegister RegisterTable[];

	public:
	TestRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~TestRegister () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const TestRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const TestRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Debug Registers
class DebugRegister : public Register
{
	static DebugRegister RegisterTable[];

	public:
	DebugRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~DebugRegister () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const DebugRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const DebugRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// MMX Registers
class MMXRegister : public Register
{
	static MMXRegister RegisterTable[];

	public:
	MMXRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~MMXRegister () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const MMXRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const MMXRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// XMM Registers
class XMMRegister : public Register
{
	static XMMRegister RegisterTable[];

	public:
	XMMRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~XMMRegister () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const XMMRegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const XMMRegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class FPURegister : public Register
{
	static FPURegister RegisterTable[];

	public:
	FPURegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~FPURegister () throw () {}

	// Checks whether ptr is a pointer to an instance of this class.
	static const FPURegister * const ClassInstance;
	bool Identify (const BasicArgument * const ptr) const throw ()
		{return dynamic_cast<const FPURegister * const> (ptr) != 0;}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

#endif
