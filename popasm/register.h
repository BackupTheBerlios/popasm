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

	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const Register *> (arg) != 0;}
	};

	template <unsigned int n>
	class CompareCodeFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg)
		{
			const Register *reg = dynamic_cast<const Register *> (arg);
			if (reg == 0) return false;
			return reg->GetCode() == n;
		}
	};

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

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const SegmentRegister *> (arg) != 0;}
	};

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// General-Purpose Registers
class GPRegister : public Register
{
	public:
	GPRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~GPRegister () throw () {};

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const GPRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};
/*
class Accumulator : public GPRegister
{
	// Makes constructor private, so no one can build it
	Accumulator () throw () : GPRegister ("", 0, 0) {}

	public:
	~Accumulator () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg)
		{
			const GPRegister *gpr = dynamic_cast<const GPRegister * const> (arg);
			if (gpr == 0) return false;
			return gpr->GetCode() == 0;
		}
	};
};
*/
class GPRegister8Bits : public GPRegister
{
	static GPRegister8Bits RegisterTable[];

	public:
	GPRegister8Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister8Bits () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const GPRegister8Bits *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister16Bits : public GPRegister
{
	static GPRegister16Bits RegisterTable[];

	public:
	GPRegister16Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister16Bits () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const GPRegister16Bits *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister32Bits : public GPRegister
{
	static GPRegister32Bits RegisterTable[];

	public:
	GPRegister32Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister32Bits () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const GPRegister32Bits *> (arg) != 0;}
	};

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

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const BaseRegister *> (arg) != 0;}
	};

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

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const IndexRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Control Registers
class ControlRegister : public Register
{
	static ControlRegister RegisterTable[];

	public:
	ControlRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~ControlRegister () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const ControlRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Test Registers
class TestRegister : public Register
{
	static TestRegister RegisterTable[];

	public:
	TestRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~TestRegister () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const TestRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Debug Registers
class DebugRegister : public Register
{
	static DebugRegister RegisterTable[];

	public:
	DebugRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~DebugRegister () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const DebugRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// MMX Registers
class MMXRegister : public Register
{
	static MMXRegister RegisterTable[];

	public:
	MMXRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~MMXRegister () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const MMXRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// XMM Registers
class XMMRegister : public Register
{
	static XMMRegister RegisterTable[];

	public:
	XMMRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~XMMRegister () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const XMMRegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class FPURegister : public Register
{
	static FPURegister RegisterTable[];

	public:
	FPURegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~FPURegister () throw () {}

	class IdFunctor : public Register::IdFunctor
	{
		public:
		bool operator() (const BasicArgument *arg) {return dynamic_cast<const FPURegister *> (arg) != 0;}
	};

	static Register *Read (const string &str, InputFile &inp) throw ();
};

#endif
