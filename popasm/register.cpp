/***************************************************************************
                          register.cpp  -  description
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

#include "register.h"

Register::~Register () throw () {}

SegmentRegister SegmentRegister:: RegisterTable[] =
{
	SegmentRegister ("ES", 16, 0),
	SegmentRegister ("CS", 16, 1),
	SegmentRegister ("SS", 16, 2),
	SegmentRegister ("DS", 16, 3),
	SegmentRegister ("FS", 16, 4),
	SegmentRegister ("GS", 16, 5)
};

Register *SegmentRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (SegmentRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

GPRegister GPRegister:: RegisterTable[] =
{
	GPRegister ("EAX", 32, 0),
	GPRegister ("ECX", 32, 1),
	GPRegister ("EDX", 32, 2),
	GPRegister ("EBX", 32, 3),
	GPRegister ("ESP", 32, 4),
	GPRegister ("EBP", 32, 5),
	GPRegister ("ESI", 32, 6),
	GPRegister ("EDI", 32, 7),

	GPRegister ("AX", 16, 0),
	GPRegister ("CX", 16, 1),
	GPRegister ("DX", 16, 2),
	GPRegister ("BX", 16, 3),
	GPRegister ("SP", 16, 4),
	GPRegister ("BP", 16, 5),
	GPRegister ("SI", 16, 6),
	GPRegister ("DI", 16, 7),

	GPRegister ("AL", 8, 0),
	GPRegister ("CL", 8, 1),
	GPRegister ("DL", 8, 2),
	GPRegister ("BL", 8, 3),
	GPRegister ("AH", 8, 4),
	GPRegister ("CH", 8, 5),
	GPRegister ("DH", 8, 6),
	GPRegister ("BH", 8, 7)
};

Register *GPRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (GPRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

ControlRegister ControlRegister:: RegisterTable[] =
{
	ControlRegister ("CR0", 32, 0),
	ControlRegister ("CR1", 32, 1),
	ControlRegister ("CR2", 32, 2),
	ControlRegister ("CR3", 32, 3),
	ControlRegister ("CR4", 32, 4),
	ControlRegister ("CR5", 32, 5),
	ControlRegister ("CR6", 32, 6),
	ControlRegister ("CR7", 32, 7)
};

Register *ControlRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (ControlRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

TestRegister TestRegister:: RegisterTable[] =
{
	TestRegister ("TR0", 32, 0),
	TestRegister ("TR1", 32, 1),
	TestRegister ("TR2", 32, 2),
	TestRegister ("TR3", 32, 3),
	TestRegister ("TR4", 32, 4),
	TestRegister ("TR5", 32, 5),
	TestRegister ("TR6", 32, 6),
	TestRegister ("TR7", 32, 7)
};

Register *TestRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (TestRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

DebugRegister DebugRegister:: RegisterTable[] =
{
	DebugRegister ("DR0", 32, 0),
	DebugRegister ("DR1", 32, 1),
	DebugRegister ("DR2", 32, 2),
	DebugRegister ("DR3", 32, 3),
	DebugRegister ("DR4", 32, 4),
	DebugRegister ("DR5", 32, 5),
	DebugRegister ("DR6", 32, 6),
	DebugRegister ("DR7", 32, 7)
};

Register *DebugRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (DebugRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

MMXRegister MMXRegister:: RegisterTable[] =
{
	MMXRegister ("MM",  64, 0),
	MMXRegister ("MM0", 64, 0),
	MMXRegister ("MM1", 64, 1),
	MMXRegister ("MM2", 64, 2),
	MMXRegister ("MM3", 64, 3),
	MMXRegister ("MM4", 64, 4),
	MMXRegister ("MM5", 64, 5),
	MMXRegister ("MM6", 64, 6),
	MMXRegister ("MM7", 64, 7)
};

Register *MMXRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (MMXRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

XMMRegister XMMRegister:: RegisterTable[] =
{
	XMMRegister ("XMM",  128, 0),
	XMMRegister ("XMM0", 128, 0),
	XMMRegister ("XMM1", 128, 1),
	XMMRegister ("XMM2", 128, 2),
	XMMRegister ("XMM3", 128, 3),
	XMMRegister ("XMM4", 128, 4),
	XMMRegister ("XMM5", 128, 5),
	XMMRegister ("XMM6", 128, 6),
	XMMRegister ("XMM7", 128, 7)
};

Register *XMMRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (XMMRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

FPURegister FPURegister:: RegisterTable[] =
{
	FPURegister ("ST0", 80, 0),
	FPURegister ("ST1", 80, 1),
	FPURegister ("ST2", 80, 2),
	FPURegister ("ST3", 80, 3),
	FPURegister ("ST4", 80, 4),
	FPURegister ("ST5", 80, 5),
	FPURegister ("ST6", 80, 6),
	FPURegister ("ST7", 80, 7)
};

Register *FPURegister::Read (const string &str, InputFile &inp) throw ()
{
	static FPURegister ST ("ST", 80, 7);

	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (FPURegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	if (str == "ST")
	{
		string *t, *u, *v;
		char c, d, x;

		// Checks if user has said ST(n)
		t = inp.GetString();
		c = inp.GetNextChar();
		if (*t != "(") goto abort1;

		// Checks if there is a digit between parentheses
		u = inp.GetString();
		d = inp.GetNextChar();
		if (u->size() != 1) goto abort2;

		// Must be between ST(0) and ST(7)
		x = (*u)[0];
		if ((x < '0') || (x > '7')) goto abort2;

		// Checks if user has closed parentheses
		v = inp.GetString();
		if (*v != ")") goto abort3;

		// Returns STn instead of ST(n)
		delete t;
		delete u;
		delete v;
		return RegisterTable + x - '0';

		// If not a valid register, unget everything and return ST
		abort3:	inp.UngetString (v);
		abort2:	inp.UngetString (u, d);
		abort1:	inp.UngetString (t, c);
					return &ST;
	}

	return 0;
}

Register *Register::Read (const string &str, InputFile &inp) throw ()
{
	Register *answer;
	string UppercaseName(str);

	// Gets a copy of the original string in uppercase
	for (string::iterator i = UppercaseName.begin(); i < UppercaseName.end(); i++)
		if ((*i <= 'z') && (*i >= 'a')) *i -= 32;

	answer = SegmentRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = GPRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = ControlRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = TestRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = DebugRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = MMXRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = XMMRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = FPURegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	return 0;
}
