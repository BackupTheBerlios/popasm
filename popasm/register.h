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
#include <typeinfo>

#include "symbol.h"
#include "inp_file.h"
#include "argument.h"

class UndefinedArgument;

class Register : public BasicArgument, public BasicSymbol
{
	unsigned int Code;	// Register code

	public:
	Register (const string &n = "", unsigned int s = 0, unsigned int c = 0) throw () : BasicArgument (s, UNDEFINED), BasicSymbol (n), Code (c) {}
	~Register () throw () {}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
	unsigned int GetCode () const throw () {return Code;}

	static Argument *MakeArgument (const Expression &e) throw (InvalidArgument, exception);

	template <class T, Byte code, int size>
	class IdFunctor : public BasicArgument::IdFunctor
	{
		public:
		bool operator() (Argument &arg)
		{
			const T *reg = dynamic_cast<const T *> (arg.GetData());

			if (reg == 0)
			{
				if (arg.IsUndefined())
				{
					arg.SetData (T::GetRegister(code, GetFirstSize(size)), false);
					return true;
				}

				return false;
			}

			return ((code == ANY) || (code == reg->GetCode())) && MatchSize (size, reg->GetSize());
		}
	};

	virtual string Print () const throw () {return BasicSymbol::Print();}
};

// Segment Registers
class SegmentRegister : public Register
{
	static SegmentRegister RegisterTable[];

	Byte PrefixCode;

	public:
	SegmentRegister (const string &n, unsigned int s, unsigned int c, Byte p) throw ()
		: Register (n, s, c), PrefixCode (p) {}
	~SegmentRegister () throw () {}

	Byte GetPrefixCode () const throw () {return PrefixCode;}
	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// General-Purpose Registers
class GPRegister : public Register
{
	public:
	GPRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~GPRegister () throw () {};

	static const Register *GetRegister (Byte code, int size) throw ();
	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister8Bits : public GPRegister
{
	static GPRegister8Bits RegisterTable[];

	public:
	GPRegister8Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister8Bits () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister16Bits : public GPRegister
{
	static GPRegister16Bits RegisterTable[];

	public:
	GPRegister16Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister16Bits () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

class GPRegister32Bits : public GPRegister
{
	static GPRegister32Bits RegisterTable[];

	public:
	GPRegister32Bits (const string &n, unsigned int s, unsigned int c) throw () : GPRegister (n, s, c) {}
	~GPRegister32Bits () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
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

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Control Registers
class ControlRegister : public Register
{
	static ControlRegister RegisterTable[];

	public:
	ControlRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~ControlRegister () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Test Registers
class TestRegister : public Register
{
	static TestRegister RegisterTable[];

	public:
	TestRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~TestRegister () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Debug Registers
class DebugRegister : public Register
{
	static DebugRegister RegisterTable[];

	public:
	DebugRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~DebugRegister () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// MMX Registers
class MMXRegister : public Register
{
	static MMXRegister RegisterTable[];

	public:
	MMXRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~MMXRegister () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// XMM Registers
class XMMRegister : public Register
{
	static XMMRegister RegisterTable[];

	public:
	XMMRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~XMMRegister () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

class FPURegister : public Register
{
	static FPURegister RegisterTable[];

	public:
	FPURegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~FPURegister () throw () {}

	static const Register *GetRegister (Byte code, int size) throw () {return (code == 255) ? RegisterTable : RegisterTable + code;}
	static Register *Read (const string &str, InputFile &inp) throw ();
};

#endif
