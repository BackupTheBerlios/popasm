/***************************************************************************
                          register.cpp  -  description
                             --------------------
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

Argument *Register::MakeArgument (const Expression &e) throw (InvalidArgument, exception)
{
	// Only scalars can be registers
	if (e.GetConstData().GetCurrentType() != Type::SCALAR)
		return 0;

	const SimpleExpression *pref;
	const Symbol *sym = e.GetSymbol (pref);

	if (sym == 0)
		return 0;

	// Check if the only symbol is a register
	const Register *reg = dynamic_cast<const Register *> (sym->GetData());
	if (reg == 0)
		return 0;

	// Registers cannot have prefixes
	if (pref != 0)
		throw 0;

	// Cannot have NEAR, or FAR registers
	if (e.GetConstData().GetDistanceType() != UNDEFINED)
		throw 0;

	return new Argument (reg, false);
}

SegmentRegister SegmentRegister::RegisterTable[] =
{
	SegmentRegister ("ES", 16, 0, 0x26),
	SegmentRegister ("CS", 16, 1, 0x2E),
	SegmentRegister ("SS", 16, 2, 0x36),
	SegmentRegister ("DS", 16, 3, 0x3E),
	SegmentRegister ("FS", 16, 4, 0x64),
	SegmentRegister ("GS", 16, 5, 0x65)
};

Register *SegmentRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (SegmentRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

Register *GPRegister::Read (const string &str, InputFile &inp) throw ()
{
	Register *answer;

	answer = GPRegister8Bits::Read (str, inp);
	if (answer != 0) return answer;

	answer = GPRegister16Bits::Read (str, inp);
	if (answer != 0) return answer;

	return GPRegister32Bits::Read (str, inp);
}

const Register *GPRegister::GetRegister (Byte code, int size) throw ()
{
	switch (size)
	{
		case 8:
			return GPRegister8Bits::GetRegister (code, size);

		case 16:
			return GPRegister16Bits::GetRegister (code, size);

		case 32:
		default:
			break;
	}

	return GPRegister32Bits::GetRegister (code, size);
}

GPRegister8Bits GPRegister8Bits::RegisterTable[] =
{
	GPRegister8Bits ("AL", 8, 0),
	GPRegister8Bits ("CL", 8, 1),
	GPRegister8Bits ("DL", 8, 2),
	GPRegister8Bits ("BL", 8, 3),
	GPRegister8Bits ("AH", 8, 4),
	GPRegister8Bits ("CH", 8, 5),
	GPRegister8Bits ("DH", 8, 6),
	GPRegister8Bits ("BH", 8, 7)
};

Register *GPRegister8Bits::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (GPRegister8Bits); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

GPRegister16Bits GPRegister16Bits::RegisterTable[] =
{
	GPRegister16Bits ("AX", 16, 0),
	GPRegister16Bits ("CX", 16, 1),
	GPRegister16Bits ("DX", 16, 2),
	GPRegister16Bits ("SP", 16, 4)
};

Register *GPRegister16Bits::Read (const string &str, InputFile &inp) throw ()
{
	Register *answer;

	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (GPRegister16Bits); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	answer = BaseRegister::Read (str, inp);
	if (answer != 0) return 0;

	return IndexRegister::Read (str, inp);
}

GPRegister32Bits GPRegister32Bits::RegisterTable[] =
{
	GPRegister32Bits ("EAX", 32, 0),
	GPRegister32Bits ("ECX", 32, 1),
	GPRegister32Bits ("EDX", 32, 2),
	GPRegister32Bits ("EBX", 32, 3),
	GPRegister32Bits ("ESP", 32, 4),
	GPRegister32Bits ("EBP", 32, 5),
	GPRegister32Bits ("ESI", 32, 6),
	GPRegister32Bits ("EDI", 32, 7)
};

Register *GPRegister32Bits::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (GPRegister32Bits); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

BaseRegister BaseRegister::RegisterTable[] =
{
	BaseRegister ("BX", 16, 3, 7, 0),
	BaseRegister ("BP", 16, 5, 6, 2)
};

Register *BaseRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (BaseRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

IndexRegister IndexRegister::RegisterTable[] =
{
	IndexRegister ("SI", 16, 6, 4, 0),
	IndexRegister ("DI", 16, 7, 5, 1)
};

Register *IndexRegister::Read (const string &str, InputFile &inp) throw ()
{
	for (unsigned int i = 0; i < sizeof (RegisterTable) / sizeof (IndexRegister); i++)
		if (RegisterTable[i].GetName() == str) return RegisterTable + i;

	return 0;
}

ControlRegister ControlRegister::RegisterTable[] =
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

TestRegister TestRegister::RegisterTable[] =
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

DebugRegister DebugRegister::RegisterTable[] =
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

MMXRegister MMXRegister::RegisterTable[] =
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

XMMRegister XMMRegister::RegisterTable[] =
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

FPURegister FPURegister::RegisterTable[] =
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
	static FPURegister ST ("ST", 80, 0);

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
	UpperCase (UppercaseName);

	answer = SegmentRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = GPRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = BaseRegister::Read (UppercaseName, inp);
	if (answer != 0) return answer;

	answer = IndexRegister::Read (UppercaseName, inp);
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
