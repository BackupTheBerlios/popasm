/***************************************************************************
                          instruct.cpp  -  description
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

#include <iostream>
#include "instruct.h"
#include "immed.h"
#include "memory.h"
#include "full_ptr.h"
#include "defs.h"
#include "parser.h"

Instruction::Instruction (const string &n,
	const Syntax *s1  = 0, const Syntax *s2  = 0, const Syntax *s3  = 0, const Syntax *s4  = 0,
	const Syntax *s5  = 0, const Syntax *s6  = 0, const Syntax *s7  = 0, const Syntax *s8  = 0,
	const Syntax *s9  = 0, const Syntax *s10 = 0, const Syntax *s11 = 0, const Syntax *s12 = 0) throw () : Command (n)
{
	if (s1 != 0)  AddSyntax (s1);  else return;
	if (s2 != 0)  AddSyntax (s2);  else return;
	if (s3 != 0)  AddSyntax (s3);  else return;
	if (s4 != 0)  AddSyntax (s4);  else return;
	if (s5 != 0)  AddSyntax (s5);  else return;
	if (s6 != 0)  AddSyntax (s6);  else return;
	if (s7 != 0)  AddSyntax (s7);  else return;
	if (s8 != 0)  AddSyntax (s8);  else return;
	if (s9 != 0)  AddSyntax (s9);  else return;
	if (s10 != 0) AddSyntax (s10); else return;
	if (s11 != 0) AddSyntax (s11); else return;
	if (s12 != 0) AddSyntax (s12); else return;
}

BasicSymbol *Instruction::Read (const string &str, InputFile &inp)
{
	Instruction s (str);
	Instruction * const *i = InstructionTable.Find (&s);

	return (i == 0) ? 0 : *i;
}

void Instruction::Assemble (const BasicSymbol *sym, vector<Token *>::iterator i, vector<Token *>::iterator j,
	vector<Byte> &Encoding) const
{
	// Converts the tokens into arguments.
	vector<Argument *> Arguments;
	Parser::ParseArguments (i, j, Arguments);

	// Include in the symbol table the creation of the new label
	if (sym != 0)
	{
		cout << "Label definition not implemented yet." << endl;
		return;
	}

	// Checks all syntaxes avaiable
	for (vector<const Syntax *>::const_iterator i = Syntaxes.begin(); i != Syntaxes.end(); i++)
	{
		if ((*i)->Assemble (Arguments, Encoding))
		{
			for (vector<Argument *>::iterator x = Arguments.begin(); x != Arguments.end(); x++)
				delete *x;
			return;
		}
	}

	// None of them fits, so throw exception
	throw InvalidSyntax(this, Arguments);
}

HashTable <Instruction *, HashFunctor, PointerComparator<Instruction> > Instruction::InstructionTable = HashTable <Instruction *, HashFunctor, PointerComparator<Instruction> > ();

// Memory functors
typedef Memory::IdFunctor<UNDEFINED | BYTE | WORD | DWORD, UNDEFINED,        (UNDEFINED | INTEGER)> GPMem;
typedef Memory::IdFunctor<BYTE | WORD | DWORD, UNDEFINED,                    (UNDEFINED | INTEGER)> SGPMem;
typedef Memory::IdFunctor<UNDEFINED | BYTE,                UNDEFINED,        (UNDEFINED | INTEGER)> Mem8;
typedef Memory::IdFunctor<BYTE,                            UNDEFINED,        (UNDEFINED | INTEGER)> SMem8;
typedef Memory::IdFunctor<UNDEFINED | WORD,                UNDEFINED,        (UNDEFINED | INTEGER)> Mem16;
typedef Memory::IdFunctor<WORD,                            UNDEFINED,        (UNDEFINED | INTEGER)> SMem16;
typedef Memory::IdFunctor<UNDEFINED | DWORD,               UNDEFINED,        (UNDEFINED | INTEGER)> Mem32;
typedef Memory::IdFunctor<UNDEFINED | DWORD,               UNDEFINED,        (UNDEFINED | FLOAT)>   Mem32f;
typedef Memory::IdFunctor<DWORD,                           UNDEFINED,        (UNDEFINED | INTEGER)> SMem32;
typedef Memory::IdFunctor<DWORD,                           UNDEFINED,        (UNDEFINED | FLOAT)>   SMem32f;
typedef Memory::IdFunctor<UNDEFINED | PWORD,               UNDEFINED,        (UNDEFINED | INTEGER)> Mem48;
typedef Memory::IdFunctor<PWORD,                           UNDEFINED,        (UNDEFINED | INTEGER)> SMem48;
typedef Memory::IdFunctor<UNDEFINED | QWORD,               UNDEFINED,        (UNDEFINED | INTEGER)> Mem64;
typedef Memory::IdFunctor<QWORD,                           UNDEFINED,        (UNDEFINED | INTEGER)> SMem64;
typedef Memory::IdFunctor<UNDEFINED | QWORD,               UNDEFINED,        (UNDEFINED | FLOAT)>   Mem64f;
typedef Memory::IdFunctor<QWORD,                           UNDEFINED,        (UNDEFINED | FLOAT)>   SMem64f;
typedef Memory::IdFunctor<UNDEFINED | TBYTE,               UNDEFINED,        (UNDEFINED | BCD)>     Mem80BCD;
typedef Memory::IdFunctor<TBYTE,                           UNDEFINED,        (UNDEFINED | FLOAT)>   SMem80f;
typedef Memory::IdFunctor<UNDEFINED | TBYTE,               UNDEFINED,        (UNDEFINED | FLOAT)>   Mem80f;
typedef Memory::IdFunctor<UNDEFINED | OWORD,               UNDEFINED,        (UNDEFINED | INTEGER)> Mem128;
typedef Memory::IdFunctor<UNDEFINED | QWORD,               UNDEFINED,        (UNDEFINED | INTEGER)> MMXMem;
typedef Memory::IdFunctor<UNDEFINED | OWORD,               UNDEFINED,        (UNDEFINED | FLOAT)>   XMMMem;
typedef Memory::IdFunctor<UNDEFINED | WORD | DWORD,        UNDEFINED,        (UNDEFINED | INTEGER)> WordMem;
typedef Memory::IdFunctor<WORD | DWORD,                    UNDEFINED,        (UNDEFINED | INTEGER)> SWordMem;
typedef Memory::IdFunctor<UNDEFINED | WORD | DWORD,        UNDEFINED | NEAR, (UNDEFINED | INTEGER)> NearMem;
typedef Memory::IdFunctor<ANY,                             UNDEFINED,        (UNDEFINED | INTEGER)> AnyMemInt;
typedef Memory::IdFunctor<ANY,                             UNDEFINED,        ANY>                   AnyMem;

// Register functors
typedef Register::IdFunctor <GPRegister,      ANY, ANY>            GPReg;
typedef Register::IdFunctor <GPRegister,      ANY, BYTE>           GPReg8;
typedef Register::IdFunctor <GPRegister,      ANY, WORD>           GPReg16;
typedef Register::IdFunctor <GPRegister,      ANY, DWORD>          GPReg32;
typedef Register::IdFunctor <GPRegister,      ANY, (WORD | DWORD)> WordReg;
typedef Register::IdFunctor <MMXRegister,     ANY, ANY>            MMXReg;
typedef Register::IdFunctor <XMMRegister,     ANY, ANY>            XMMReg;
typedef Register::IdFunctor <FPURegister,     ANY, ANY>            FPUReg;
typedef Register::IdFunctor <SegmentRegister, ANY, ANY>            SegReg;
typedef Register::IdFunctor <ControlRegister, ANY, ANY>            ControlReg;
typedef Register::IdFunctor <TestRegister,    ANY, ANY>            TestReg;
typedef Register::IdFunctor <DebugRegister,   ANY, ANY>            DebugReg;

// Specific register functors
typedef Register::IdFunctor<GPRegister,        0, ANY> Accumulator;
typedef Register::IdFunctor<FPURegister,       0, ANY> ST;
typedef Register::IdFunctor<GPRegister8Bits,   0, ANY> AL;
typedef Register::IdFunctor<GPRegister16Bits,  0, ANY> AX;
typedef Register::IdFunctor<GPRegister16Bits,  2, ANY> DX;
typedef Register::IdFunctor<GPRegister8Bits,   1, ANY> CL;

// Segment register functors
typedef Register::IdFunctor<SegmentRegister,   0, ANY> ES;
typedef Register::IdFunctor<SegmentRegister,   1, ANY> CS;
typedef Register::IdFunctor<SegmentRegister,   2, ANY> SS;
typedef Register::IdFunctor<SegmentRegister,   3, ANY> DS;
typedef Register::IdFunctor<SegmentRegister,   4, ANY> FS;
typedef Register::IdFunctor<SegmentRegister,   5, ANY> GS;

// For convenience
template <class A, class B>
class OR : public  BinaryCompose <logical_or<bool>, A, B> {};

void Instruction::SetupInstructionTable () throw ()
{
	static Instruction Instructions[] =
	{
		Instruction ("AAA",
			new ZerarySyntax          (Opcode (0x37),                   Syntax::NOTHING)),

		Instruction ("AAD",
			new UnarySyntax           (Opcode (0xD5),                   Syntax::NOTHING,                                                                         new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax          (Opcode (0xD5, 0x0A),             Syntax::NOTHING)),

		Instruction ("AAM",
			new UnarySyntax           (Opcode (0xD4),                   Syntax::NOTHING,                                                                         new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax          (Opcode (0xD4, 0x0A),             Syntax::NOTHING)),

		Instruction ("AAS",
			new ZerarySyntax          (Opcode (0x3F),                   Syntax::NOTHING)),

		Instruction ("ADC",
			new BinarySyntax          (Opcode (0x10),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR <GPMem, GPReg>()),
			new BinarySyntax          (Opcode (0x83, 0x02),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x14),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x02),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPMem, GPReg>(),       new Immediate::IdFunctor())),

		Instruction ("ADD",
			new BinarySyntax          (Opcode (0x00),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR <GPMem, GPReg>()),
			new BinarySyntax          (Opcode (0x83, 0x00),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x04),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x00),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPMem, GPReg>(),       new Immediate::IdFunctor())),

		Instruction ("ADDPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x58),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>())),

		Instruction ("ADDPS",
			new BinarySyntax          (Opcode (0x0F, 0x58),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>())),

		Instruction ("ADDSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x58),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<Mem64f, XMMReg>())),

		Instruction ("ADDSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x58),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<Mem32f, XMMReg>())),

		Instruction ("AND",
			new BinarySyntax          (Opcode (0x20),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR <GPMem, GPReg>()),
			new BinarySyntax          (Opcode (0x83, 0x04),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x24),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x04),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPMem, GPReg>(),       new Immediate::IdFunctor())),

		Instruction ("ANDPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x54),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>())),

		Instruction ("ANDPS",
			new BinarySyntax          (Opcode (0x0F, 0x54),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>())),

		Instruction ("ANDNPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x55),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>())),

		Instruction ("ANDNPS",
			new BinarySyntax          (Opcode (0x0F, 0x55),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>())),

		Instruction ("ARPL",
			new BinarySyntax          (Opcode (0x63),                   Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<Mem16, GPReg16>(),     new GPReg16())),

		Instruction ("BOUND",
			new BinarySyntax          (Opcode (0x62),                   Syntax::FIRST_ARGUMENT,  false, Argument::HALF,         0, BinarySyntax::PRESENT,        new WordReg(),                new Memory::IdFunctor<UNDEFINED | DWORD | QWORD, UNDEFINED, (UNDEFINED | INTEGER)>())),

		Instruction ("BSF",
			new BinarySyntax          (Opcode (0x0F, 0xBC),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("BSR",
			new BinarySyntax          (Opcode (0x0F, 0xBD),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("BSWAP",
			new AdditiveUnarySyntax   (Opcode (0x0F, 0xC8),             Syntax::FIRST_ARGUMENT,                                                                  new GPReg32())),

		Instruction ("BT",
			new BinarySyntax          (Opcode (0x0F, 0xA3),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<WordMem, WordReg>(),   new WordReg()),
			new BinarySyntax          (Opcode (0x0F, 0xBA, 0x04),       Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   0, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::UNSIGNED>())),

		Instruction ("BTC",
			new BinarySyntax          (Opcode (0x0F, 0xBB),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<WordMem, WordReg>(),   new WordReg()),
			new BinarySyntax          (Opcode (0x0F, 0xBA, 0x07),       Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   0, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::UNSIGNED>())),

		Instruction ("BTR",
			new BinarySyntax          (Opcode (0x0F, 0xB3),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<WordMem, WordReg>(),   new WordReg()),
			new BinarySyntax          (Opcode (0x0F, 0xBA, 0x06),       Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   0, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::UNSIGNED>())),

		Instruction ("BTS",
			new BinarySyntax          (Opcode (0x0F, 0xAB),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<WordMem, WordReg>(),   new WordReg()),
			new BinarySyntax          (Opcode (0x0F, 0xBA, 0x05),       Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   0, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::UNSIGNED>())),

		Instruction ("CALL",
			new UnarySyntax           (Opcode (0xFF, 0x03),             Syntax::FULL_POINTER,                                                                    new Memory::FarMemory()),
			new UnarySyntax           (Opcode (0xFF, 0x02),             Syntax::FIRST_ARGUMENT,                                                                  new OR<WordReg, NearMem>()),
			new RelativeUnarySyntax   (Opcode (0xE8),                   Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR)),
			new UnarySyntax           (Opcode (0x9A),                   Syntax::FIRST_ARGUMENT,                                                                  new FullPointer::IdFunctor())),

		Instruction ("CBW",
			new ZerarySyntax          (Opcode (0x98),                   Syntax::MODE_16BITS)),

		Instruction ("CDQ",
			new ZerarySyntax          (Opcode (0x99),                   Syntax::MODE_32BITS)),

		Instruction ("CLC",
			new ZerarySyntax          (Opcode (0xF8),                   Syntax::NOTHING)),

		Instruction ("CLD",
			new ZerarySyntax          (Opcode (0xFC),                   Syntax::NOTHING)),

		Instruction ("CLFLUSH",
			new UnarySyntax           (Opcode (0x0F, 0xAE, 0x07),       Syntax::NOTHING,                                                                         new Mem8())),

		Instruction ("CLI",
			new ZerarySyntax          (Opcode (0xFA),                   Syntax::NOTHING)),

		Instruction ("CLTS",
			new ZerarySyntax          (Opcode (0x0F, 0X06),             Syntax::NOTHING)),

		Instruction ("CMC",
			new ZerarySyntax          (Opcode (0xF5),                   Syntax::NOTHING)),

		Instruction ("CMOVA",
			new BinarySyntax          (Opcode (0x0F, 0x47),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVAE",
			new BinarySyntax          (Opcode (0x0F, 0x43),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVB",
			new BinarySyntax          (Opcode (0x0F, 0x42),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVBE",
			new BinarySyntax          (Opcode (0x0F, 0x46),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVC",
			new BinarySyntax          (Opcode (0x0F, 0x42),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVE",
			new BinarySyntax          (Opcode (0x0F, 0x44),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVG",
			new BinarySyntax          (Opcode (0x0F, 0x4F),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVGE",
			new BinarySyntax          (Opcode (0x0F, 0x4D),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVL",
			new BinarySyntax          (Opcode (0x0F, 0x4C),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVLE",
			new BinarySyntax          (Opcode (0x0F, 0x4E),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNA",
			new BinarySyntax          (Opcode (0x0F, 0x46),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNAE",
			new BinarySyntax          (Opcode (0x0F, 0x42),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNB",
			new BinarySyntax          (Opcode (0x0F, 0x43),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNBE",
			new BinarySyntax          (Opcode (0x0F, 0x47),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNC",
			new BinarySyntax          (Opcode (0x0F, 0x43),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNE",
			new BinarySyntax          (Opcode (0x0F, 0x45),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNG",
			new BinarySyntax          (Opcode (0x0F, 0x4E),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNGE",
			new BinarySyntax          (Opcode (0x0F, 0x4C),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNL",
			new BinarySyntax          (Opcode (0x0F, 0x4D),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNLE",
			new BinarySyntax          (Opcode (0x0F, 0x4F),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNO",
			new BinarySyntax          (Opcode (0x0F, 0x41),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNP",
			new BinarySyntax          (Opcode (0x0F, 0x4B),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNS",
			new BinarySyntax          (Opcode (0x0F, 0x49),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVNZ",
			new BinarySyntax          (Opcode (0x0F, 0x45),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVO",
			new BinarySyntax          (Opcode (0x0F, 0x40),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVP",
			new BinarySyntax          (Opcode (0x0F, 0x4A),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVPE",
			new BinarySyntax          (Opcode (0x0F, 0x4A),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVPO",
			new BinarySyntax          (Opcode (0x0F, 0x4B),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVS",
			new BinarySyntax          (Opcode (0x0F, 0x48),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMOVZ",
			new BinarySyntax          (Opcode (0x0F, 0x44),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),                new OR<WordMem, WordReg>())),

		Instruction ("CMP",
			new BinarySyntax          (Opcode (0x38),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR<GPMem, GPReg>()),
			new BinarySyntax          (Opcode (0x83, 0x07),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<SWordMem, WordReg>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x3C),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x07),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPReg, GPMem> (),      new Immediate::IdFunctor())),

		Instruction ("CMPPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMMem, XMMReg>(),           new ImmedRange<8, 0, 7>())),

		Instruction ("CMPEQPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 0)),

		Instruction ("CMPLTPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 1)),

		Instruction ("CMPLEPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 2)),

		Instruction ("CMPUNORDPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 3)),

		Instruction ("CMPNEQPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 4)),

		Instruction ("CMPNLTPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 5)),

		Instruction ("CMPNLEPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 6)),

		Instruction ("CMPORDPD",
			new SuffixedBinarySyntax  (Opcode (0x66, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                 new OR<XMMReg, XMMMem>(), 7)),

		Instruction ("CMPPS",
			new BinarySyntax          (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(),           new ImmedRange<8, 0, 7>())),

		Instruction ("CMPEQPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 0)),

		Instruction ("CMPLTPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 1)),

		Instruction ("CMPLEPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 2)),

		Instruction ("CMPUNORDPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 3)),

		Instruction ("CMPNEQPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 4)),

		Instruction ("CMPNLTPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 5)),

		Instruction ("CMPNLEPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 6)),

		Instruction ("CMPORDPS",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0xC2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMReg(),                new OR<XMMReg, XMMMem>(), 7)),

		Instruction ("CMPS",
			new StringSyntax          (Opcode (0xA6),                   Syntax::FIRST_ARGUMENT,         Argument::EQUAL,                                         new Memory::IdFunctor <UNDEFINED | BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(),     new Memory::IdFunctor <UNDEFINED | BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(), 0),
			new StringSyntax          (Opcode (0xA7),                   Syntax::FIRST_ARGUMENT,         Argument::EQUAL,                                         new Memory::IdFunctor <UNDEFINED | WORD, UNDEFINED, (UNDEFINED | INTEGER)>(),     new Memory::IdFunctor <UNDEFINED | WORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0),
			new StringSyntax          (Opcode (0xA7),                   Syntax::FIRST_ARGUMENT,         Argument::EQUAL,                                         new Memory::IdFunctor <UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(),    new Memory::IdFunctor <UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0)),

		Instruction ("CMPSB",
			new ZerarySyntax          (Opcode (0xA6),                   Syntax::NOTHING)),

		Instruction ("CMPSW",
			new ZerarySyntax          (Opcode (0xA7),                   Syntax::MODE_16BITS)),

		Instruction ("CMPSD",
			new ZerarySyntax          (Opcode (0xA7),                   Syntax::MODE_32BITS),
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>(), new ImmedRange<8, 0, 7>())),

		Instruction ("CMPEQSD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 0)),

		Instruction ("CMPLTSD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 1)),

		Instruction ("CMPLESD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 2)),

		Instruction ("CMPUNORDSD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 3)),

		Instruction ("CMPNEQSD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 4)),

		Instruction ("CMPNLTSD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 5)),

		Instruction ("CMPNLESD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 6)),

		Instruction ("CMPORDSD",
			new SuffixedBinarySyntax  (Opcode (0xF2, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>(), 7)),

		Instruction ("CMPSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), new ImmedRange<8, 0, 7>())),

		Instruction ("CMPEQSS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 0)),

		Instruction ("CMPLTSS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 1)),

		Instruction ("CMPLESS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 2)),

		Instruction ("CMPUNORDSS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 3)),

		Instruction ("CMPNEQSS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 4)),

		Instruction ("CMPNLTSS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 5)),

		Instruction ("CMPNLESS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 6)),

		Instruction ("CMPORDSS",
			new SuffixedBinarySyntax  (Opcode (0xF3, 0x0F, 0xC2),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >(), 7)),

		Instruction ("CMPXCHG",
			new BinarySyntax          (Opcode (0x0F, 0xB0),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PRESENT,        new OR<GPReg, GPMem>(),   new GPReg())),

		Instruction ("CMPXCHG8B",
			new UnarySyntax           (Opcode (0x0F, 0xC7, 0x01),       Syntax::NOTHING,                                                                         new Memory::IdFunctor<UNDEFINED | QWORD, UNDEFINED, (UNDEFINED | INTEGER)>())),

		Instruction ("COMISD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x2F),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("COMISS",
			new BinarySyntax          (Opcode (0x0F, 0x2F),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem32f>())),

		Instruction ("CPUID",
			new ZerarySyntax          (Opcode (0x0F, 0xA2),             Syntax::NOTHING)),

		Instruction ("CVTDQ2PD",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0xE6),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64>())),

		Instruction ("CVTDQ2PS",
			new BinarySyntax          (Opcode (0x0F, 0x5B),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem128>())),

		Instruction ("CVTPD2DQ",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0xE6),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("CVTPD2PI",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x2D),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem128>())),

		Instruction ("CVTPD2PS",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x5A),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("CVTPI2PD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x2A),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<MMXReg, MMXMem>())),

		Instruction ("CVTPI2PS",
			new BinarySyntax          (Opcode (0x0F, 0x2A),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<MMXReg, MMXMem>())),

		Instruction ("CVTPS2DQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x5B),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("CVTPS2PD",
			new BinarySyntax          (Opcode (0x0F, 0x5A),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("CVTPS2PI",
			new BinarySyntax          (Opcode (0x0F, 0x2D),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("CVTSD2SI",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x2D),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new GPReg32(), new OR<XMMReg, Mem64f>())),

		Instruction ("CVTSD2SS",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x5A),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("CVTSI2SD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x2A),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<GPReg32, Mem32> ())),

		Instruction ("CVTSI2SS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x2A),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<GPReg32, Mem32> ())),

		Instruction ("CVTSS2SD",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x5A),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("CVTSS2SI",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x2D),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new GPReg32(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("CVTTPD2PI",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x2C),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("CVTTPD2DQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE6),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("CVTTPS2DQ",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x5B),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("CVTTPS2PI",
			new BinarySyntax          (Opcode (0x0F, 0x2C),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("CVTTSD2SI",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x2C),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new GPReg32(), new OR<XMMReg, Mem64f>())),

		Instruction ("CVTTSS2SI",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x2C),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new GPReg32(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("CWD",
			new ZerarySyntax          (Opcode (0x99),                   Syntax::MODE_16BITS)),

		Instruction ("CWDE",
			new ZerarySyntax          (Opcode (0x98),                   Syntax::MODE_32BITS)),

		Instruction ("DAA",
			new ZerarySyntax          (Opcode (0x27),                   Syntax::NOTHING)),

		Instruction ("DAS",
			new ZerarySyntax          (Opcode (0x2F),                   Syntax::NOTHING)),

		Instruction ("DEC",
			new UnarySyntax           (Opcode (0xFE, 0x01),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1),
			new AdditiveUnarySyntax   (Opcode (0x48),                   Syntax::FIRST_ARGUMENT,                                                                  new WordReg())),

		Instruction ("DIV",
			new UnarySyntax           (Opcode (0xF6, 0x06),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1)),

		Instruction ("DIVPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x5E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("DIVPS",
			new BinarySyntax          (Opcode (0x0F, 0x5E),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("DIVSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x5E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("DIVSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x5E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("EMMS",
			new ZerarySyntax          (Opcode (0x0F, 0x77),             Syntax::NOTHING)),

		Instruction ("ENTER",
			new BinarySyntax          (Opcode (0xC8),                   Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::ABSENT,         new Immed<16, Number::UNSIGNED>(), new ImmedRange<8, 0, 31>())),

		Instruction ("F2XM1",
			new ZerarySyntax          (Opcode (0xD9, 0xF0),             Syntax::NOTHING)),

		Instruction ("FABS",
			new ZerarySyntax          (Opcode (0xD9, 0xE1),             Syntax::NOTHING)),

		Instruction ("FADD",
			new UnarySyntax           (Opcode (0xD8, 0x00),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x00),             Syntax::NOTHING,                                                                         new SMem64f()),
			new FPUBinarySyntax       (Opcode (0xD8, 0xC0),                                                                                                      new ST(),                     new FPUReg()),
			new FPUBinarySyntax       (Opcode (0xDC, 0xC0),                                                                                                      new FPUReg(), new ST())),

		Instruction ("FADDP",
			new FPUBinarySyntax       (Opcode (0xDE, 0xC0),                                                                                                      new FPUReg(), new ST()),
			new ZerarySyntax          (Opcode (0xDE, 0xC1),             Syntax::NOTHING)),

		Instruction ("FIADD",
			new UnarySyntax           (Opcode (0xD8, 0x00),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xD8, 0x00),             Syntax::NOTHING,                                                                         new SMem16())),

		Instruction ("FBLD",
			new UnarySyntax           (Opcode (0xDF, 0x04),             Syntax::NOTHING,                                                                         new Mem80BCD())),

		Instruction ("FBSTP",
			new UnarySyntax           (Opcode (0xDF, 0x06),             Syntax::NOTHING,                                                                         new Mem80BCD())),

		Instruction ("FCHS",
			new ZerarySyntax          (Opcode (0xD9, 0xE0),             Syntax::NOTHING)),

		Instruction ("FCLEX",
			new ZerarySyntax          (Opcode (0x9B, 0xDB, 0xE2),       Syntax::NOTHING)),

		Instruction ("FNCLEX",
			new ZerarySyntax          (Opcode (0xDB, 0xE2),             Syntax::NOTHING)),

		Instruction ("FCMOVB",
			new FPUBinarySyntax       (Opcode (0xDA, 0xC0),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVE",
			new FPUBinarySyntax       (Opcode (0xDA, 0xC8),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVBE",
			new FPUBinarySyntax       (Opcode (0xDA, 0xD0),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVU",
			new FPUBinarySyntax       (Opcode (0xDA, 0xD8),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVNB",
			new FPUBinarySyntax       (Opcode (0xDB, 0xC0),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVNE",
			new FPUBinarySyntax       (Opcode (0xDB, 0xC8),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVNBE",
			new FPUBinarySyntax       (Opcode (0xDB, 0xD0),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCMOVNU",
			new FPUBinarySyntax       (Opcode (0xDB, 0xD8),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCOM",
			new UnarySyntax           (Opcode (0xD8, 0x02),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x02),             Syntax::NOTHING,                                                                         new SMem64f()),
			new AdditiveUnarySyntax   (Opcode (0xD8, 0xD0),             Syntax::NOTHING,                                                                         new FPUReg()),
			new ZerarySyntax          (Opcode (0xD8, 0xD1),             Syntax::NOTHING)),

		Instruction ("FCOMP",
			new UnarySyntax           (Opcode (0xD8, 0x03),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x03),             Syntax::NOTHING,                                                                         new SMem64f()),
			new AdditiveUnarySyntax   (Opcode (0xD8, 0xD8),             Syntax::NOTHING,                                                                         new FPUReg()),
			new ZerarySyntax          (Opcode (0xD8, 0xD9),             Syntax::NOTHING)),

		Instruction ("FCOMPP",
			new ZerarySyntax          (Opcode (0xDE, 0xD9),             Syntax::NOTHING)),

		Instruction ("FCOMI",
			new FPUBinarySyntax       (Opcode (0xDB, 0xF0),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCOMIP",
			new FPUBinarySyntax       (Opcode (0xDF, 0xF0),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FUCOMI",
			new FPUBinarySyntax       (Opcode (0xDB, 0xE8),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FUCOMIP",
			new FPUBinarySyntax       (Opcode (0xDF, 0xE8),                                                                                                      new ST(),                     new FPUReg())),

		Instruction ("FCOS",
			new ZerarySyntax          (Opcode (0xD9, 0xFF),             Syntax::NOTHING)),

		Instruction ("FDECSTP",
			new ZerarySyntax          (Opcode (0xD9, 0xF6),             Syntax::NOTHING)),

		Instruction ("FDIV",
			new UnarySyntax           (Opcode (0xD8, 0x06),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x06),             Syntax::NOTHING,                                                                         new SMem64f()),
			new FPUBinarySyntax       (Opcode (0xD8, 0xF0),                                                                                                      new ST(),                     new FPUReg()),
			new FPUBinarySyntax       (Opcode (0xDC, 0xF8),                                                                                                      new FPUReg(),                 new ST())),

		Instruction ("FDIVP",
			new FPUBinarySyntax       (Opcode (0xDE, 0xF8),                                                                                                      new FPUReg(),                 new ST()),
			new ZerarySyntax          (Opcode (0xDE, 0xF9),             Syntax::NOTHING)),

		Instruction ("FIDIV",
			new UnarySyntax           (Opcode (0xDA, 0x06),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDE, 0x06),             Syntax::NOTHING,                                                                         new SMem16())),

		Instruction ("FDIVR",
			new UnarySyntax           (Opcode (0xD8, 0x07),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x07),             Syntax::NOTHING,                                                                         new SMem64f()),
			new FPUBinarySyntax       (Opcode (0xD8, 0xF8),                                                                                                      new ST(),                     new FPUReg()),
			new FPUBinarySyntax       (Opcode (0xDC, 0xF0),                                                                                                      new FPUReg(), new ST())),

		Instruction ("FDIVRP",
			new FPUBinarySyntax       (Opcode (0xDE, 0xF0),                                                                                                      new FPUReg(), new ST()),
			new ZerarySyntax          (Opcode (0xDE, 0xF1),             Syntax::NOTHING)),

		Instruction ("FIDIVR",
			new UnarySyntax           (Opcode (0xDA, 0x07),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDE, 0x07),             Syntax::NOTHING,                                                                         new SMem16())),

		Instruction ("FEMMS",
			new ZerarySyntax          (Opcode (0x0F, 0x0E),             Syntax::NOTHING)),

		Instruction ("FFREE",
			new AdditiveUnarySyntax   (Opcode (0xDD, 0xC0),             Syntax::NOTHING,                                                                         new FPUReg())),

		Instruction ("FICOM",
			new UnarySyntax           (Opcode (0xDE, 0x02),             Syntax::NOTHING,                                                                         new SMem16()),
			new UnarySyntax           (Opcode (0xDA, 0x02),             Syntax::NOTHING,                                                                         new SMem32())),

		Instruction ("FICOMP",
			new UnarySyntax           (Opcode (0xDE, 0x03),             Syntax::NOTHING,                                                                         new SMem16()),
			new UnarySyntax           (Opcode (0xDA, 0x03),             Syntax::NOTHING,                                                                         new SMem32())),

		Instruction ("FILD",
			new UnarySyntax           (Opcode (0xDF, 0x00),             Syntax::NOTHING,                                                                         new SMem16()),
			new UnarySyntax           (Opcode (0xDB, 0x00),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDF, 0x05),             Syntax::NOTHING,                                                                         new SMem64())),

		Instruction ("FINCSTP",
			new ZerarySyntax          (Opcode (0xD9, 0xF7),             Syntax::NOTHING)),

		Instruction ("FINIT",
			new ZerarySyntax          (Opcode (0x9B, 0xDB, 0xE3),       Syntax::NOTHING)),

		Instruction ("FNINIT",
			new ZerarySyntax          (Opcode (0xDB, 0xE3),             Syntax::NOTHING)),

		Instruction ("FIST",
			new UnarySyntax           (Opcode (0xDF, 0x02),             Syntax::NOTHING,                                                                         new SMem16()),
			new UnarySyntax           (Opcode (0xDB, 0x02),             Syntax::NOTHING,                                                                         new SMem32())),

		Instruction ("FISTP",
			new UnarySyntax           (Opcode (0xDF, 0x03),             Syntax::NOTHING,                                                                         new SMem16()),
			new UnarySyntax           (Opcode (0xDB, 0x03),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDF, 0x07),             Syntax::NOTHING,                                                                         new SMem64())),

		Instruction ("FLD",
			new UnarySyntax           (Opcode (0xD9, 0x00),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDD, 0x00),             Syntax::NOTHING,                                                                         new SMem64f()),
			new UnarySyntax           (Opcode (0xDB, 0x05),             Syntax::NOTHING,                                                                         new SMem80f()),
			new AdditiveUnarySyntax   (Opcode (0xD9, 0xC0),             Syntax::NOTHING,                                                                         new FPUReg())),

		Instruction ("FLD1",
			new ZerarySyntax          (Opcode (0xD9, 0xE8),             Syntax::NOTHING)),

		Instruction ("FLDL2T",
			new ZerarySyntax          (Opcode (0xD9, 0xE9),             Syntax::NOTHING)),

		Instruction ("FLDL2E",
			new ZerarySyntax          (Opcode (0xD9, 0xEA),             Syntax::NOTHING)),

		Instruction ("FLDPI",
			new ZerarySyntax          (Opcode (0xD9, 0xEB),             Syntax::NOTHING)),

		Instruction ("FLDLG2",
			new ZerarySyntax          (Opcode (0xD9, 0xEC),             Syntax::NOTHING)),

		Instruction ("FLDLN2",
			new ZerarySyntax          (Opcode (0xD9, 0xED),             Syntax::NOTHING)),

		Instruction ("FLDZ",
			new ZerarySyntax          (Opcode (0xD9, 0xEE),             Syntax::NOTHING)),

		Instruction ("FLDCW",
			new UnarySyntax           (Opcode (0xD9, 0x05),             Syntax::NOTHING,                                                                         new Mem16())),

		Instruction ("FLDENV",
			new UnarySyntax           (Opcode (0xD9, 0x04),             Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FMUL",
			new UnarySyntax           (Opcode (0xD8, 0x01),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x01),             Syntax::NOTHING,                                                                         new SMem64f()),
			new FPUBinarySyntax       (Opcode (0xD8, 0xC8),                                                                                                      new ST(),                     new FPUReg()),
			new FPUBinarySyntax       (Opcode (0xDC, 0xC8),                                                                                                      new FPUReg(), new ST())),

		Instruction ("FMULP",
			new FPUBinarySyntax       (Opcode (0xDE, 0xC8),                                                                                                      new FPUReg(), new ST()),
			new ZerarySyntax          (Opcode (0xDE, 0xC9),             Syntax::NOTHING)),

		Instruction ("FIMUL",
			new UnarySyntax           (Opcode (0xDA, 0x01),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDE, 0x01),             Syntax::NOTHING,                                                                         new SMem16())),

		Instruction ("FNOP",
			new ZerarySyntax          (Opcode (0xD9, 0xD0),             Syntax::NOTHING)),

		Instruction ("FPATAN",
			new ZerarySyntax          (Opcode (0xD9, 0xF3),             Syntax::NOTHING)),

		Instruction ("FPREM",
			new ZerarySyntax          (Opcode (0xD9, 0xF8),             Syntax::NOTHING)),

		Instruction ("FPREM1",
			new ZerarySyntax          (Opcode (0xD9, 0xF5),             Syntax::NOTHING)),

		Instruction ("FPTAN",
			new ZerarySyntax          (Opcode (0xD9, 0xF2),             Syntax::NOTHING)),

		Instruction ("FRNDINT",
			new ZerarySyntax          (Opcode (0xD9, 0xFC),             Syntax::NOTHING)),

		Instruction ("FRSTOR",
			new UnarySyntax           (Opcode (0xDD, 0x04),             Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FSAVE",
			new UnarySyntax           (Opcode (0x9B, 0xDD, 0x06),       Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FNSAVE",
			new UnarySyntax           (Opcode (0xDD, 0x06),             Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FSCALE",
			new ZerarySyntax          (Opcode (0xD9, 0xFD),             Syntax::NOTHING)),

		Instruction ("FSIN",
			new ZerarySyntax          (Opcode (0xD9, 0xFE),             Syntax::NOTHING)),

		Instruction ("FSINCOS",
			new ZerarySyntax          (Opcode (0xD9, 0xFB),             Syntax::NOTHING)),

		Instruction ("FSQRT",
			new ZerarySyntax          (Opcode (0xD9, 0xFA),             Syntax::NOTHING)),

		Instruction ("FST",
			new UnarySyntax           (Opcode (0xD9, 0x02),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDD, 0x02),             Syntax::NOTHING,                                                                         new SMem64f()),
			new AdditiveUnarySyntax   (Opcode (0xDD, 0xD0),             Syntax::NOTHING,                                                                         new FPUReg())),

		Instruction ("FSTP",
			new UnarySyntax           (Opcode (0xD9, 0x03),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDD, 0x03),             Syntax::NOTHING,                                                                         new SMem64f()),
			new UnarySyntax           (Opcode (0xDB, 0x07),             Syntax::NOTHING,                                                                         new SMem80f()),
			new AdditiveUnarySyntax   (Opcode (0xDD, 0xD8),             Syntax::NOTHING,                                                                         new FPUReg())),

		Instruction ("FSTCW",
			new UnarySyntax           (Opcode (0x9B, 0xD9, 0x07),       Syntax::NOTHING,                                                                         new Mem16())),

		Instruction ("FNSTCW",
			new UnarySyntax           (Opcode (0xD9, 0x07),             Syntax::NOTHING,                                                                         new Mem16())),

		Instruction ("FSTENV",
			new UnarySyntax           (Opcode (0x9B, 0xD9, 0x06),       Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FNSTENV",
			new UnarySyntax           (Opcode (0xD9, 0x06),             Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FSTSW",
			new UnarySyntax           (Opcode (0x9B, 0xDD, 0x07),       Syntax::NOTHING,                                                                         new Mem16()),
			new ZerarySyntax          (Opcode (0x9B, 0xDF, 0XE0),       Syntax::NOTHING,                                                                         new AX())),

		Instruction ("FNSTSW",
			new UnarySyntax           (Opcode (0xDD, 0x07),             Syntax::NOTHING,                                                                         new Mem16()),
			new ZerarySyntax          (Opcode (0xDF, 0XE0),             Syntax::NOTHING)),

		Instruction ("FSUB",
			new UnarySyntax           (Opcode (0xD8, 0x04),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x04),             Syntax::NOTHING,                                                                         new SMem64f()),
			new FPUBinarySyntax       (Opcode (0xD8, 0xE0),                                                                                                      new ST(),                     new FPUReg()),
			new FPUBinarySyntax       (Opcode (0xDC, 0xE8),                                                                                                      new FPUReg(), new ST())),

		Instruction ("FSUBP",
			new FPUBinarySyntax       (Opcode (0xDE, 0xE8),                                                                                                      new FPUReg(), new ST()),
			new ZerarySyntax          (Opcode (0xDE, 0xE9),             Syntax::NOTHING)),

		Instruction ("FISUB",
			new UnarySyntax           (Opcode (0xDA, 0x04),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDE, 0x04),             Syntax::NOTHING,                                                                         new SMem16())),

		Instruction ("FSUBR",
			new UnarySyntax           (Opcode (0xD8, 0x05),             Syntax::NOTHING,                                                                         new SMem32f()),
			new UnarySyntax           (Opcode (0xDC, 0x05),             Syntax::NOTHING,                                                                         new SMem64f()),
			new FPUBinarySyntax       (Opcode (0xD8, 0xE8),                                                                                                      new ST(),                     new FPUReg()),
			new FPUBinarySyntax       (Opcode (0xDC, 0xE0),                                                                                                      new FPUReg(), new ST())),

		Instruction ("FSUBRP",
			new FPUBinarySyntax       (Opcode (0xDE, 0xE0),                                                                                                      new FPUReg(), new ST()),
			new ZerarySyntax          (Opcode (0xDE, 0xE1),             Syntax::NOTHING)),

		Instruction ("FISUBR",
			new UnarySyntax           (Opcode (0xDA, 0x05),             Syntax::NOTHING,                                                                         new SMem32()),
			new UnarySyntax           (Opcode (0xDE, 0x05),             Syntax::NOTHING,                                                                         new SMem16())),

		Instruction ("FTST",
			new ZerarySyntax          (Opcode (0xD9, 0xE4),             Syntax::NOTHING)),

		Instruction ("FUCOM",
			new AdditiveUnarySyntax   (Opcode (0xDD, 0xE0),             Syntax::NOTHING,                                                                         new FPUReg()),
			new ZerarySyntax          (Opcode (0xDD, 0xE1),             Syntax::NOTHING)),

		Instruction ("FUCOMP",
			new AdditiveUnarySyntax   (Opcode (0xDD, 0xE8),             Syntax::NOTHING,                                                                         new FPUReg()),
			new ZerarySyntax          (Opcode (0xDD, 0xE9),             Syntax::NOTHING)),

		Instruction ("FUCOMPP",
			new ZerarySyntax          (Opcode (0xDA, 0xE9),             Syntax::NOTHING)),

		Instruction ("FWAIT",
			new ZerarySyntax          (Opcode (0x9B),                   Syntax::NOTHING)),

		Instruction ("FXAM",
			new ZerarySyntax          (Opcode (0xD9, 0xE5),             Syntax::NOTHING)),

		Instruction ("FXCH",
			new AdditiveUnarySyntax   (Opcode (0xD9, 0xC8),             Syntax::NOTHING,                                                                         new FPUReg()),
			new ZerarySyntax          (Opcode (0xD9, 0xc9),             Syntax::NOTHING)),

		Instruction ("FXRSTOR",
			new UnarySyntax           (Opcode (0x0F, 0xAE, 0x01),       Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FXSAVE",
			new UnarySyntax           (Opcode (0x0F, 0xAE, 0x00),       Syntax::NOTHING,                                                                         new AnyMemInt())),

		Instruction ("FXTRACT",
			new ZerarySyntax          (Opcode (0xD9, 0xF4),             Syntax::NOTHING)),

		Instruction ("FYL2X",
			new ZerarySyntax          (Opcode (0xD9, 0xF1),             Syntax::NOTHING)),

		Instruction ("FYL2XP1",
			new ZerarySyntax          (Opcode (0xD9, 0xF9),             Syntax::NOTHING)),

		Instruction ("HLT",
			new ZerarySyntax          (Opcode (0xF4),                   Syntax::NOTHING)),

		Instruction ("IDIV",
			new UnarySyntax           (Opcode (0xF6, 0x07),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1)),

		Instruction ("IMUL",
			new UnarySyntax           (Opcode (0xF6, 0x05),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1),
			new BinarySyntax          (Opcode (0x0F, 0xAF),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new WordReg(), new OR<WordReg, WordMem>()),
			new BinarySyntax          (Opcode (0x6B),                   Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         0, BinarySyntax::REPEATED,       new WordReg(), new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x69),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::REPEATED,       new GPReg16(), new Immed<16, Number::SIGNED>()),
			new BinarySyntax          (Opcode (0x69),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::REPEATED,       new GPReg32(), new Immed<32, Number::SIGNED>()),
			new BinarySyntax          (Opcode (0x6B),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new WordReg(), new WordReg(),             new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x69),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new GPReg16(), new OR<GPReg16, Mem16>(),  new Immed<16, Number::SIGNED>()),
			new BinarySyntax          (Opcode (0x69),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new GPReg32(), new OR <GPReg32, Mem32>(), new Immed<32, Number::SIGNED>())),

		Instruction ("IN",
			new BinarySyntax          (Opcode (0xE4),                   Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::ABSENT,         new Accumulator(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax          (Opcode (0xEC),                   Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::ABSENT,         new Accumulator(), new DX())),

		Instruction ("INC",
			new AdditiveUnarySyntax   (Opcode (0x40),                   Syntax::FIRST_ARGUMENT,                                                                  new WordReg()),
			new UnarySyntax           (Opcode (0xFE, 0x00),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1)),

		Instruction ("INS",
			new StringSyntax          (Opcode (0x6C),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(),     new DX(), 2),
			new StringSyntax          (Opcode (0x6D),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <WORD, UNDEFINED, (UNDEFINED | INTEGER)>(),    new DX(), 2),
			new StringSyntax          (Opcode (0x6D),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(),    new DX(), 2)),

		Instruction ("INSB",
			new ZerarySyntax          (Opcode (0x6C),                   Syntax::NOTHING)),

		Instruction ("INSW",
			new ZerarySyntax          (Opcode (0x6D),                   Syntax::MODE_16BITS)),

		Instruction ("INSD",
			new ZerarySyntax          (Opcode (0x6D),                   Syntax::MODE_32BITS)),

		Instruction ("INT",
			new ZerarySyntax          (Opcode (0xCC),                   Syntax::NOTHING,                                                                         new ImmedEqual<UNDEFINED, 3>),
			new UnarySyntax           (Opcode (0xCD),                   Syntax::NOTHING,                                                                         new Immed<8, Number::UNSIGNED>())),

		Instruction ("INTO",
			new ZerarySyntax          (Opcode (0xCE),                   Syntax::NOTHING)),

		Instruction ("INVD",
			new ZerarySyntax          (Opcode (0x0F, 0x08),             Syntax::NOTHING)),

		Instruction ("INVLPG",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x07),       Syntax::NOTHING,                                                                         new Memory::IdFunctor<ANY, ANY, ANY>())),

		Instruction ("IRET",
			new ZerarySyntax          (Opcode (0xCF),                   Syntax::NOTHING)),

		Instruction ("IRETW",
			new ZerarySyntax          (Opcode (0xCF),                   Syntax::MODE_16BITS)),

		Instruction ("IRETD",
			new ZerarySyntax          (Opcode (0xCF),                   Syntax::MODE_32BITS)),

		Instruction ("JA",
			new RelativeUnarySyntax   (Opcode (0x77),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x87),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JAE",
			new RelativeUnarySyntax   (Opcode (0x73),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x83),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JB",
			new RelativeUnarySyntax   (Opcode (0x72),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x82),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JBE",
			new RelativeUnarySyntax   (Opcode (0x76),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x86),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JC",
			new RelativeUnarySyntax   (Opcode (0x72),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x82),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

  		Instruction ("JCXZ",
  			new RelativeUnarySyntax   (Opcode (0xE3),                   Syntax::MODE_16BITS,                                                                     new RelativeArgument (SHORT))),

  		Instruction ("JECXZ",
  			new RelativeUnarySyntax   (Opcode (0xE3),                   Syntax::MODE_32BITS,                                                                     new RelativeArgument (SHORT))),

		Instruction ("JE",
			new RelativeUnarySyntax   (Opcode (0x74),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x84),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JG",
			new RelativeUnarySyntax   (Opcode (0x7F),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8F),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JGE",
			new RelativeUnarySyntax   (Opcode (0x7D),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8D),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JL",
			new RelativeUnarySyntax   (Opcode (0x7C),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8C),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JLE",
			new RelativeUnarySyntax   (Opcode (0x7E),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8E),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNA",
			new RelativeUnarySyntax   (Opcode (0x76),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x86),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNAE",
			new RelativeUnarySyntax   (Opcode (0x72),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x82),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNB",
			new RelativeUnarySyntax   (Opcode (0x73),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x83),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNBE",
			new RelativeUnarySyntax   (Opcode (0x77),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x87),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNC",
			new RelativeUnarySyntax   (Opcode (0x73),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x83),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNE",
			new RelativeUnarySyntax   (Opcode (0x75),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x85),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNG",
			new RelativeUnarySyntax   (Opcode (0x7E),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8E),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNGE",
			new RelativeUnarySyntax   (Opcode (0x7C),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8C),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNL",
			new RelativeUnarySyntax   (Opcode (0x7D),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8D),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNLE",
			new RelativeUnarySyntax   (Opcode (0x7F),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8F),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNO",
			new RelativeUnarySyntax   (Opcode (0x71),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x81),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNP",
			new RelativeUnarySyntax   (Opcode (0x7B),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8B),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNS",
			new RelativeUnarySyntax   (Opcode (0x79),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x89),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JNZ",
			new RelativeUnarySyntax   (Opcode (0x75),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x85),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JO",
			new RelativeUnarySyntax   (Opcode (0x70),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x80),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JP",
			new RelativeUnarySyntax   (Opcode (0x7A),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8A),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JPE",
			new RelativeUnarySyntax   (Opcode (0x7A),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8A),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JPO",
			new RelativeUnarySyntax   (Opcode (0x7B),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x8B),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JS",
			new RelativeUnarySyntax   (Opcode (0x78),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x88),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JZ",
			new RelativeUnarySyntax   (Opcode (0x74),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0x0F, 0x84),             Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR))),

		Instruction ("JMP",
			new UnarySyntax           (Opcode (0xFF, 0x04),             Syntax::FIRST_ARGUMENT,                                                                  new OR <WordReg, NearMem>()),
			new UnarySyntax           (Opcode (0xFF, 0x05),             Syntax::FULL_POINTER,                                                                    new Memory::FarMemory()),
			new RelativeUnarySyntax   (Opcode (0xEB),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT)),
			new RelativeUnarySyntax   (Opcode (0xE9),                   Syntax::FIRST_ARGUMENT,                                                                  new RelativeArgument (NEAR)),
			new UnarySyntax           (Opcode (0xEA),                   Syntax::FIRST_ARGUMENT,                                                                  new FullPointer::IdFunctor())),

		Instruction ("LAHF",
			new ZerarySyntax          (Opcode (0x9F),                   Syntax::NOTHING)),

		Instruction ("LAR",
			new BinarySyntax          (Opcode (0x0F, 0x02),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new OR<WordReg, WordMem>())),

		Instruction ("LDMXCSR",
			new UnarySyntax           (Opcode (0x0F, 0xAE, 0x02),       Syntax::NOTHING,                                                                         new GPReg32())),

		Instruction ("LFENCE",
			new ZerarySyntax          (Opcode (0x0F, 0xAE, 0xE8),       Syntax::NOTHING)),

		Instruction ("LDS",
			new BinarySyntax          (Opcode (0xC5),                   Syntax::FIRST_ARGUMENT,  false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT,        new WordReg(),    new Memory::IdFunctor<UNDEFINED | DWORD | PWORD, UNDEFINED, ANY>())),

		Instruction ("LES",
			new BinarySyntax          (Opcode (0xC4),                   Syntax::FIRST_ARGUMENT,  false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT,        new WordReg(),    new Memory::IdFunctor<UNDEFINED | DWORD | PWORD, UNDEFINED, ANY>())),

		Instruction ("LFS",
			new BinarySyntax          (Opcode (0x0F, 0xB4),             Syntax::FIRST_ARGUMENT,  false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT,        new WordReg(),    new Memory::IdFunctor<UNDEFINED | DWORD | PWORD, UNDEFINED, ANY>())),

		Instruction ("LGS",
			new BinarySyntax          (Opcode (0x0F, 0xB5),             Syntax::FIRST_ARGUMENT,  false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT,        new WordReg(),    new Memory::IdFunctor<UNDEFINED | DWORD | PWORD, UNDEFINED, ANY>())),

		Instruction ("LSS",
			new BinarySyntax          (Opcode (0x0F, 0xB2),             Syntax::FIRST_ARGUMENT,  false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT,        new WordReg(),    new Memory::IdFunctor<UNDEFINED | DWORD | PWORD, UNDEFINED, ANY>())),

		Instruction ("LEA",
			new BinarySyntax          (Opcode (0x8D),                   Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         0, BinarySyntax::PRESENT,        new WordReg(),    new AnyMem())),

		Instruction ("LEAVE",
			new ZerarySyntax          (Opcode (0xC9),                   Syntax::NOTHING)),

		Instruction ("LGDT",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x02),       Syntax::NOTHING,                                                                         new Mem48())),

		Instruction ("LIDT",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x03),       Syntax::NOTHING,                                                                         new Mem48())),

		Instruction ("LLDT",
			new UnarySyntax           (Opcode (0x0F, 0x00, 0x02),       Syntax::NOTHING,                                                                         new OR<GPReg16, Mem16>())),

		Instruction ("LMSW",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x06),       Syntax::NOTHING,                                                                         new OR<GPReg16, Mem16>())),

		Instruction ("LOCK",
			new ZerarySyntax          (Opcode (0xF0),                   Syntax::NOTHING)),

		Instruction ("LODS",
			new StringSyntax          (Opcode (0xAC),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(),  0, 0),
			new StringSyntax          (Opcode (0xAD),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <WORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0, 0),
			new StringSyntax          (Opcode (0xAD),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0, 0)),

		Instruction ("LODSB",
			new ZerarySyntax          (Opcode (0xAC),                   Syntax::NOTHING)),

		Instruction ("LODSW",
			new ZerarySyntax          (Opcode (0xAD),                   Syntax::MODE_16BITS)),

		Instruction ("LODSD",
			new ZerarySyntax          (Opcode (0xAD),                   Syntax::MODE_32BITS)),

		Instruction ("LOOP",
			new RelativeUnarySyntax   (Opcode (0xE2),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT))),

		Instruction ("LOOPE",
			new RelativeUnarySyntax   (Opcode (0xE1),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT))),

		Instruction ("LOOPZ",
			new RelativeUnarySyntax   (Opcode (0xE1),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT))),

		Instruction ("LOOPNE",
			new RelativeUnarySyntax   (Opcode (0xE0),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT))),

		Instruction ("LOOPNZ",
			new RelativeUnarySyntax   (Opcode (0xE0),                   Syntax::NOTHING,                                                                         new RelativeArgument (SHORT))),

		Instruction ("LSL",
			new BinarySyntax          (Opcode (0x0F, 0x03),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new WordReg(),    new OR<WordReg, WordMem>())),

		Instruction ("LTR",
			new UnarySyntax           (Opcode (0x0F, 0x00, 0x03),       Syntax::NOTHING,                                                                         new OR<GPReg16, Mem16>())),

		Instruction ("MASKMOVQ",
			new BinarySyntax          (Opcode (0x0F, 0xF7),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new MMXReg())),

		Instruction ("MASKMOVDQU",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF7),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new XMMReg())),

		Instruction ("MAXPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x5F),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("MAXPS",
			new BinarySyntax          (Opcode (0x0F, 0x5F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("MAXSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x5F),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("MAXSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x5F),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem32f>())),

		Instruction ("MFENCE",
			new ZerarySyntax          (Opcode (0x0F, 0xAE, 0xF0),       Syntax::NOTHING)),

		Instruction ("MINPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x5D),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("MINPS",
			new BinarySyntax          (Opcode (0x0F, 0x5D),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("MINSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x5D),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("MINSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x5D),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("MOV",
			new BinarySyntax          (Opcode (0xA0),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),          new Memory::DirectMemory()),
			new BinarySyntax          (Opcode (0xA2),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Memory::DirectMemory(), new Accumulator()),
			new AdditiveUnarySyntax   (Opcode (0xB0),                   Syntax::FIRST_ARGUMENT,                                                                  new GPReg8(),               new Immed<8, Number::ANY>()),
			new AdditiveUnarySyntax   (Opcode (0xB8),                   Syntax::FIRST_ARGUMENT,                                                                  new GPReg16(),              new Immed<16, Number::ANY>()),
			new AdditiveUnarySyntax   (Opcode (0xB8),                   Syntax::FIRST_ARGUMENT,                                                                  new GPReg32(),              new Immed<32, Number::ANY>()),
			new BinarySyntax          (Opcode (0x88),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                new OR<GPReg, GPMem>()),
			new BinarySyntax          (Opcode (0x8C),                   Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         0, BinarySyntax::PRESENT,        new OR<WordReg, WordMem>(), new SegReg()),
			new BinarySyntax          (Opcode (0x8E),                   Syntax::SECOND_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT,        new SegReg(),               new OR<WordReg, WordMem>()),
			new BinarySyntax          (Opcode (0xC6, 0x00),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,	     1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),    new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x0F, 0x20),             Syntax::NOTHING,         true,  Argument::NONE,         2, BinarySyntax::PRESENT,        new GPReg32(),              new ControlReg()),
			new BinarySyntax          (Opcode (0x0F, 0x21),             Syntax::NOTHING,         true,  Argument::NONE,         2, BinarySyntax::PRESENT,        new GPReg32(),              new DebugReg()),
			new BinarySyntax          (Opcode (0x0F, 0x24),             Syntax::NOTHING,         true,  Argument::NONE,         2, BinarySyntax::PRESENT,        new GPReg32(),              new TestReg())),

		Instruction ("MOVAPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x28),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x29),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(), new XMMReg())),

		Instruction ("MOVAPS",
			new BinarySyntax          (Opcode (0x0F, 0x28),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x29),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(), new XMMReg())),

		Instruction ("MOVD",
			new BinarySyntax          (Opcode (0x0F, 0x6E),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new MMXReg(), new OR <GPReg32, Mem32>()),
			new BinarySyntax          (Opcode (0x0F, 0x7E),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new OR <GPReg32, Mem32>(),               new MMXReg()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x6E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR <GPReg32, Mem32>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x7E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new OR <GPReg32, Mem32>(),               new XMMReg())),

		Instruction ("MOVDQA",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x6F),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x7F),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(), new XMMReg())),

		Instruction ("MOVDQU",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x6F),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x7F),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(), new XMMReg())),

		Instruction ("MOVDQ2Q",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0xD6),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new MMXReg(), new XMMReg())),

		Instruction ("MOVHLPS",
			new BinarySyntax          (Opcode (0x0F, 0x12),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new XMMReg())),

		Instruction ("MOVHPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x16),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new Mem64f()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x17),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new Mem64f(), new XMMReg())),

		Instruction ("MOVHPS",
			new BinarySyntax          (Opcode (0x0F, 0x16),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new Mem64f()),
			new BinarySyntax          (Opcode (0x0F, 0x17),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new Mem64f(), new XMMReg())),

		Instruction ("MOVLHPS",
			new BinarySyntax          (Opcode (0x0F, 0x16),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new XMMReg())),

		Instruction ("MOVLPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x12),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new Mem64f()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x13),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new Mem64f(), new XMMReg())),

		Instruction ("MOVLPS",
			new BinarySyntax          (Opcode (0x0F, 0x12),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new Mem64f()),
			new BinarySyntax          (Opcode (0x0F, 0x13),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new Mem64f(), new XMMReg())),

		Instruction ("MOVMSKPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x50),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new GPReg32(), new XMMReg())),

		Instruction ("MOVMSKPS",
			new BinarySyntax          (Opcode (0x0F, 0x50),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new GPReg32(), new XMMReg())),

		Instruction ("MOVNTDQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE7),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(),     new XMMReg())),

		Instruction ("MOVNTI",
			new BinarySyntax          (Opcode (0x0F, 0xC3),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new Mem32(),    new GPReg32())),

		Instruction ("MOVNTPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x2B),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(),     new XMMReg())),

		Instruction ("MOVNTPS",
			new BinarySyntax          (Opcode (0x0F, 0x2B),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(),     new XMMReg())),

		Instruction ("MOVNTQ",
			new BinarySyntax          (Opcode (0x0F, 0xE7),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXMem(),   new MMXReg())),

		Instruction ("MOVQ",
			new BinarySyntax          (Opcode (0x0F, 0x6F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x7F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<MMXReg, MMXMem>(),              new MMXReg()),
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x7E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD6),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new OR<XMMReg, MMXMem>(),              new XMMReg())),

		Instruction ("MOVQ2DQ",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0xD6),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new MMXReg())),

		Instruction ("MOVS",
			new StringSyntax          (Opcode (0xA4),                   Syntax::FIRST_ARGUMENT,         Argument::EQUAL,                                         new Memory::IdFunctor <UNDEFINED | BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(),     new Memory::IdFunctor <UNDEFINED | BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(), 1),
			new StringSyntax          (Opcode (0xA5),                   Syntax::FIRST_ARGUMENT,         Argument::EQUAL,                                         new Memory::IdFunctor <UNDEFINED | WORD, UNDEFINED, (UNDEFINED | INTEGER)>(),    new Memory::IdFunctor <UNDEFINED | WORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 1),
			new StringSyntax          (Opcode (0xA5),                   Syntax::FIRST_ARGUMENT,         Argument::EQUAL,                                         new Memory::IdFunctor <UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(),    new Memory::IdFunctor <UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 1)),

		Instruction ("MOVSB",
			new ZerarySyntax          (Opcode (0xA4),                   Syntax::NOTHING)),

		Instruction ("MOVSW",
			new ZerarySyntax          (Opcode (0xA5),                   Syntax::MODE_16BITS)),

		Instruction ("MOVSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x10),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, MMXMem>()),
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x11),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new MMXMem(), new XMMReg()),
			new ZerarySyntax          (Opcode (0xA5),                   Syntax::MODE_32BITS)),

		Instruction ("MOVSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x10),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem32f>()),
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x11),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new Mem32f(), new XMMReg())),

		Instruction ("MOVUPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x10),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x11),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(), new XMMReg())),

		Instruction ("MOVUPS",
			new BinarySyntax          (Opcode (0x0F, 0x10),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x11),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMMem(), new XMMReg())),

		Instruction ("MOVSX",
			new BinarySyntax          (Opcode (0x0F, 0xBE),             Syntax::FIRST_ARGUMENT,  false, Argument::GREATER,      0, BinarySyntax::PRESENT,        new WordReg(), new OR <GPReg8, SMem8>()),
			new BinarySyntax          (Opcode (0x0F, 0xBF),             Syntax::FIRST_ARGUMENT,  false, Argument::GREATER,      0, BinarySyntax::PRESENT,        new WordReg(), new OR <WordReg, SWordMem>())),

		Instruction ("MOVZX",
			new BinarySyntax          (Opcode (0x0F, 0xB6),             Syntax::FIRST_ARGUMENT,  false, Argument::GREATER,      0, BinarySyntax::PRESENT,        new WordReg(), new OR <GPReg8, SMem8>()),
			new BinarySyntax          (Opcode (0x0F, 0xB7),             Syntax::FIRST_ARGUMENT,  false, Argument::GREATER,      0, BinarySyntax::PRESENT,        new WordReg(), new OR <WordReg, SWordMem>())),

		Instruction ("MUL",
			new UnarySyntax           (Opcode (0xF6, 0x04),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1)),

		Instruction ("MULPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x59),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("MULPS",
			new BinarySyntax          (Opcode (0x0F, 0x59),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("MULSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x59),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("MULSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x59),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem32f>())),

		Instruction ("NEG",
			new UnarySyntax           (Opcode (0xF6, 0x03),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1)),

		Instruction ("NOT",
			new UnarySyntax           (Opcode (0xF6, 0x02),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(), 1)),

		Instruction ("NOP",
			new ZerarySyntax          (Opcode (0x90),                   Syntax::NOTHING)),

		Instruction ("OR",
			new BinarySyntax          (Opcode (0x08),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),  new OR<GPReg, GPMem>()),
			new BinarySyntax          (Opcode (0x83, 0x01),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<WordReg, SWordMem>(),             new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x0C),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x01),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPReg, GPMem>(),  new Immediate::IdFunctor())),

		Instruction ("ORPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x56),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("ORPS",
			new BinarySyntax          (Opcode (0x0F, 0x56),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("OUT",
			new ZerarySyntax          (Opcode (0xEE),                   Syntax::NOTHING,                                                                         new DX(),         new AL()),
			new ZerarySyntax          (Opcode (0xEF),                   Syntax::SECOND_ARGUMENT,                                                                 new DX(),         new Accumulator()),
			new UnarySyntax           (Opcode (0xE6),                   Syntax::SECOND_ARGUMENT,                                                                 new Immed<8, Number::UNSIGNED>(), new Accumulator(), 1)),

		Instruction ("OUTS",
			new StringSyntax          (Opcode (0x6E),                   Syntax::SECOND_ARGUMENT,        Argument::NONE,                                          new DX(),         new Memory::IdFunctor <BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(), 1),
			new StringSyntax          (Opcode (0x6F),                   Syntax::SECOND_ARGUMENT,        Argument::NONE,                                          new DX(),         new Memory::IdFunctor <WORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 1),
			new StringSyntax          (Opcode (0x6F),                   Syntax::SECOND_ARGUMENT,        Argument::NONE,                                          new DX(),         new Memory::IdFunctor <DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 1)),

		Instruction ("OUTSB",
			new ZerarySyntax          (Opcode (0x6E),                   Syntax::NOTHING)),

		Instruction ("OUTSW",
			new ZerarySyntax          (Opcode (0x6F),                   Syntax::MODE_16BITS)),

		Instruction ("OUTSD",
			new ZerarySyntax          (Opcode (0x6F),                   Syntax::MODE_32BITS)),

		Instruction ("PACKSSWB",
			new BinarySyntax          (Opcode (0x0F, 0x63),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x63),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PACKSSDW",
			new BinarySyntax          (Opcode (0x0F, 0x6B),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x6B),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PACKUSWB",
			new BinarySyntax          (Opcode (0x0F, 0x67),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x67),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDB",
			new BinarySyntax          (Opcode (0x0F, 0xFC),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xFC),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDW",
			new BinarySyntax          (Opcode (0x0F, 0xFD),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xFD),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDD",
			new BinarySyntax          (Opcode (0x0F, 0xFE),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xFE),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDQ",
			new BinarySyntax          (Opcode (0x0F, 0xD4),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD4),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDSB",
			new BinarySyntax          (Opcode (0x0F, 0xEC),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xEC),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDSW",
			new BinarySyntax          (Opcode (0x0F, 0xED),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xED),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDUSB",
			new BinarySyntax          (Opcode (0x0F, 0xDC),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xDC),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PADDUSW",
			new BinarySyntax          (Opcode (0x0F, 0xDD),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xDD),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PAND",
			new BinarySyntax          (Opcode (0x0F, 0xDB),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xDB),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PANDN",
			new BinarySyntax          (Opcode (0x0F, 0xDF),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xDF),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PAUSE",
			new ZerarySyntax          (Opcode (0xF3, 0x90),             Syntax::NOTHING)),

		Instruction ("PAVGB",
			new BinarySyntax          (Opcode (0x0F, 0xE0),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE0),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PAVGW",
			new BinarySyntax          (Opcode (0x0F, 0xE3),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE3),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PAVGUSB",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xBF)),

		Instruction ("PCMPEQB",
			new BinarySyntax          (Opcode (0x0F, 0x74),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x74),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PCMPEQW",
			new BinarySyntax          (Opcode (0x0F, 0x75),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x75),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PCMPEQD",
			new BinarySyntax          (Opcode (0x0F, 0x76),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x76),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PCMPGTB",
			new BinarySyntax          (Opcode (0x0F, 0x64),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x64),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PCMPGTW",
			new BinarySyntax          (Opcode (0x0F, 0x65),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x65),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PCMPGTD",
			new BinarySyntax          (Opcode (0x0F, 0x66),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x66),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PEXTRW",
			new BinarySyntax          (Opcode (0x0F, 0xC5),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new GPReg32(), new MMXReg(), new ImmedRange<8, 0, 3>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xC5),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new GPReg32(), new XMMReg(), new ImmedRange<8, 0, 7>())),

		Instruction ("PF2ID",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x1D)),

		Instruction ("PF2IW",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x1C)),

		Instruction ("PFACC",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xAE)),

		Instruction ("PFADD",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x9E)),

		Instruction ("PFCMPEQ",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xB0)),

		Instruction ("PFCMPGE",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x90)),

		Instruction ("PFCMPGT",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xA0)),

		Instruction ("PFMAX",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xA4)),

		Instruction ("PFMIN",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x94)),

		Instruction ("PFMUL",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xB4)),

		Instruction ("PFNACC",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x8A)),

		Instruction ("PFPNACC",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x8E)),

		Instruction ("PFRCP",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x96)),

		Instruction ("PFRCPIT1",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xA6)),

		Instruction ("PFRCPIT2",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xB6)),

		Instruction ("PFRSQIT1",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xA7)),

		Instruction ("PFSUB",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x9A)),

		Instruction ("PFSUBR",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xAA)),

		Instruction ("PI2FD",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x0D)),

		Instruction ("PI2FW",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0x0C)),

		Instruction ("PINSRW",
			new BinarySyntax          (Opcode (0x0F, 0xC4),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new MMXReg(), new GPReg32(), new ImmedRange<8, 0, 3>()),
			new BinarySyntax          (Opcode (0x0F, 0xC4),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new MMXReg(), new Mem16(),   new ImmedRange<8, 0, 3>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xC4),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new GPReg32(), new ImmedRange<8, 0, 7>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xC4),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new Mem16(),   new ImmedRange<8, 0, 7>())),

		Instruction ("PMADDWD",
			new BinarySyntax          (Opcode (0x0F, 0xF5),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF5),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMAXSW",
			new BinarySyntax          (Opcode (0x0F, 0xEE),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xEE),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMAXUB",
			new BinarySyntax          (Opcode (0x0F, 0xDE),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xDE),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMINSW",
			new BinarySyntax          (Opcode (0x0F, 0xEA),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xEA),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMINUB",
			new BinarySyntax          (Opcode (0x0F, 0xDA),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xDA),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMOVMSKB",
			new BinarySyntax          (Opcode (0x0F, 0xD7),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new GPReg32(), new MMXReg()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD7),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new GPReg32(), new XMMReg())),

		Instruction ("PMULHRW",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xB7)),

		Instruction ("PMULHUW",
			new BinarySyntax          (Opcode (0x0F, 0xE4),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE4),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMULHW",
			new BinarySyntax          (Opcode (0x0F, 0xE5),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE5),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMULLW",
			new BinarySyntax          (Opcode (0x0F, 0xD5),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD5),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PMULUDQ",
			new BinarySyntax          (Opcode (0x0F, 0xF4),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF4),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("POP",
			new UnarySyntax           (Opcode (0x8F, 0x00),             Syntax::FIRST_ARGUMENT,                                                                  new SWordMem()),
			new AdditiveUnarySyntax   (Opcode (0x58),                   Syntax::FIRST_ARGUMENT,                                                                  new WordReg()),
			new ZerarySyntax          (Opcode (0x1F),                   Syntax::NOTHING,                                                                         new DS()),
//			new ZerarySyntax          (Opcode (0x0F),                   Syntax::NOTHING,                                                                         new CS()),
			new ZerarySyntax          (Opcode (0x07),                   Syntax::NOTHING,                                                                         new ES()),
			new ZerarySyntax          (Opcode (0x17),                   Syntax::NOTHING,                                                                         new SS()),
			new ZerarySyntax          (Opcode (0x0F, 0xA1),             Syntax::NOTHING,                                                                         new FS()),
			new ZerarySyntax          (Opcode (0x0F, 0xA9),             Syntax::NOTHING,                                                                         new GS())),

		Instruction ("POPA",
			new ZerarySyntax          (Opcode (0x61),                   Syntax::NOTHING)),

		Instruction ("POPAW",
			new ZerarySyntax          (Opcode (0x61),                   Syntax::MODE_16BITS)),

		Instruction ("POPAD",
			new ZerarySyntax          (Opcode (0x61),                   Syntax::MODE_32BITS)),

		Instruction ("POPF",
			new ZerarySyntax          (Opcode (0x9D),                   Syntax::NOTHING)),

		Instruction ("POPFW",
			new ZerarySyntax          (Opcode (0x9D),                   Syntax::MODE_16BITS)),

		Instruction ("POPFD",
			new ZerarySyntax          (Opcode (0x9D),                   Syntax::MODE_32BITS)),

		Instruction ("POR",
			new BinarySyntax          (Opcode (0x0F, 0xEB),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xEB),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PREFETCH",
			new UnarySyntax           (Opcode (0x0F, 0x0D, 0x00),       Syntax::NOTHING,                                                                         new AnyMem())),

		Instruction ("PREFETCH0",
			new UnarySyntax           (Opcode (0x0F, 0x18, 0x01),       Syntax::NOTHING,                                                                         new AnyMem())),

		Instruction ("PREFETCH1",
			new UnarySyntax           (Opcode (0x0F, 0x18, 0x02),       Syntax::NOTHING,                                                                         new AnyMem())),

		Instruction ("PREFETCH2",
			new UnarySyntax           (Opcode (0x0F, 0x18, 0x03),       Syntax::NOTHING,                                                                         new AnyMem())),

		Instruction ("PREFETCHNTA",
			new UnarySyntax           (Opcode (0x0F, 0x18, 0x00),       Syntax::NOTHING,                                                                         new AnyMem())),

		Instruction ("PREFETCHW",
			new UnarySyntax           (Opcode (0x0F, 0x0D, 0x01),       Syntax::NOTHING,                                                                         new AnyMem())),

		Instruction ("PSADBW",
			new BinarySyntax          (Opcode (0x0F, 0xF6),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF6),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSHUFD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x70),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSHUFHW",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x70),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSHUFLW",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x70),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSHUFW",
			new BinarySyntax          (Opcode (0x0F, 0x70),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSLLW",
			new BinarySyntax          (Opcode (0x0F, 0xF1),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x71, 0x06),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 15>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x71, 0x06), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 15>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF1),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSLLD",
			new BinarySyntax          (Opcode (0x0F, 0xF2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x72, 0x06),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 31>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x72, 0x06), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 31>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSLLDQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x73, 0x07), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 15>())),

		Instruction ("PSLLQ",
			new BinarySyntax          (Opcode (0x0F, 0xF3),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x73, 0x06),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 63>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF3),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x73, 0x06), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 63>())),

		Instruction ("PSRAW",
			new BinarySyntax          (Opcode (0x0F, 0xE1),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x71, 0x04),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 15>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE1),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x71, 0x04), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 15>())),

		Instruction ("PSRAD",
			new BinarySyntax          (Opcode (0x0F, 0xE2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x72, 0x04),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 31>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x72, 0x04), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 31>())),

		Instruction ("PSRLW",
			new BinarySyntax          (Opcode (0x0F, 0xD1),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x71, 0x02),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 15>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x71, 0x02), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 15>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD1),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSRLD",
			new BinarySyntax          (Opcode (0x0F, 0xD2),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x72, 0x02),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 31>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x72, 0x02), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 31>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD2),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSRLDQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x73, 0x03), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 15>())),

		Instruction ("PSRLQ",
			new BinarySyntax          (Opcode (0x0F, 0xD3),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x0F, 0x73, 0x02),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new MMXReg(), new ImmedRange<8, 0, 63>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x73, 0x02), Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PARTIAL,        new XMMReg(), new ImmedRange<8, 0, 63>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD3),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBB",
			new BinarySyntax          (Opcode (0x0F, 0xF8),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF8),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBW",
			new BinarySyntax          (Opcode (0x0F, 0xF9),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xF9),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBD",
			new BinarySyntax          (Opcode (0x0F, 0xFA),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xFA),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBQ",
			new BinarySyntax          (Opcode (0x0F, 0xFB),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xFB),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBSB",
			new BinarySyntax          (Opcode (0x0F, 0xE8),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE8),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBSW",
			new BinarySyntax          (Opcode (0x0F, 0xE9),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xE9),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBUSB",
			new BinarySyntax          (Opcode (0x0F, 0xD8),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD8),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSUBUSW",
			new BinarySyntax          (Opcode (0x0F, 0xD9),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xD9),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PSWAPD",
			new SuffixedBinarySyntax  (Opcode (0x0F, 0x0F),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new MMXReg(), new OR<MMXReg, MMXMem>(), 0xBB)),

		Instruction ("PUNPCKHBW",
			new BinarySyntax          (Opcode (0x0F, 0x68),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x68),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKHWD",
			new BinarySyntax          (Opcode (0x0F, 0x69),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x69),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKHDQ",
			new BinarySyntax          (Opcode (0x0F, 0x6A),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x6A),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKHQDQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x6D),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKLBW",
			new BinarySyntax          (Opcode (0x0F, 0x60),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x60),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKLWD",
			new BinarySyntax          (Opcode (0x0F, 0x61),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x61),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKLDQ",
			new BinarySyntax          (Opcode (0x0F, 0x62),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x62),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUNPCKLQDQ",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x6C),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("PUSH",
			new UnarySyntax           (Opcode (0x6A),                   Syntax::NOTHING,                                                                         new Immed<8, Number::SIGNED, false>()),
			new UnarySyntax           (Opcode (0xFF, 0x06),             Syntax::FIRST_ARGUMENT,                                                                  new SWordMem()),
			new AdditiveUnarySyntax   (Opcode (0x50),                   Syntax::FIRST_ARGUMENT,                                                                  new WordReg()),
			new UnarySyntax           (Opcode (0x68),                   Syntax::FIRST_ARGUMENT,                                                                  new DefaultImmed()),
			new ZerarySyntax          (Opcode (0x1E),                   Syntax::NOTHING,                                                                         new DS()),
			new ZerarySyntax          (Opcode (0x0E),                   Syntax::NOTHING,                                                                         new CS()),
			new ZerarySyntax          (Opcode (0x06),                   Syntax::NOTHING,                                                                         new ES()),
			new ZerarySyntax          (Opcode (0x16),                   Syntax::NOTHING,                                                                         new SS()),
			new ZerarySyntax          (Opcode (0x0F, 0xA0),             Syntax::NOTHING,                                                                         new FS()),
			new ZerarySyntax          (Opcode (0x0F, 0xA8),             Syntax::NOTHING,                                                                         new GS())),

		Instruction ("PUSHA",
			new ZerarySyntax          (Opcode (0x60),                   Syntax::NOTHING)),

		Instruction ("PUSHAW",
			new ZerarySyntax          (Opcode (0x60),                   Syntax::MODE_16BITS)),

		Instruction ("PUSHAD",
			new ZerarySyntax          (Opcode (0x60),                   Syntax::MODE_32BITS)),

		Instruction ("PUSHF",
			new ZerarySyntax          (Opcode (0x9C),                   Syntax::NOTHING)),

		Instruction ("PUSHFW",
			new ZerarySyntax          (Opcode (0x9C),                   Syntax::MODE_16BITS)),

		Instruction ("PUSHFD",
			new ZerarySyntax          (Opcode (0x9C),                   Syntax::MODE_32BITS)),

		Instruction ("PXOR",
			new BinarySyntax          (Opcode (0x0F, 0xEF),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new MMXReg(), new OR<MMXReg, MMXMem>()),
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xEF),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("RCL",
			new UnarySyntax           (Opcode (0xD0, 0x02),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x02),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x02),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("RCR",
			new UnarySyntax           (Opcode (0xD0, 0x03),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x03),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x03),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("ROL",
			new UnarySyntax           (Opcode (0xD0, 0x00),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x00),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x00),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("ROR",
			new UnarySyntax           (Opcode (0xD0, 0x01),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x01),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x01),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("RCPPS",
			new BinarySyntax          (Opcode (0x0F, 0x53),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("RCPSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x53),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("RDMSR",
			new ZerarySyntax          (Opcode (0x0F, 0x32),             Syntax::NOTHING)),

		Instruction ("RDPMC",
			new ZerarySyntax          (Opcode (0x0F, 0x33),             Syntax::NOTHING)),

		Instruction ("RDTSC",
			new ZerarySyntax          (Opcode (0x0F, 0x31),             Syntax::NOTHING)),

		Instruction ("REP",
			new ZerarySyntax          (Opcode (0xF3),                   Syntax::NOTHING)),

		Instruction ("REPE",
			new ZerarySyntax          (Opcode (0xF3),                   Syntax::NOTHING)),

		Instruction ("REPZ",
			new ZerarySyntax          (Opcode (0xF3),                   Syntax::NOTHING)),

		Instruction ("REPNE",
			new ZerarySyntax          (Opcode (0xF2),                   Syntax::NOTHING)),

		Instruction ("REPNZ",
			new ZerarySyntax          (Opcode (0xF2),                   Syntax::NOTHING)),

		Instruction ("RSM",
			new ZerarySyntax          (Opcode (0x0F, 0xAA),             Syntax::NOTHING)),

		Instruction ("RSQRTPS",
			new BinarySyntax          (Opcode (0x0F, 0x52),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("RSQRTSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x52),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("RET",
			new ZerarySyntax          (Opcode (0xC3),                   Syntax::NOTHING),
			new UnarySyntax           (Opcode (0xC2),                   Syntax::NOTHING,                                                                         new Immed<16, Number::UNSIGNED>())),

		Instruction ("RETN",
			new ZerarySyntax          (Opcode (0xC3),                   Syntax::NOTHING),
			new UnarySyntax           (Opcode (0xC2),                   Syntax::NOTHING,                                                                         new Immed<16, Number::UNSIGNED>())),

		Instruction ("RETF",
			new ZerarySyntax          (Opcode (0xCB),                   Syntax::NOTHING),
			new UnarySyntax           (Opcode (0xCA),                   Syntax::NOTHING,                                                                         new Immed<16, Number::UNSIGNED>())),

		Instruction ("SAHF",
			new ZerarySyntax          (Opcode (0x9E),                   Syntax::NOTHING)),

		Instruction ("SAL",
			new UnarySyntax           (Opcode (0xD0, 0x04),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x04),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x04),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("SAR",
			new UnarySyntax           (Opcode (0xD0, 0x07),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new BinarySyntax          (Opcode (0xD2, 0x07),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new CL()),
			new BinarySyntax          (Opcode (0xC0, 0x07),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("SBB",
			new BinarySyntax          (Opcode (0x18),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR<GPReg, GPMem>()),
			new BinarySyntax          (Opcode (0x83, 0x03),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<WordReg, SWordMem>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x1C),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x03),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPMem, GPReg>(),       new Immediate::IdFunctor())),

		Instruction ("SCAS",
			new StringSyntax          (Opcode (0xAE),                   Syntax::NOTHING,                Argument::NONE,                                          new Memory::IdFunctor <BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(),  0, 2),
			new StringSyntax          (Opcode (0xAF),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <WORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0, 2),
			new StringSyntax          (Opcode (0xAF),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0, 2)),

		Instruction ("SCASB",
			new ZerarySyntax          (Opcode (0xAE),                   Syntax::NOTHING)),

		Instruction ("SCASW",
			new ZerarySyntax          (Opcode (0xAF),                   Syntax::MODE_16BITS)),

		Instruction ("SCASD",
			new ZerarySyntax          (Opcode (0xAF),                   Syntax::MODE_32BITS)),

		Instruction ("SETA",
			new UnarySyntax           (Opcode (0x0F, 0x97, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETAE",
			new UnarySyntax           (Opcode (0x0F, 0x93, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETB",
			new UnarySyntax           (Opcode (0x0F, 0x92, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETBE",
			new UnarySyntax           (Opcode (0x0F, 0x96, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETC",
			new UnarySyntax           (Opcode (0x0F, 0x92, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETE",
			new UnarySyntax           (Opcode (0x0F, 0x94, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETG",
			new UnarySyntax           (Opcode (0x0F, 0x9F, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETGE",
			new UnarySyntax           (Opcode (0x0F, 0x9D, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETL",
			new UnarySyntax           (Opcode (0x0F, 0x9C, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETLE",
			new UnarySyntax           (Opcode (0x0F, 0x9E, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNA",
			new UnarySyntax           (Opcode (0x0F, 0x96, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNAE",
			new UnarySyntax           (Opcode (0x0F, 0x92, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNB",
			new UnarySyntax           (Opcode (0x0F, 0x93, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNBE",
			new UnarySyntax           (Opcode (0x0F, 0x97, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNC",
			new UnarySyntax           (Opcode (0x0F, 0x93, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNE",
			new UnarySyntax           (Opcode (0x0F, 0x95, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNG",
			new UnarySyntax           (Opcode (0x0F, 0x9E, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNGE",
			new UnarySyntax           (Opcode (0x0F, 0x9C, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNL",
			new UnarySyntax           (Opcode (0x0F, 0x9D, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNLE",
			new UnarySyntax           (Opcode (0x0F, 0x9F, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNO",
			new UnarySyntax           (Opcode (0x0F, 0x91, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNP",
			new UnarySyntax           (Opcode (0x0F, 0x9B, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNS",
			new UnarySyntax           (Opcode (0x0F, 0x99, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETNZ",
			new UnarySyntax           (Opcode (0x0F, 0x95, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETO",
			new UnarySyntax           (Opcode (0x0F, 0x90, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETP",
			new UnarySyntax           (Opcode (0x0F, 0x9A, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETPE",
			new UnarySyntax           (Opcode (0x0F, 0x9A, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETPO",
			new UnarySyntax           (Opcode (0x0F, 0x9B, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETS",
			new UnarySyntax           (Opcode (0x0F, 0x98, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SETZ",
			new UnarySyntax           (Opcode (0x0F, 0x94, 0x00),       Syntax::NOTHING,                                                                         new OR <GPReg8, Mem8>())),

		Instruction ("SFENCE",
			new ZerarySyntax          (Opcode (0x0F, 0xAE, 0xF8),       Syntax::NOTHING)),

		Instruction ("SGDT",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x00),       Syntax::FIRST_ARGUMENT,                                                                  new Mem48())),

		Instruction ("SHL",
			new UnarySyntax           (Opcode (0xD0, 0x04),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x04),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x04),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHLD",
			new BinarySyntax          (Opcode (0x0F, 0xA5),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<WordReg, WordMem>(),             new WordReg(),                     new CL()),
			new BinarySyntax          (Opcode (0x0F, 0xA4),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   0, BinarySyntax::PRESENT,        new OR<WordReg, WordMem>(),             new WordReg(),                     new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHR",
			new UnarySyntax           (Opcode (0xD0, 0x05),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new ImmedEqual<UNDEFINED, 1>(), 1),
			new UnarySyntax           (Opcode (0xD2, 0x05),             Syntax::FIRST_ARGUMENT,                                                                  new OR<GPReg, SGPMem>(),              new CL(), 1),
			new BinarySyntax          (Opcode (0xC0, 0x05),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),              new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHRD",
			new BinarySyntax          (Opcode (0x0F, 0xAD),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new OR<WordReg, WordMem>(),             new WordReg(),                     new CL()),
			new BinarySyntax          (Opcode (0x0F, 0xAC),             Syntax::FIRST_ARGUMENT,  false, Argument::BIT_NUMBER,   0, BinarySyntax::PRESENT,        new OR<WordReg, WordMem>(),             new WordReg(),                     new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHUFPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0xC6),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>(), new ImmedRange<8, 0, 3>())),

		Instruction ("SHUFPS",
			new BinarySyntax          (Opcode (0x0F, 0xC6),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("SIDT",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x01),       Syntax::FIRST_ARGUMENT,                                                                  new Mem48())),

		Instruction ("SLDT",
			new UnarySyntax           (Opcode (0x0F, 0x00, 0x00),       Syntax::FIRST_ARGUMENT,                                                                  new OR<WordReg, WordMem>())),

		Instruction ("SMSW",
			new UnarySyntax           (Opcode (0x0F, 0x01, 0x04),       Syntax::FIRST_ARGUMENT,                                                                  new OR<WordReg, Mem16>())),

		Instruction ("SQRTPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x51),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("SQRTPS",
			new BinarySyntax          (Opcode (0x0F, 0x51),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("SQRTSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x51),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("SQRTSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x51),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("STC",
			new ZerarySyntax          (Opcode (0xF9),                   Syntax::NOTHING)),

		Instruction ("STD",
			new ZerarySyntax          (Opcode (0xFD),                   Syntax::NOTHING)),

		Instruction ("STI",
			new ZerarySyntax          (Opcode (0xFB),                   Syntax::NOTHING)),

		Instruction ("STMXCSR",
			new UnarySyntax           (Opcode (0x0F, 0xAE, 0x03),       Syntax::NOTHING,                                                                         new GPReg32())),

		Instruction ("STOS",
			new StringSyntax          (Opcode (0xAA),                   Syntax::NOTHING,                Argument::NONE,                                          new Memory::IdFunctor <BYTE, UNDEFINED, (UNDEFINED | INTEGER)>(),  0, 2),
			new StringSyntax          (Opcode (0xAB),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <WORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0, 2),
			new StringSyntax          (Opcode (0xAB),                   Syntax::FIRST_ARGUMENT,         Argument::NONE,                                          new Memory::IdFunctor <DWORD, UNDEFINED, (UNDEFINED | INTEGER)>(), 0, 2)),

		Instruction ("STOSB",
			new ZerarySyntax          (Opcode (0xAA),                   Syntax::NOTHING)),

		Instruction ("STOSW",
			new ZerarySyntax          (Opcode (0xAB),                   Syntax::MODE_16BITS)),

		Instruction ("STOSD",
			new ZerarySyntax          (Opcode (0xAB),                   Syntax::MODE_32BITS)),

		Instruction ("STR",
			new UnarySyntax           (Opcode (0x0F, 0x00, 0x01),       Syntax::FIRST_ARGUMENT,                                                                  new OR<WordReg, WordMem>())),

		Instruction ("SUB",
			new BinarySyntax          (Opcode (0x28),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR<GPReg, GPMem>()),
			new BinarySyntax          (Opcode (0x83, 0x05),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<WordReg, SWordMem>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x2C),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x05),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<GPMem, GPReg>(),       new Immediate::IdFunctor())),

		Instruction ("SUBPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x5C),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("SUBPS",
			new BinarySyntax          (Opcode (0x0F, 0x5C),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("SUBSD",
			new BinarySyntax          (Opcode (0xF2, 0x0F, 0x5C),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("SUBSS",
			new BinarySyntax          (Opcode (0xF3, 0x0F, 0x5C),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Memory::IdFunctor<UNDEFINED | DWORD, UNDEFINED, (UNDEFINED | FLOAT)> >())),

		Instruction ("SYSENTER",
			new ZerarySyntax          (Opcode (0x0F, 0x34),             Syntax::NOTHING)),

		Instruction ("SYSEXIT",
			new ZerarySyntax          (Opcode (0x0F, 0x35),             Syntax::NOTHING)),

		Instruction ("TEST",
			new BinarySyntax          (Opcode (0xA8),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),         new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0xF6, 0x00),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,	     1, BinarySyntax::PARTIAL,        new OR<GPReg, SGPMem>(),   new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x84),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,        1, BinarySyntax::PRESENT,        new OR<GPReg, GPMem>(),    new GPReg())),

		Instruction ("UCOMISD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x2E),       Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem64f>())),

		Instruction ("UCOMISS",
			new BinarySyntax          (Opcode (0x0F, 0x2E),             Syntax::NOTHING,         false, Argument::NONE,         0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, Mem32f>())),

		Instruction ("UD2",
			new ZerarySyntax          (Opcode (0x0F, 0x0B),             Syntax::NOTHING)),

		Instruction ("UNPCKHPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x15),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("UNPCKHPS",
			new BinarySyntax          (Opcode (0x0F, 0x15),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("UNPCKLPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x14),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("UNPCKLPS",
			new BinarySyntax          (Opcode (0x0F, 0x14),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("VERR",
			new UnarySyntax           (Opcode (0x0F, 0x00, 0x04),       Syntax::NOTHING,                                                                         new OR<GPReg16, Mem16>())),

		Instruction ("VERW",
			new UnarySyntax           (Opcode (0x0F, 0x00, 0x05),       Syntax::NOTHING,                                                                         new OR<GPReg16, Mem16>())),

		Instruction ("WAIT",
			new ZerarySyntax          (Opcode (0x9B),                   Syntax::NOTHING)),

		Instruction ("WBINVD",
			new ZerarySyntax          (Opcode (0x0F, 0x09),             Syntax::NOTHING)),

		Instruction ("WRMSR",
			new ZerarySyntax          (Opcode (0x0F, 0x30),             Syntax::NOTHING)),

		Instruction ("XADD",
			new BinarySyntax          (Opcode (0x0F, 0xC0),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PRESENT,        new OR<GPReg, GPMem>(), new GPReg())),

		Instruction ("XCHG",
			new AdditiveUnarySyntax   (Opcode (0x90),                   Syntax::FIRST_ARGUMENT,                                                                  new WordReg(),     new Accumulator()),
			new AdditiveUnarySyntax   (Opcode (0x90),                   Syntax::SECOND_ARGUMENT,                                                                 new Accumulator(), new WordReg()),
			new BinarySyntax          (Opcode (0x86),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,        1, BinarySyntax::PRESENT,        new OR<GPReg, GPMem>(),   new GPReg())),

		Instruction ("XLAT",
			new StringSyntax          (Opcode (0xD7),                   Syntax::NOTHING,                Argument::NONE,                                          new SMem8,  0, 0)),

		Instruction ("XLATB",
			new ZerarySyntax          (Opcode (0xD7),                   Syntax::NOTHING)),

		Instruction ("XOR",
			new BinarySyntax          (Opcode (0x30),                   Syntax::FIRST_ARGUMENT,  true,  Argument::EQUAL,	     3, BinarySyntax::PRESENT,        new GPReg(),                  new OR<GPReg, GPMem>()),
			new BinarySyntax          (Opcode (0x83, 0x06),             Syntax::FIRST_ARGUMENT,  false, Argument::NONE,         1, BinarySyntax::PARTIAL,        new OR<WordReg, SWordMem>(),  new Immed<8, Number::SIGNED, false>()),
			new BinarySyntax          (Opcode (0x34),                   Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::ABSENT,         new Accumulator(),            new Immediate::IdFunctor()),
			new BinarySyntax          (Opcode (0x80, 0x06),             Syntax::FIRST_ARGUMENT,  false, Argument::EQUAL,        1, BinarySyntax::PARTIAL,        new OR<SGPMem,GPReg>(),       new Immediate::IdFunctor())),

		Instruction ("XORPD",
			new BinarySyntax          (Opcode (0x66, 0x0F, 0x57),       Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),

		Instruction ("XORPS",
			new BinarySyntax          (Opcode (0x0F, 0x57),             Syntax::NOTHING,         false, Argument::EQUAL,        0, BinarySyntax::PRESENT,        new XMMReg(), new OR<XMMReg, XMMMem>())),
	};

	unsigned int n = sizeof (Instructions) / sizeof (Instruction);
	for (unsigned int i = 0; i < n; i++)
	{
		try
		{
			InstructionTable.Insert (&(Instructions[i]));
		}
		catch (exception &e)
		{
			cout << e.what() << ": " << Instructions[i].GetName() << endl;
		}
	}
}

InvalidSyntax::InvalidSyntax (const Command *cmd, const vector<Argument *> args) throw()
	: BadCommand(cmd), BadArguments(args), WhatString("Invalid combination of arguments for this command.")
{
}

const char *InvalidSyntax::what() const throw ()
{
	return WhatString.c_str();
}
