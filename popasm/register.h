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

class Register : public BasicSymbol
{
	unsigned int Size;	// Register size in bits
	unsigned int Code;	// Register code

	public:
	Register (const string &n, unsigned int s, unsigned int c) throw () : BasicSymbol (n), Size (s), Code (c) {}
	~Register () throw () = 0;

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Segment Registers
class SegmentRegister : public Register
{
	static SegmentRegister RegisterTable[];

	public:
	SegmentRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~SegmentRegister () throw () {}

	// Attempts to read a register from the given string. Gets more from inp if necessary. Returns 0 if failed.
	static Register *Read (const string &str, InputFile &inp) throw ();
};

// General-Purpose Registers
class GPRegister : public Register
{
	static GPRegister RegisterTable[];

	public:
	GPRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~GPRegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Control Registers
class ControlRegister : public Register
{
	static ControlRegister RegisterTable[];

	public:
	ControlRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~ControlRegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Test Registers
class TestRegister : public Register
{
	static TestRegister RegisterTable[];

	public:
	TestRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~TestRegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// Debug Registers
class DebugRegister : public Register
{
	static DebugRegister RegisterTable[];

	public:
	DebugRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~DebugRegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// MMX Registers
class MMXRegister : public Register
{
	static MMXRegister RegisterTable[];

	public:
	MMXRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~MMXRegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

// XMM Registers
class XMMRegister : public Register
{
	static XMMRegister RegisterTable[];

	public:
	XMMRegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~XMMRegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

class FPURegister : public Register
{
	static FPURegister RegisterTable[];

	public:
	FPURegister (const string &n, unsigned int s, unsigned int c) throw () : Register (n, s, c) {}
	~FPURegister () throw () {}

	static Register *Read (const string &str, InputFile &inp) throw ();
};

#endif
