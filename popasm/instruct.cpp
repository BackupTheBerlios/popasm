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

void Instruction::Assemble (const BasicSymbol *sym, vector<Argument *> &Arguments, vector<Byte> &Encoding) const
{
	// Include in the symbol table the creation of the new label
	if (sym != 0)
	{
		cout << "Label definition not implemented yet." << endl;
		return;
	}

	// Checks all syntaxes avaiable
	for (ContainerType::const_reverse_iterator i = Syntaxes.rbegin(); i != Syntaxes.rend(); i++)
	{
		if ((*i)->Assemble (Arguments, Encoding)) return;
	}

	// None of them fits, so throw exception
	throw InvalidSyntax(this, Arguments);
}

HashTable <Instruction *, HashFunctor, PointerComparator<Instruction> > Instruction::InstructionTable = HashTable <Instruction *, HashFunctor, PointerComparator<Instruction> > ();

// Memory functors
typedef Memory::IdFunctor<UNDEFINED | BYTE | WORD | DWORD, UNDEFINED, UNDEFINED | INTEGER> GPMem;
typedef Memory::IdFunctor<UNDEFINED | WORD,                UNDEFINED, UNDEFINED | INTEGER> Mem16;
typedef Memory::IdFunctor<UNDEFINED | DWORD,               UNDEFINED, UNDEFINED | INTEGER> Mem32;
typedef Memory::IdFunctor<UNDEFINED | QWORD,               UNDEFINED, UNDEFINED | INTEGER> MMXMem;
typedef Memory::IdFunctor<UNDEFINED | WORD | DWORD,        UNDEFINED, UNDEFINED | INTEGER> WordMem;

// Register functors
typedef Register::IdFunctor <GPRegister,  ANY>               GPReg;
typedef Register::IdFunctor <GPRegister,  ANY, BYTE>         GPReg8;
typedef Register::IdFunctor <GPRegister,  ANY, WORD>         GPReg16;
typedef Register::IdFunctor <GPRegister,  ANY, DWORD>        GPReg32;
typedef Register::IdFunctor <GPRegister,  ANY, WORD | DWORD> WordReg;
typedef Register::IdFunctor <MMXRegister, ANY>               MMXReg;

// Specific register functors
typedef Register::IdFunctor<GPRegister,       0> Accumulator;
typedef Register::IdFunctor<FPURegister,      0> ST;
typedef Register::IdFunctor<GPRegister8Bits,  0> AL;
typedef Register::IdFunctor<GPRegister16Bits, 0> AX;
typedef Register::IdFunctor<GPRegister16Bits, 2> DX;
typedef Register::IdFunctor<GPRegister8Bits,  1> CL;

// Segment register functors
typedef Register::IdFunctor<SegmentRegister,  0> ES;
typedef Register::IdFunctor<SegmentRegister,  1> CS;
typedef Register::IdFunctor<SegmentRegister,  2> SS;
typedef Register::IdFunctor<SegmentRegister,  3> DS;
typedef Register::IdFunctor<SegmentRegister,  4> FS;
typedef Register::IdFunctor<SegmentRegister,  5> GS;

// Combinations with memory and registers
typedef BinaryCompose <logical_or<bool>, GPMem,   GPReg>   MemGPReg;
typedef BinaryCompose <logical_or<bool>, MMXMem,  MMXReg>  MemMMXReg;
typedef BinaryCompose <logical_or<bool>, Mem16,   GPReg16> MemGPReg16;
typedef BinaryCompose <logical_or<bool>, Mem32,   GPReg32> MemGPReg32;
typedef BinaryCompose <logical_or<bool>, WordMem, WordReg> MemWordReg;

template <unsigned int sz = 0, Memory::ContentsType ct = Memory::FLOAT>
class MemXMMReg : public BinaryCompose <logical_or<bool>,  Mem<sz, ct>, XMMRegister::IdFunctor>
{
};

// AMD 3DNow! Instructions
class TDNowSyntax : public BinarySyntax
{
	Byte Suffix;

	public:
	TDNowSyntax (unsigned int p, Byte suf) throw ();
	~TDNowSyntax () throw () {}

	bool Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const;
};

TDNowSyntax::TDNowSyntax (unsigned int p, Byte suf) throw () :
	BinarySyntax (p, Opcode (0x0F, 0x0F), NOTHING, false, Argument::EQUAL, 0, PRESENT,
	new MMXRegister::IdFunctor(), new MemMMXReg ()), Suffix(suf) {}

bool TDNowSyntax::Assemble (vector<Argument *> &Arguments, vector<Byte> &Output) const
{
	BinarySyntax::Assemble (Arguments, Output);
	Output.push_back (Suffix);
	return true;
}

void Instruction::SetupInstructionTable () throw ()
{
	static Instruction Instructions[] =
	{
		Instruction ("AAA",
			new ZerarySyntax        (100, Opcode (0x37),             Syntax::NOTHING)),

		Instruction ("AAD",
			new UnarySyntax         (100, Opcode (0xD5),             Syntax::NOTHING, new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax        (110, Opcode (0xD5, 0x0A),       Syntax::NOTHING)),

		Instruction ("AAM",
			new UnarySyntax         (100, Opcode (0xD4),             Syntax::NOTHING, new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax        (110, Opcode (0xD4, 0x0A),       Syntax::NOTHING)),

		Instruction ("AAS",
			new ZerarySyntax        (100, Opcode (0x3F),             Syntax::NOTHING)),

		Instruction ("ADC",
			new BinarySyntax        (110, Opcode (0x10),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x14),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("ADD",
			new BinarySyntax        (110, Opcode (0x00),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x04),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("ADDPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x58), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ADDPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x58),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ADDSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x58), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("ADDSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x58), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("AND",
			new BinarySyntax        (110, Opcode (0x20),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x24),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("ANDPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x54), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ANDPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x54),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ANDNPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x55), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ANDNPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x55),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ARPL",
			new BinarySyntax        (100, Opcode (0x63),             Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemReg16(),   new GPRegister16Bits::IdFunctor())),

		Instruction ("BOUND",
			new BinarySyntax        (100, Opcode (0x62),             Syntax::FIRST_ARGUMENT, false, Argument::HALF,         0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("BSF",
			new BinarySyntax        (100, Opcode (0x0F, 0xBC),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		Instruction ("BSR",
			new BinarySyntax        (100, Opcode (0x0F, 0xBD),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		Instruction ("BSWAP",
			new AdditiveUnarySyntax (100, Opcode (0x0F, 0xC8),       Syntax::FIRST_ARGUMENT,                                                   new GPRegister32Bits::IdFunctor())),

		Instruction ("BT",
			new BinarySyntax        (100, Opcode (0x0F, 0xA3),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x04), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("BTC",
			new BinarySyntax        (100, Opcode (0x0F, 0xBB),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x07), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("BTR",
			new BinarySyntax        (100, Opcode (0x0F, 0xB3),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x06), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("BTS",
			new BinarySyntax        (100, Opcode (0x0F, 0xAB),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x05), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("CALL",
			new UnarySyntax         (100, Opcode (0xFF, 0x03),       Syntax::FULL_POINTER,                                                            new Mem<0, Memory::INTEGER, Type::FAR>()),
			new UnarySyntax         (110, Opcode (0xFF, 0x02),       Syntax::FIRST_ARGUMENT,                                                          new WordRegNearMem()),
			new RelativeUnarySyntax (120, Opcode (0xE8),             Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR)),
			new UnarySyntax         (130, Opcode (0x9A),             Syntax::FIRST_ARGUMENT,                                                          new FullPointer::IdFunctor())),

		Instruction ("CBW",
			new ZerarySyntax        (100, Opcode (0x98),             Syntax::MODE_16BITS)),

		Instruction ("CDQ",
			new ZerarySyntax        (100, Opcode (0x99),             Syntax::MODE_32BITS)),

		Instruction ("CLC",
			new ZerarySyntax        (100, Opcode (0xF8),             Syntax::NOTHING)),

		Instruction ("CLD",
			new ZerarySyntax        (100, Opcode (0xFC),             Syntax::NOTHING)),

		Instruction ("CLFLUSH",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x07), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("CLI",
			new ZerarySyntax        (100, Opcode (0xFA),             Syntax::NOTHING)),

		Instruction ("CLTS",
			new ZerarySyntax        (100, Opcode (0x0F, 0X06),       Syntax::NOTHING)),

		Instruction ("CMC",
			new ZerarySyntax        (100, Opcode (0xF5),             Syntax::NOTHING)),

		Instruction ("CMOVA",
			new BinarySyntax        (100, Opcode (0x0F, 0x47),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVAE",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVB",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVBE",
			new BinarySyntax        (100, Opcode (0x0F, 0x46),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVC",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVE",
			new BinarySyntax        (100, Opcode (0x0F, 0x44),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVG",
			new BinarySyntax        (100, Opcode (0x0F, 0x4F),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVGE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4D),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVL",
			new BinarySyntax        (100, Opcode (0x0F, 0x4C),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVLE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4E),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNA",
			new BinarySyntax        (100, Opcode (0x0F, 0x46),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNAE",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNB",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNBE",
			new BinarySyntax        (100, Opcode (0x0F, 0x47),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNC",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNE",
			new BinarySyntax        (100, Opcode (0x0F, 0x45),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNG",
			new BinarySyntax        (100, Opcode (0x0F, 0x4E),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNGE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4C),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNL",
			new BinarySyntax        (100, Opcode (0x0F, 0x4D),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNLE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4F),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNO",
			new BinarySyntax        (100, Opcode (0x0F, 0x41),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNP",
			new BinarySyntax        (100, Opcode (0x0F, 0x4B),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNS",
			new BinarySyntax        (100, Opcode (0x0F, 0x49),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVNZ",
			new BinarySyntax        (100, Opcode (0x0F, 0x45),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVO",
			new BinarySyntax        (100, Opcode (0x0F, 0x40),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVP",
			new BinarySyntax        (100, Opcode (0x0F, 0x4A),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVPE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4A),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVPO",
			new BinarySyntax        (100, Opcode (0x0F, 0x4B),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVS",
			new BinarySyntax        (100, Opcode (0x0F, 0x48),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMOVZ",
			new BinarySyntax        (100, Opcode (0x0F, 0x44),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS, new WordReg(),    new MemWordReg())),

		Instruction ("CMP",
			new BinarySyntax        (110, Opcode (0x38),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x07),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x3C),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x07),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("CMPPD",
			new BinarySyntax         (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("CMPEQPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 0)),

		Instruction ("CMPLTPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 1)),

		Instruction ("CMPLEPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 2)),

		Instruction ("CMPUNORDPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 3)),

		Instruction ("CMPNEQPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 4)),

		Instruction ("CMPNLTPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 5)),

		Instruction ("CMPNLEPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 6)),

		Instruction ("CMPORDPD",
			new SuffixedBinarySyntax (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 7)),

		Instruction ("CMPPS",
			new BinarySyntax         (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("CMPEQPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 0)),

		Instruction ("CMPLTPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 1)),

		Instruction ("CMPLEPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 2)),

		Instruction ("CMPUNORDPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 3)),

		Instruction ("CMPNEQPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 4)),

		Instruction ("CMPNLTPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 5)),

		Instruction ("CMPNLEPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 6)),

		Instruction ("CMPORDPS",
			new SuffixedBinarySyntax (100, Opcode (0x0F, 0xC2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), 7)),

		Instruction ("CMPS",
			new StringSyntax         (100, Opcode (0xA6),             Syntax::FIRST_ARGUMENT,        Argument::EQUAL,                                  new Mem<8>(),     new Mem<8>(), 0),
			new StringSyntax         (110, Opcode (0xA7),             Syntax::FIRST_ARGUMENT,        Argument::EQUAL,                                  new Mem<16>(),    new Mem<16>(), 0),
			new StringSyntax         (120, Opcode (0xA7),             Syntax::FIRST_ARGUMENT,        Argument::EQUAL,                                  new Mem<32>(),    new Mem<32>(), 0)),

		Instruction ("CMPSB",
			new ZerarySyntax        (100, Opcode (0xA6),             Syntax::NOTHING)),

		Instruction ("CMPSW",
			new ZerarySyntax        (100, Opcode (0xA7),             Syntax::MODE_16BITS)),

		Instruction ("CMPSD",
			new ZerarySyntax        (100, Opcode (0xA7),             Syntax::MODE_32BITS),
			new BinarySyntax        (110, Opcode (0xF2, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("CMPEQSD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 0)),

		Instruction ("CMPLTSD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 1)),

		Instruction ("CMPLESD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 2)),

		Instruction ("CMPUNORDSD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 3)),

		Instruction ("CMPNEQSD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 4)),

		Instruction ("CMPNLTSD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 5)),

		Instruction ("CMPNLESD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 6)),

		Instruction ("CMPORDSD",
			new SuffixedBinarySyntax (100, Opcode (0xF2, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), 7)),

		Instruction ("CMPSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("CMPEQSS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 0)),

		Instruction ("CMPLTSS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 1)),

		Instruction ("CMPLESS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 2)),

		Instruction ("CMPUNORDSS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 3)),

		Instruction ("CMPNEQSS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 4)),

		Instruction ("CMPNLTSS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 5)),

		Instruction ("CMPNLESS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 6)),

		Instruction ("CMPORDSS",
			new SuffixedBinarySyntax (100, Opcode (0xF3, 0x0F, 0xC2),Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), 7)),

		Instruction ("CMPXCHG",
			new BinarySyntax        (100, Opcode (0x0F, 0xB0),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemGPReg(),   new GPRegister::IdFunctor())),

		Instruction ("CMPXCHG8B",
			new UnarySyntax         (100, Opcode (0x0F, 0xC7, 0x01), Syntax::NOTHING,                                                          new Mem<64>())),

		Instruction ("COMISD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2F), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("COMISS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2F),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::EXCHANGED_REGS,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CPUID",
			new ZerarySyntax        (100, Opcode (0x0F, 0xA2),       Syntax::NOTHING)),

		Instruction ("CVTDQ2PD",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0xE6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::INTEGER>())),

		Instruction ("CVTDQ2PS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5B),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::INTEGER>())),

		Instruction ("CVTPD2DQ",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0xE6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("CVTPD2PI",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::INTEGER>())),

		Instruction ("CVTPD2PS",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("CVTPI2PD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemMMXReg())),

		Instruction ("CVTPI2PS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2A),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemMMXReg())),

		Instruction ("CVTPS2DQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5B), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("CVTPS2PD",
			new BinarySyntax        (100, Opcode (0x0F, 0x5A),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CVTPS2PI",
			new BinarySyntax        (100, Opcode (0x0F, 0x2D),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CVTSD2SI",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x2D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CVTSD2SS",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CVTSI2SD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x2A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemReg32)),

		Instruction ("CVTSI2SS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x2A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemReg32)),

		Instruction ("CVTSS2SD",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("CVTSS2SI",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x2D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("CVTTPD2PI",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2C), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("CVTTPD2DQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xE6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("CVTTPS2DQ",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5B), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("CVTTPS2PI",
			new BinarySyntax        (100, Opcode (0x0F, 0x2C),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CVTTSD2SI",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x2C), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("CVTTSS2SI",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x2C), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("CWD",
			new ZerarySyntax        (100, Opcode (0x99),             Syntax::MODE_16BITS)),

		Instruction ("CWDE",
			new ZerarySyntax        (100, Opcode (0x98),             Syntax::MODE_32BITS)),

		Instruction ("DAA",
			new ZerarySyntax        (100, Opcode (0x27),             Syntax::NOTHING)),

		Instruction ("DAS",
			new ZerarySyntax        (100, Opcode (0x2F),             Syntax::NOTHING)),

		Instruction ("DEC",
			new UnarySyntax         (100, Opcode (0xFE, 0x01),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1),
			new AdditiveUnarySyntax (110, Opcode (0x48),             Syntax::FIRST_ARGUMENT,                                                   new WordReg())),

		Instruction ("DIV",
			new UnarySyntax         (100, Opcode (0xF6, 0x06),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1)),

		Instruction ("DIVPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5E), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("DIVPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5E),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("DIVSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5E), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("DIVSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5E), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("EMMS",
			new ZerarySyntax        (100, Opcode (0x0F, 0x77),       Syntax::NOTHING)),

		Instruction ("ENTER",
			new BinarySyntax        (100, Opcode (0xC8),             Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::ABSENT,  new Immed<16, Number::UNSIGNED>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("F2XM1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF0),       Syntax::NOTHING)),

		Instruction ("FABS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE1),       Syntax::NOTHING)),

		Instruction ("FADD",
			new UnarySyntax         (100, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x00),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xC0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		Instruction ("FADDP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xC1),       Syntax::NOTHING)),

		Instruction ("FIADD",
			new UnarySyntax         (100, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FBLD",
			new UnarySyntax         (100, Opcode (0xDF, 0x04),       Syntax::NOTHING,                                                          new Mem<80, Memory::BCD>())),

		Instruction ("FBSTP",
			new UnarySyntax         (100, Opcode (0xDF, 0x06),       Syntax::NOTHING,                                                          new Mem<80, Memory::BCD>())),

		Instruction ("FCHS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE0),       Syntax::NOTHING)),

		Instruction ("FCLEX",
			new ZerarySyntax        (100, Opcode (0x9B, 0xDB, 0xE2), Syntax::NOTHING)),

		Instruction ("FNCLEX",
			new ZerarySyntax        (100, Opcode (0xDB, 0xE2),       Syntax::NOTHING)),

		Instruction ("FCMOVB",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVE",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVBE",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xD0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVU",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xD8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVNB",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVNE",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVNBE",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xD0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCMOVNU",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xD8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCOM",
			new UnarySyntax         (100, Opcode (0xD8, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x02),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xD8, 0xD0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD8, 0xD1),       Syntax::NOTHING)),

		Instruction ("FCOMP",
			new UnarySyntax         (100, Opcode (0xD8, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x03),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xD8, 0xD8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD8, 0xD9),       Syntax::NOTHING)),

		Instruction ("FCOMPP",
			new ZerarySyntax        (100, Opcode (0xDE, 0xD9),       Syntax::NOTHING)),

		Instruction ("FCOMI",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCOMIP",
			new FPUBinarySyntax     (100, Opcode (0xDF, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FUCOMI",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FUCOMIP",
			new FPUBinarySyntax     (100, Opcode (0xDF, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		Instruction ("FCOS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFF),       Syntax::NOTHING)),

		Instruction ("FDECSTP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF6),       Syntax::NOTHING)),

		Instruction ("FDIV",
			new UnarySyntax         (100, Opcode (0xD8, 0x06),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x06),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xF8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		Instruction ("FDIVP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xF8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xF9),       Syntax::NOTHING)),

		Instruction ("FIDIV",
			new UnarySyntax         (100, Opcode (0xDA, 0x06),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x06),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FDIVR",
			new UnarySyntax         (100, Opcode (0xD8, 0x07),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x07),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xF8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xF0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		Instruction ("FDIVRP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xF1),       Syntax::NOTHING)),

		Instruction ("FIDIVR",
			new UnarySyntax         (100, Opcode (0xDA, 0x07),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FEMMS",
			new ZerarySyntax        (100, Opcode (0x0F, 0x0E),       Syntax::NOTHING)),

		Instruction ("FFREE",
			new AdditiveUnarySyntax (100, Opcode (0xDD, 0xC0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		Instruction ("FICOM",
			new UnarySyntax         (100, Opcode (0xDE, 0x02),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDA, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		Instruction ("FICOMP",
			new UnarySyntax         (100, Opcode (0xDE, 0x03),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDA, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		Instruction ("FILD",
			new UnarySyntax         (100, Opcode (0xDF, 0x00),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDB, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (120, Opcode (0xDF, 0x05),       Syntax::NOTHING,                                                          new Mem<64, Memory::INTEGER>())),

		Instruction ("FINCSTP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF7),       Syntax::NOTHING)),

		Instruction ("FINIT",
			new ZerarySyntax        (100, Opcode (0x9B, 0xDB, 0xE3), Syntax::NOTHING)),

		Instruction ("FNINIT",
			new ZerarySyntax        (100, Opcode (0xDB, 0xE3),       Syntax::NOTHING)),

		Instruction ("FIST",
			new UnarySyntax         (100, Opcode (0xDF, 0x02),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDB, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		Instruction ("FISTP",
			new UnarySyntax         (100, Opcode (0xDF, 0x03),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDF, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (120, Opcode (0xDF, 0x07),       Syntax::NOTHING,                                                          new Mem<64, Memory::INTEGER>())),

		Instruction ("FLD",
			new UnarySyntax         (100, Opcode (0xD9, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x00),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new UnarySyntax         (120, Opcode (0xDB, 0x05),       Syntax::NOTHING,                                                          new Mem<80, Memory::FLOAT>()),
			new AdditiveUnarySyntax (130, Opcode (0xD9, 0xC0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		Instruction ("FLD1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE8),       Syntax::NOTHING)),

		Instruction ("FLDL2T",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE9),       Syntax::NOTHING)),

		Instruction ("FLDL2E",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEA),       Syntax::NOTHING)),

		Instruction ("FLDPI",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEB),       Syntax::NOTHING)),

		Instruction ("FLDLG2",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEC),       Syntax::NOTHING)),

		Instruction ("FLDLN2",
			new ZerarySyntax        (100, Opcode (0xD9, 0xED),       Syntax::NOTHING)),

		Instruction ("FLDZ",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEE),       Syntax::NOTHING)),

		Instruction ("FLDCW",
			new UnarySyntax         (100, Opcode (0xD9, 0x05),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FLDENV",
			new UnarySyntax         (100, Opcode (0xD9, 0x04),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FMUL",
			new UnarySyntax         (100, Opcode (0xD8, 0x01),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x01),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xC8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		Instruction ("FMULP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xC9),       Syntax::NOTHING)),

		Instruction ("FIMUL",
			new UnarySyntax         (100, Opcode (0xDA, 0x01),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x01),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FNOP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xD0),       Syntax::NOTHING)),

		Instruction ("FATAN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF3),       Syntax::NOTHING)),

		Instruction ("FPREM",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF8),       Syntax::NOTHING)),

		Instruction ("FPREM1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF5),       Syntax::NOTHING)),

		Instruction ("FTAN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF2),       Syntax::NOTHING)),

		Instruction ("FRDINT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFC),       Syntax::NOTHING)),

		Instruction ("FRSTOR",
			new UnarySyntax         (100, Opcode (0xDD, 0x04),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FSAVE",
			new UnarySyntax         (100, Opcode (0x9B, 0xDD, 0x06), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FNSAVE",
			new UnarySyntax         (100, Opcode (0xDD, 0x06),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FSCALE",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFD),       Syntax::NOTHING)),

		Instruction ("FSIN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFE),       Syntax::NOTHING)),

		Instruction ("FSINCOS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFB),       Syntax::NOTHING)),

		Instruction ("FSQRT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFA),       Syntax::NOTHING)),

		Instruction ("FST",
			new UnarySyntax         (100, Opcode (0xD9, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x02),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xD0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		Instruction ("FSTP",
			new UnarySyntax         (100, Opcode (0xD9, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x03),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new UnarySyntax         (120, Opcode (0xDB, 0x07),       Syntax::NOTHING,                                                          new Mem<80, Memory::FLOAT>()),
			new AdditiveUnarySyntax (130, Opcode (0xDD, 0xD8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		Instruction ("FSTCW",
			new UnarySyntax         (100, Opcode (0x9B, 0xD9, 0x07), Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FNSTCW",
			new UnarySyntax         (100, Opcode (0xD9, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FSTENV",
			new UnarySyntax         (100, Opcode (0x9B, 0xD9, 0x06), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FNSTENV",
			new UnarySyntax         (100, Opcode (0xD9, 0x06),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FSTSW",
			new UnarySyntax         (100, Opcode (0x9B, 0xDD, 0x07), Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new ZerarySyntax        (110, Opcode (0x9B, 0xDF, 0XE0), Syntax::NOTHING)),

		Instruction ("FNSTSW",
			new UnarySyntax         (100, Opcode (0xDD, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new ZerarySyntax        (110, Opcode (0xDF, 0XE0),       Syntax::NOTHING)),

		Instruction ("FSUB",
			new UnarySyntax         (100, Opcode (0xD8, 0x04),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x04),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xE0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xE8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		Instruction ("FSUBP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xE9),       Syntax::NOTHING)),

		Instruction ("FISUB",
			new UnarySyntax         (100, Opcode (0xDA, 0x04),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x04),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FSUBR",
			new UnarySyntax         (100, Opcode (0xD8, 0x05),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x05),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xE0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		Instruction ("FSUBRP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xE0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xE1),       Syntax::NOTHING)),

		Instruction ("FISUBR",
			new UnarySyntax         (100, Opcode (0xDA, 0x05),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x05),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		Instruction ("FTST",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE4),       Syntax::NOTHING)),

		Instruction ("FUCOM",
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xE0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xDD, 0xE1),       Syntax::NOTHING)),

		Instruction ("FUCOMP",
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xE8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xDD, 0xE9),       Syntax::NOTHING)),

		Instruction ("FUCOMPP",
			new ZerarySyntax        (100, Opcode (0xDA, 0xE9),       Syntax::NOTHING)),

		Instruction ("FWAIT",
			new ZerarySyntax        (100, Opcode (0x9B),             Syntax::NOTHING)),

		Instruction ("FXAM",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE5),       Syntax::NOTHING)),

		Instruction ("FXCH",
			new AdditiveUnarySyntax (120, Opcode (0xD9, 0xC8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD9, 0xc9),       Syntax::NOTHING)),

		Instruction ("FXRSTOR",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x01), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FXSAVE",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x00), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		Instruction ("FXTRACT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF4),       Syntax::NOTHING)),

		Instruction ("FYL2X",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF1),       Syntax::NOTHING)),

		Instruction ("FYL2XP1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF9),       Syntax::NOTHING)),

		Instruction ("HLT",
			new ZerarySyntax        (100, Opcode (0xF4),             Syntax::NOTHING)),

		Instruction ("IDIV",
			new UnarySyntax         (100, Opcode (0xF6, 0x07),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1)),

		Instruction ("IMUL",
			new UnarySyntax         (100, Opcode (0xF6, 0x05),       Syntax::FIRST_ARGUMENT,                                                           new MemGPReg(), 1),
			new BinarySyntax        (110, Opcode (0x0F, 0xAF),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new WordReg(),                     new MemWordReg()),
			new BinarySyntax        (120, Opcode (0x6B),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new WordReg(),                     new WordReg(),  new Immed<8, Number::SIGNED>()),
			new BinarySyntax        (125, Opcode (0x6B),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::REPEATED, new WordReg(),                     new Immed<8, Number::SIGNED>()),
			new BinarySyntax        (130, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new GPRegister16Bits::IdFunctor(), new MemReg16(), new Immed<16, Number::SIGNED>()),
			new BinarySyntax        (135, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::REPEATED, new GPRegister16Bits::IdFunctor(), new Immed<16, Number::UNSIGNED>()),
			new BinarySyntax        (140, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new GPRegister32Bits::IdFunctor(), new MemReg32(), new Immed<32, Number::SIGNED>()),
			new BinarySyntax        (145, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::REPEATED, new GPRegister32Bits::IdFunctor(), new Immed<32, Number::SIGNED>())),

		Instruction ("IN",
			new BinarySyntax        (100, Opcode (0xE4),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::ABSENT,  new Accumulator(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (110, Opcode (0xEC),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::ABSENT,  new Accumulator(), new DX())),

		Instruction ("INC",
			new UnarySyntax         (100, Opcode (0xFE, 0x00),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1),
			new AdditiveUnarySyntax (110, Opcode (0x40),             Syntax::FIRST_ARGUMENT,                                                   new WordReg())),

		Instruction ("INS",
			new StringSyntax        (100, Opcode (0x6C),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<8>(),     new DX(), 2),
			new StringSyntax        (110, Opcode (0x6D),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<16>(),    new DX(), 2),
			new StringSyntax        (120, Opcode (0x6D),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<32>(),    new DX(), 2)),

		Instruction ("INSB",
			new ZerarySyntax        (100, Opcode (0x6C),             Syntax::NOTHING)),

		Instruction ("INSW",
			new ZerarySyntax        (100, Opcode (0x6D),             Syntax::MODE_16BITS)),

		Instruction ("INSD",
			new ZerarySyntax        (100, Opcode (0x6D),             Syntax::MODE_32BITS)),

		Instruction ("INT",
			new UnarySyntax         (100, Opcode (0xCD),             Syntax::NOTHING,                                                          new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax        (110, Opcode (0xCC),             Syntax::NOTHING,                                                          new ImmedEqual<3>)),

		Instruction ("INTO",
			new ZerarySyntax        (100, Opcode (0xCE),             Syntax::NOTHING)),

		Instruction ("INVD",
			new ZerarySyntax        (100, Opcode (0x0F, 0x08),       Syntax::NOTHING)),

		Instruction ("INVLPG",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x07), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("IRET",
			new ZerarySyntax        (100, Opcode (0xCF),             Syntax::NOTHING)),

		Instruction ("IRETW",
			new ZerarySyntax        (100, Opcode (0xCF),             Syntax::MODE_16BITS)),

		Instruction ("IRETD",
			new ZerarySyntax        (100, Opcode (0xCF),             Syntax::MODE_32BITS)),

		Instruction ("JA",
			new RelativeUnarySyntax (10100, Opcode (0x77),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x87),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JAE",
			new RelativeUnarySyntax (10100, Opcode (0x73),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x83),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JB",
			new RelativeUnarySyntax (10100, Opcode (0x72),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x82),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JBE",
			new RelativeUnarySyntax (10100, Opcode (0x76),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x86),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JC",
			new RelativeUnarySyntax (10100, Opcode (0x72),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x82),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

  		Instruction ("JCXZ",
  			new RelativeUnarySyntax (100, Opcode (0xE3),             Syntax::MODE_16BITS,                                                             new RelativeArgument (Type::SHORT))),

  		Instruction ("JECXZ",
  			new RelativeUnarySyntax (100, Opcode (0xE3),             Syntax::MODE_32BITS,                                                             new RelativeArgument (Type::SHORT))),

		Instruction ("JE",
			new RelativeUnarySyntax (10100, Opcode (0x74),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x84),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JG",
			new RelativeUnarySyntax (10100, Opcode (0x7F),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8F),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JGE",
			new RelativeUnarySyntax (10100, Opcode (0x7D),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8D),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JL",
			new RelativeUnarySyntax (10100, Opcode (0x7C),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8C),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JLE",
			new RelativeUnarySyntax (10100, Opcode (0x7E),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8E),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNA",
			new RelativeUnarySyntax (10100, Opcode (0x76),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x86),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNAE",
			new RelativeUnarySyntax (10100, Opcode (0x72),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x82),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNB",
			new RelativeUnarySyntax (10100, Opcode (0x73),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x83),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNBE",
			new RelativeUnarySyntax (10100, Opcode (0x77),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x87),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNC",
			new RelativeUnarySyntax (10100, Opcode (0x73),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x83),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNE",
			new RelativeUnarySyntax (10100, Opcode (0x75),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x85),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNG",
			new RelativeUnarySyntax (10100, Opcode (0x7E),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8E),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNGE",
			new RelativeUnarySyntax (10100, Opcode (0x7C),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8C),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNL",
			new RelativeUnarySyntax (10100, Opcode (0x7D),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8D),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNLE",
			new RelativeUnarySyntax (10100, Opcode (0x7F),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8F),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNO",
			new RelativeUnarySyntax (10100, Opcode (0x71),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x81),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNP",
			new RelativeUnarySyntax (10100, Opcode (0x7B),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8B),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNS",
			new RelativeUnarySyntax (10100, Opcode (0x79),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x89),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JNZ",
			new RelativeUnarySyntax (10100, Opcode (0x75),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x85),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JO",
			new RelativeUnarySyntax (10100, Opcode (0x70),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x80),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JP",
			new RelativeUnarySyntax (10100, Opcode (0x7A),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8A),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JPE",
			new RelativeUnarySyntax (10100, Opcode (0x7A),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8A),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JPO",
			new RelativeUnarySyntax (10100, Opcode (0x7B),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8B),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JS",
			new RelativeUnarySyntax (10100, Opcode (0x78),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x88),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JZ",
			new RelativeUnarySyntax (10100, Opcode (0x74),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x84),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		Instruction ("JMP",
			new UnarySyntax         (100,   Opcode (0xFF, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new WordRegNearMem()),
			new UnarySyntax         (110,   Opcode (0xFF, 0x05),       Syntax::FULL_POINTER,                                                            new Mem<0, Memory::INTEGER, Type::FAR>()),
			new RelativeUnarySyntax (10125, Opcode (0xE8),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (120,   Opcode (0xE9),             Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR)),
			new UnarySyntax         (130,   Opcode (0xEA),             Syntax::FIRST_ARGUMENT,                                                          new FullPointer::IdFunctor())),

		Instruction ("LAHF",
			new ZerarySyntax        (100, Opcode (0x9F),             Syntax::NOTHING)),

		Instruction ("LAR",
			new BinarySyntax        (100, Opcode (0x0F, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		Instruction ("LDMXCSR",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x02), Syntax::NOTHING,                                                                 new GPRegister32Bits::IdFunctor())),

		Instruction ("LFENCE",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAE, 0xE8), Syntax::NOTHING)),

		Instruction ("LDS",
			new BinarySyntax        (100, Opcode (0xC5),             Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("LES",
			new BinarySyntax        (100, Opcode (0xC4),             Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("LFS",
			new BinarySyntax        (100, Opcode (0x0F, 0xB4),       Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("LGS",
			new BinarySyntax        (100, Opcode (0x0F, 0xB5),       Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("LSS",
			new BinarySyntax        (100, Opcode (0x0F, 0xB2),       Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("LEA",
			new BinarySyntax        (100, Opcode (0x8D),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		Instruction ("LEAVE",
			new ZerarySyntax        (100, Opcode (0xC9),             Syntax::NOTHING)),

		Instruction ("LGDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x02), Syntax::NOTHING,                                                                 new Mem<48>())),

		Instruction ("LIDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x03), Syntax::NOTHING,                                                                 new Mem<48>())),

		Instruction ("LLDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x02), Syntax::NOTHING,                                                                 new MemReg16())),

		Instruction ("LMSW",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x06), Syntax::NOTHING,                                                                 new MemReg16())),

		Instruction ("LOCK",
			new ZerarySyntax        (100, Opcode (0xF0),             Syntax::NOTHING)),

		Instruction ("LODS",
			new StringSyntax        (100, Opcode (0xAC),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<8>(),  0, 0),
			new StringSyntax        (110, Opcode (0xAD),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<16>(), 0, 0),
			new StringSyntax        (120, Opcode (0xAD),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<32>(), 0, 0)),

		Instruction ("LODSB",
			new ZerarySyntax        (100, Opcode (0xAC),             Syntax::NOTHING)),

		Instruction ("LODSW",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_16BITS)),

		Instruction ("LODSD",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_32BITS)),

		Instruction ("LOOP",
			new RelativeUnarySyntax (100, Opcode (0xE2),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		Instruction ("LOOPE",
			new RelativeUnarySyntax (100, Opcode (0xE1),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		Instruction ("LOOPZ",
			new RelativeUnarySyntax (100, Opcode (0xE1),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		Instruction ("LOOPNE",
			new RelativeUnarySyntax (100, Opcode (0xE0),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		Instruction ("LOOPNZ",
			new RelativeUnarySyntax (100, Opcode (0xE0),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		Instruction ("LSL",
			new BinarySyntax        (100, Opcode (0x0F, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		Instruction ("LTR",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x03), Syntax::NOTHING,                                                                 new MemReg16())),

		Instruction ("MASKMOVQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xF7),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MMXRegister::IdFunctor())),

		Instruction ("MASKMOVDQU",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xF7), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("MAXPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("MAXPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5F),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("MAXSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5F), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("MAXSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5F), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("MFENCE",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAE, 0xF0), Syntax::NOTHING)),

		Instruction ("MINPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5D), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("MINPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5D),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("MINSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("MINSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("MOV",
			new BinarySyntax        (100, Opcode (0x88),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),      new MemGPReg()),
			new BinarySyntax        (110, Opcode (0x8C),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemWordReg(),                 new SegmentRegister::IdFunctor()),
			new BinarySyntax        (120, Opcode (0x8E),             Syntax::SECOND_ARGUMENT,false, Argument::NONE,         0, BinarySyntax::PRESENT, new SegmentRegister::IdFunctor(), new MemWordReg()),
			new BinarySyntax        (130, Opcode (0xA0),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,        0, BinarySyntax::ABSENT,  new Accumulator(),                new Memory::DirectMemory()),
			new BinarySyntax        (140, Opcode (0xC6, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,	       1, BinarySyntax::PARTIAL, new MemGPReg(),                   new Immediate::IdFunctor()),
			new AdditiveUnarySyntax (150, Opcode (0xB0),             Syntax::FIRST_ARGUMENT,                                                          new GPRegister8Bits::IdFunctor(),  new Immed<8, Number::ANY>()),
			new AdditiveUnarySyntax (160, Opcode (0xB8),             Syntax::FIRST_ARGUMENT,                                                          new GPRegister16Bits::IdFunctor(), new Immed<16, Number::ANY>()),
			new AdditiveUnarySyntax (170, Opcode (0xB8),             Syntax::FIRST_ARGUMENT,                                                          new GPRegister32Bits::IdFunctor(), new Immed<32, Number::ANY>()),
			new BinarySyntax        (180, Opcode (0x0F, 0x20),       Syntax::NOTHING,        true,  Argument::NONE,         2, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new ControlRegister::IdFunctor()),
			new BinarySyntax        (190, Opcode (0x0F, 0x21),       Syntax::NOTHING,        true,  Argument::NONE,         2, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new DebugRegister::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x0F, 0x24),       Syntax::NOTHING,        true,  Argument::NONE,         2, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new TestRegister::IdFunctor())),

		Instruction ("MOVAPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x28), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x29), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVAPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x28),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x29),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVD",
			new BinarySyntax        (100, Opcode (0x0F, 0x6E),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemReg32()),
			new BinarySyntax        (110, Opcode (0x0F, 0x7E),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemReg32(),               new MMXRegister::IdFunctor())),

		Instruction ("MOVDQA",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x6F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x7F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVDQU",
			new BinarySyntax        (110, Opcode (0xF3, 0x0F, 0x6F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x7F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVDQ2Q",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0xD6), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("MOVHLPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x12),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("MOVHPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x16), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x17), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVHPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x16),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x17),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVLHPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x16),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("MOVLPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x12), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x13), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVLPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x12),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x13),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVMSKPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x50), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("MOVMSKPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x50),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("MOVNTDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xE7), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Mem<128, Memory::FLOAT>(),     new XMMRegister::IdFunctor())),

		Instruction ("MOVNTI",
			new BinarySyntax        (100, Opcode (0x0F, 0xC3),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<32, Memory::INTEGER>(),    new GPRegister32Bits::IdFunctor())),

		Instruction ("MOVNTPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2B), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Mem<128, Memory::FLOAT>(),     new XMMRegister::IdFunctor())),

		Instruction ("MOVNTPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2B),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Mem<128, Memory::FLOAT>(),     new XMMRegister::IdFunctor())),

		Instruction ("MOVNTQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xE7),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Memory::IdFunctor(),      new MMXRegister::IdFunctor())),

		Instruction ("MOVQ",
			new BinarySyntax        (100, Opcode (0x0F, 0x6F),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x7F),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemMMXReg(),              new MMXRegister::IdFunctor())),

		Instruction ("MOVQ2DQ",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0xD6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MMXRegister::IdFunctor())),

		Instruction ("MOVS",
			new StringSyntax        (100, Opcode (0xAC),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<8>(),     new Mem<8>(), 0),
			new StringSyntax        (110, Opcode (0xAD),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<16>(),    new Mem<16>(), 0),
			new StringSyntax        (120, Opcode (0xAD),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<32>(),    new Mem<32>(), 0)),

		Instruction ("MOVSB",
			new ZerarySyntax        (100, Opcode (0xAC),             Syntax::NOTHING)),

		Instruction ("MOVSW",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_16BITS)),

		Instruction ("MOVSD",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_32BITS)),

		Instruction ("MOVSS",
			new BinarySyntax        (110, Opcode (0xF3, 0x0F, 0x10), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x11), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemXMMReg<32, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVUPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x10), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x11), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemXMMReg<32, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVUPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x10),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x11),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemXMMReg<32, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		Instruction ("MOVSX",
			new BinarySyntax        (100, Opcode (0x0F, 0xBE),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new WordReg(),                new Mem8Reg8()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBF),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new Mem16Reg16())),

		Instruction ("MOVZX",
			new BinarySyntax        (100, Opcode (0x0F, 0xB6),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new WordReg(),                new Mem8Reg8()),
			new BinarySyntax        (110, Opcode (0x0F, 0xB7),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new Mem16Reg16())),

		Instruction ("MUL",
			new UnarySyntax         (100, Opcode (0xF6, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(), 1)),

		Instruction ("MULPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x59), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("MULPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x59),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("MULSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x59), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("MULSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x59), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("NEG",
			new UnarySyntax         (100, Opcode (0xF6, 0x03),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(), 1)),

		Instruction ("NOT",
			new UnarySyntax         (100, Opcode (0xF6, 0x02),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(), 1)),

		Instruction ("NOP",
			new ZerarySyntax        (100, Opcode (0x90),             Syntax::NOTHING)),

		Instruction ("OR",
			new BinarySyntax        (110, Opcode (0x08),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x01),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x0C),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x01),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("ORPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x56), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("ORPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x56),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("OUT",
			new ZerarySyntax        (100, Opcode (0xEF),             Syntax::SECOND_ARGUMENT,                                                         new DX(),         new Accumulator()),
			new ZerarySyntax        (110, Opcode (0xEE),             Syntax::NOTHING,                                                                 new DX(),         new AL()),
			new UnarySyntax         (120, Opcode (0xE6),             Syntax::SECOND_ARGUMENT,                                                         new Immed<8, Number::UNSIGNED>(), new Accumulator()),
			new UnarySyntax         (130, Opcode (0xE7),             Syntax::SECOND_ARGUMENT,                                                         new Immed<8, Number::UNSIGNED>(), new AL())),

		Instruction ("OUTS",
			new StringSyntax        (100, Opcode (0x6E),             Syntax::SECOND_ARGUMENT,       Argument::NONE,                                   new DX(),         new Mem<8>(), 1),
			new StringSyntax        (110, Opcode (0x6F),             Syntax::SECOND_ARGUMENT,       Argument::NONE,                                   new DX(),         new Mem<16>(), 1),
			new StringSyntax        (120, Opcode (0x6F),             Syntax::SECOND_ARGUMENT,       Argument::NONE,                                   new DX(),         new Mem<32>(), 1)),

		Instruction ("OUTSB",
			new ZerarySyntax        (100, Opcode (0x6E),             Syntax::NOTHING)),

		Instruction ("OUTSW",
			new ZerarySyntax        (100, Opcode (0x6F),             Syntax::MODE_16BITS)),

		Instruction ("OUTSD",
			new ZerarySyntax        (100, Opcode (0x6F),             Syntax::MODE_32BITS)),

		Instruction ("PACKSSWB",
			new BinarySyntax        (100, Opcode (0x0F, 0x63),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x63), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PACKSSDW",
			new BinarySyntax        (100, Opcode (0x0F, 0x6B),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x6B), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PACKUSWB",
			new BinarySyntax        (100, Opcode (0x0F, 0x67),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x67), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDB",
			new BinarySyntax        (100, Opcode (0x0F, 0xFC),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFC), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDW",
			new BinarySyntax        (100, Opcode (0x0F, 0xFD),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFD), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDD",
			new BinarySyntax        (100, Opcode (0x0F, 0xFE),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFE), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xD4),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD4), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xEC),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEC), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xED),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xED), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDUSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xDC),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDC), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PADDUSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xDD),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDD), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PAND",
			new BinarySyntax        (100, Opcode (0x0F, 0xDB),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDB), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PANDN",
			new BinarySyntax        (100, Opcode (0x0F, 0xDF),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDF), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PAUSE",
			new ZerarySyntax        (100, Opcode (0xF3, 0x90),       Syntax::NOTHING)),

		Instruction ("PAVGB",
			new BinarySyntax        (100, Opcode (0x0F, 0xE0),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE0), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PAVGW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE3),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE3), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PAVGUSB",
			new TDNowSyntax         (100, 0xBF)),

		Instruction ("PCMPEQB",
			new BinarySyntax        (100, Opcode (0x0F, 0x74),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x74), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PCMPEQW",
			new BinarySyntax        (100, Opcode (0x0F, 0x75),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x75), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PCMPEQD",
			new BinarySyntax        (100, Opcode (0x0F, 0x76),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x76), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PCMPGTB",
			new BinarySyntax        (100, Opcode (0x0F, 0x64),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x64), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PCMPGTW",
			new BinarySyntax        (100, Opcode (0x0F, 0x65),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x65), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PCMPGTD",
			new BinarySyntax        (100, Opcode (0x0F, 0x66),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x66), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PEXTRW",
			new BinarySyntax        (100, Opcode (0x0F, 0xC5),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (110, Opcode (0x0F, 0xC5),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PF2ID",
			new TDNowSyntax         (100, 0x1D)),

		Instruction ("PF2IW",
			new TDNowSyntax         (100, 0x1C)),

		Instruction ("PFACC",
			new TDNowSyntax         (100, 0xAE)),

		Instruction ("PFADD",
			new TDNowSyntax         (100, 0x9E)),

		Instruction ("PFCMPEQ",
			new TDNowSyntax         (100, 0xB0)),

		Instruction ("PFCMPGE",
			new TDNowSyntax         (100, 0x90)),

		Instruction ("PFCMPGT",
			new TDNowSyntax         (100, 0xA0)),

		Instruction ("PFMAX",
			new TDNowSyntax         (100, 0xA4)),

		Instruction ("PFMIN",
			new TDNowSyntax         (100, 0x94)),

		Instruction ("PFMUL",
			new TDNowSyntax         (100, 0xB4)),

		Instruction ("PFNACC",
			new TDNowSyntax         (100, 0x8A)),

		Instruction ("PFPNACC",
			new TDNowSyntax         (100, 0x8E)),

		Instruction ("PFRCP",
			new TDNowSyntax         (100, 0x96)),

		Instruction ("PFRCPIT1",
			new TDNowSyntax         (100, 0xA6)),

		Instruction ("PFRCPIT2",
			new TDNowSyntax         (100, 0xB6)),

		Instruction ("PFRSQIT1",
			new TDNowSyntax         (100, 0xA7)),

		Instruction ("PFSUB",
			new TDNowSyntax         (100, 0x9A)),

		Instruction ("PFSUBR",
			new TDNowSyntax         (100, 0xAA)),

		Instruction ("PI2FD",
			new TDNowSyntax         (100, 0x0D)),

		Instruction ("PI2FW",
			new TDNowSyntax         (100, 0x0C)),

		Instruction ("PINSRW",
			new BinarySyntax        (100, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new GPRegister32Bits::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (110, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new Mem<16, Memory::INTEGER>(),    new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new GPRegister32Bits::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<16, Memory::INTEGER>(),    new Immed<8, Number::UNSIGNED>())),

		Instruction ("PMADDWD",
			new BinarySyntax        (100, Opcode (0x0F, 0xF5),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF5), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMAXSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xEE),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEE), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMAXUB",
			new BinarySyntax        (100, Opcode (0x0F, 0xDE),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDE), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMINSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xEA),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEA), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMINUB",
			new BinarySyntax        (100, Opcode (0x0F, 0xDA),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDA), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMOVMSKB",
			new BinarySyntax        (100, Opcode (0x0F, 0xD7),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MMXRegister::IdFunctor()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD7), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor())),

		Instruction ("PMULHRW",
			new TDNowSyntax         (100, 0xB7)),

		Instruction ("PMULHUW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE4),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE4), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMULHW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE5),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE5), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMULLW",
			new BinarySyntax        (100, Opcode (0x0F, 0xD5),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD5), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PMULUDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xF4),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF4), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("POP",
			new UnarySyntax         (100, Opcode (0x8F, 0x00),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg()),
			new AdditiveUnarySyntax (110, Opcode (0x58),             Syntax::FIRST_ARGUMENT,                                                          new WordReg()),
			new UnarySyntax         (120, Opcode (0x1F),             Syntax::NOTHING,                                                                 new DS()),
			new UnarySyntax         (125, Opcode (0x0F),             Syntax::NOTHING,                                                                 new CS()),
			new UnarySyntax         (130, Opcode (0x07),             Syntax::NOTHING,                                                                 new ES()),
			new UnarySyntax         (140, Opcode (0x17),             Syntax::NOTHING,                                                                 new SS()),
			new UnarySyntax         (150, Opcode (0x0F, 0xA1),       Syntax::NOTHING,                                                                 new FS()),
			new UnarySyntax         (160, Opcode (0x0F, 0xA9),       Syntax::NOTHING,                                                                 new GS())),

		Instruction ("POPA",
			new ZerarySyntax        (100, Opcode (0x61),             Syntax::NOTHING)),

		Instruction ("POPAW",
			new ZerarySyntax        (100, Opcode (0x61),             Syntax::MODE_16BITS)),

		Instruction ("POPAD",
			new ZerarySyntax        (100, Opcode (0x61),             Syntax::MODE_32BITS)),

		Instruction ("POPF",
			new ZerarySyntax        (100, Opcode (0x9D),             Syntax::NOTHING)),

		Instruction ("POPFW",
			new ZerarySyntax        (100, Opcode (0x9D),             Syntax::MODE_16BITS)),

		Instruction ("POPFD",
			new ZerarySyntax        (100, Opcode (0x9D),             Syntax::MODE_32BITS)),

		Instruction ("POR",
			new BinarySyntax        (100, Opcode (0x0F, 0xEB),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEB), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PREFETCH",
			new UnarySyntax         (100, Opcode (0x0F, 0x0D, 0x00), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("PREFETCH0",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x01), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("PREFETCH1",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x02), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("PREFETCH2",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x03), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("PREFETCHNTA",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x00), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("PREFETCHW",
			new UnarySyntax         (100, Opcode (0x0F, 0x0D, 0x01), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		Instruction ("PSADBW",
			new BinarySyntax        (100, Opcode (0x0F, 0xF6),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF6), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSHUFD",
			new BinarySyntax        (100, Opcode (0x0F, 0x70),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSHUFHW",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x70), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSHUFLW",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x70), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSHUFW",
			new BinarySyntax        (100, Opcode (0x0F, 0x70),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSLLW",
			new BinarySyntax        (100, Opcode (0x0F, 0xF1),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x71, 0x06), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x71, 0x06), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xF1), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSLLD",
			new BinarySyntax        (100, Opcode (0x0F, 0xF2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x72, 0x06), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x72, 0x06), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xF2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSLLDQ",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x73, 0x07), Syntax::NOTHING,        false, Argument::NONE,   0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSLLQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xF3),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x73, 0x06), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0xF3), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x73, 0x06), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSRAW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE1),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x71, 0x04), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0xE1), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x71, 0x04), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSRAD",
			new BinarySyntax        (100, Opcode (0x0F, 0xE2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x72, 0x04), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0xE2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x72, 0x04), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSRLW",
			new BinarySyntax        (100, Opcode (0x0F, 0xD1),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x71, 0x02), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x71, 0x02), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xD1), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSRLD",
			new BinarySyntax        (100, Opcode (0x0F, 0xD2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x72, 0x02), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x72, 0x02), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xD2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSRLDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x73, 0x03), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("PSRLQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xD3),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x73, 0x02), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x73, 0x02), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xD3), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBB",
			new BinarySyntax        (100, Opcode (0x0F, 0xF8),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF8), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBW",
			new BinarySyntax        (100, Opcode (0x0F, 0xF9),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF9), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBD",
			new BinarySyntax        (100, Opcode (0x0F, 0xFA),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFA), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xFB),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFB), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xE8),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE8), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE9),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE9), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBUSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xD8),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD8), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSUBUSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xD9),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD9), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PSWAPD",
			new TDNowSyntax         (100, 0xBB)),

		Instruction ("PUNPCKHBW",
			new BinarySyntax        (100, Opcode (0x0F, 0x68),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x68), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKHWD",
			new BinarySyntax        (100, Opcode (0x0F, 0x69),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x69), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKHDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0x6A),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x6A), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKHQDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x6D), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKLBW",
			new BinarySyntax        (100, Opcode (0x0F, 0x60),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x60), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKLWD",
			new BinarySyntax        (100, Opcode (0x0F, 0x61),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x61), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKLDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0x62),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x62), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUNPCKLQDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x6C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("PUSH",
			new UnarySyntax         (100, Opcode (0xFF, 0x06),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg()),
			new UnarySyntax         (110, Opcode (0x50),             Syntax::FIRST_ARGUMENT,                                                          new WordReg()),
			new UnarySyntax         (115, Opcode (0x68),             Syntax::FIRST_ARGUMENT,                                                          new DefaultImmed()),
			new UnarySyntax         (120, Opcode (0x1E),             Syntax::NOTHING,                                                                 new DS()),
			new UnarySyntax         (125, Opcode (0x0E),             Syntax::NOTHING,                                                                 new CS()),
			new UnarySyntax         (130, Opcode (0x06),             Syntax::NOTHING,                                                                 new ES()),
			new UnarySyntax         (140, Opcode (0x16),             Syntax::NOTHING,                                                                 new SS()),
			new UnarySyntax         (150, Opcode (0x0F, 0xA0),       Syntax::NOTHING,                                                                 new FS()),
			new UnarySyntax         (160, Opcode (0x0F, 0xA8),       Syntax::NOTHING,                                                                 new GS()),
			new UnarySyntax         (170, Opcode (0x6A),             Syntax::NOTHING,                                                                 new Immed<8, Number::SIGNED, false>())),

		Instruction ("PUSHA",
			new ZerarySyntax        (100, Opcode (0x60),             Syntax::NOTHING)),

		Instruction ("PUSHAW",
			new ZerarySyntax        (100, Opcode (0x60),             Syntax::MODE_16BITS)),

		Instruction ("PUSHAD",
			new ZerarySyntax        (100, Opcode (0x60),             Syntax::MODE_32BITS)),

		Instruction ("PUSHF",
			new ZerarySyntax        (100, Opcode (0x9C),             Syntax::NOTHING)),

		Instruction ("PUSHFW",
			new ZerarySyntax        (100, Opcode (0x9C),             Syntax::MODE_16BITS)),

		Instruction ("PUSHFD",
			new ZerarySyntax        (100, Opcode (0x9C),             Syntax::MODE_32BITS)),

		Instruction ("PXOR",
			new BinarySyntax        (100, Opcode (0x0F, 0xEF),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xEF), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("RCL",
			new UnarySyntax         (120, Opcode (0xD0, 0x02),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("RCR",
			new UnarySyntax         (120, Opcode (0xD0, 0x03),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("ROL",
			new UnarySyntax         (120, Opcode (0xD0, 0x00),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("ROR",
			new UnarySyntax         (120, Opcode (0xD0, 0x01),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x01),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x01),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("RCPPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x53),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("RCPSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x53), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("RDMSR",
			new ZerarySyntax        (100, Opcode (0x0F, 0x32),       Syntax::NOTHING)),

		Instruction ("RDPMC",
			new ZerarySyntax        (100, Opcode (0x0F, 0x33),       Syntax::NOTHING)),

		Instruction ("RDTSC",
			new ZerarySyntax        (100, Opcode (0x0F, 0x31),       Syntax::NOTHING)),

		Instruction ("REP",
			new ZerarySyntax        (100, Opcode (0xF3),             Syntax::NOTHING)),

		Instruction ("REPE",
			new ZerarySyntax        (100, Opcode (0xF3),             Syntax::NOTHING)),

		Instruction ("REPZ",
			new ZerarySyntax        (100, Opcode (0xF3),             Syntax::NOTHING)),

		Instruction ("REPNE",
			new ZerarySyntax        (100, Opcode (0xF2),             Syntax::NOTHING)),

		Instruction ("REPNZ",
			new ZerarySyntax        (100, Opcode (0xF2),             Syntax::NOTHING)),

		Instruction ("RSM",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAA),       Syntax::NOTHING)),

		Instruction ("RSQRTPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x52),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("RSQRTSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x52), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("RET",
			new ZerarySyntax        (100, Opcode (0xC3),             Syntax::NOTHING),
			new UnarySyntax         (110, Opcode (0xC2),             Syntax::NOTHING,                                                                 new Immed<16, Number::UNSIGNED>())),

		Instruction ("RETN",
			new ZerarySyntax        (100, Opcode (0xC3),             Syntax::NOTHING),
			new UnarySyntax         (110, Opcode (0xC2),             Syntax::NOTHING,                                                                 new Immed<16, Number::UNSIGNED>())),

		Instruction ("RETF",
			new ZerarySyntax        (100, Opcode (0xCB),             Syntax::NOTHING),
			new UnarySyntax         (110, Opcode (0xCA),             Syntax::NOTHING,                                                                 new Immed<16, Number::UNSIGNED>())),

		Instruction ("SAHF",
			new ZerarySyntax        (100, Opcode (0x9E),             Syntax::NOTHING)),

		Instruction ("SAL",
			new UnarySyntax         (120, Opcode (0xD0, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("SAR",
			new UnarySyntax         (120, Opcode (0xD0, 0x07),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x07),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x07),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("SBB",
			new BinarySyntax        (110, Opcode (0x18),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x1C),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("SCAS",
			new StringSyntax        (100, Opcode (0xAE),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<8>(),  0, 2),
			new StringSyntax        (110, Opcode (0xAF),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<16>(), 0, 2),
			new StringSyntax        (120, Opcode (0xAF),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<32>(), 0, 2)),

		Instruction ("SCASB",
			new ZerarySyntax        (100, Opcode (0xAE),             Syntax::NOTHING)),

		Instruction ("SCASW",
			new ZerarySyntax        (100, Opcode (0xAF),             Syntax::MODE_16BITS)),

		Instruction ("SCASD",
			new ZerarySyntax        (100, Opcode (0xAF),             Syntax::MODE_32BITS)),

		Instruction ("SETA",
			new UnarySyntax         (120, Opcode (0x0F, 0x97, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETAE",
			new UnarySyntax         (120, Opcode (0x0F, 0x93, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETB",
			new UnarySyntax         (120, Opcode (0x0F, 0x92, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETBE",
			new UnarySyntax         (120, Opcode (0x0F, 0x96, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETC",
			new UnarySyntax         (120, Opcode (0x0F, 0x92, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETE",
			new UnarySyntax         (120, Opcode (0x0F, 0x94, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETG",
			new UnarySyntax         (120, Opcode (0x0F, 0x9F, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETGE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9D, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETL",
			new UnarySyntax         (120, Opcode (0x0F, 0x9C, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETLE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9E, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNA",
			new UnarySyntax         (120, Opcode (0x0F, 0x96, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNAE",
			new UnarySyntax         (120, Opcode (0x0F, 0x92, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNB",
			new UnarySyntax         (120, Opcode (0x0F, 0x93, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNBE",
			new UnarySyntax         (120, Opcode (0x0F, 0x97, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNC",
			new UnarySyntax         (120, Opcode (0x0F, 0x93, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNE",
			new UnarySyntax         (120, Opcode (0x0F, 0x95, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNG",
			new UnarySyntax         (120, Opcode (0x0F, 0x9E, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNGE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9C, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNL",
			new UnarySyntax         (120, Opcode (0x0F, 0x9D, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNLE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9F, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNO",
			new UnarySyntax         (120, Opcode (0x0F, 0x91, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNP",
			new UnarySyntax         (120, Opcode (0x0F, 0x9B, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNS",
			new UnarySyntax         (120, Opcode (0x0F, 0x99, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETNZ",
			new UnarySyntax         (120, Opcode (0x0F, 0x95, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETO",
			new UnarySyntax         (120, Opcode (0x0F, 0x90, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETP",
			new UnarySyntax         (120, Opcode (0x0F, 0x9A, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETPE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9A, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETPO",
			new UnarySyntax         (120, Opcode (0x0F, 0x9B, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETS",
			new UnarySyntax         (120, Opcode (0x0F, 0x98, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SETZ",
			new UnarySyntax         (120, Opcode (0x0F, 0x94, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		Instruction ("SFENCE",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAE, 0xF8), Syntax::NOTHING)),

		Instruction ("SGDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x00), Syntax::FIRST_ARGUMENT,                                                          new Mem<48>())),

		Instruction ("SHL",
			new UnarySyntax         (120, Opcode (0xD0, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHLD",
			new BinarySyntax        (100, Opcode (0x0F, 0xA4),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new CL()),
			new BinarySyntax        (110, Opcode (0x0F, 0xA5),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHR",
			new UnarySyntax         (120, Opcode (0xD0, 0x05),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x05),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x05),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHRD",
			new BinarySyntax        (100, Opcode (0x0F, 0xAC),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new CL()),
			new BinarySyntax        (110, Opcode (0x0F, 0xAD),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHUFPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xC6), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("SHUFPS",
			new BinarySyntax        (100, Opcode (0x0F, 0xC6),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		Instruction ("SIDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x01), Syntax::FIRST_ARGUMENT,                                                          new Mem<48>())),

		Instruction ("SLDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x00), Syntax::FIRST_ARGUMENT,                                                          new MemWordReg())),

		Instruction ("SQRTPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x51), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("SQRTPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x51),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("SQRTSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x51), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("SQRTSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x51), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("STC",
			new ZerarySyntax        (100, Opcode (0xF9),             Syntax::NOTHING)),

		Instruction ("STD",
			new ZerarySyntax        (100, Opcode (0xFD),             Syntax::NOTHING)),

		Instruction ("STI",
			new ZerarySyntax        (100, Opcode (0xFB),             Syntax::NOTHING)),

		Instruction ("STMXCSR",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x03), Syntax::NOTHING,                                                                 new GPRegister32Bits::IdFunctor())),

		Instruction ("STOS",
			new StringSyntax        (100, Opcode (0xAA),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<8>(),  0, 2),
			new StringSyntax        (110, Opcode (0xAB),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<16>(), 0, 2),
			new StringSyntax        (120, Opcode (0xAB),             Syntax::NOTHING,               Argument::NONE,                                   new Mem<32>(), 0, 2)),

		Instruction ("STOSB",
			new ZerarySyntax        (100, Opcode (0xAA),             Syntax::NOTHING)),

		Instruction ("STOSW",
			new ZerarySyntax        (100, Opcode (0xAB),             Syntax::MODE_16BITS)),

		Instruction ("STOSD",
			new ZerarySyntax        (100, Opcode (0xAB),             Syntax::MODE_32BITS)),

		Instruction ("STR",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x01), Syntax::FIRST_ARGUMENT,                                                          new MemWordReg())),

		Instruction ("SUB",
			new BinarySyntax        (110, Opcode (0x28),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x05),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x2C),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x05),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,      1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("SUBPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("SUBPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5C),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("SUBSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("SUBSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("SYSENTER",
			new ZerarySyntax        (100, Opcode (0x0F, 0x34),       Syntax::NOTHING)),

		Instruction ("SYSEXIT",
			new ZerarySyntax        (100, Opcode (0x0F, 0x35),       Syntax::NOTHING)),

		Instruction ("TEST",
			new BinarySyntax        (100, Opcode (0xF6, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,	       1, BinarySyntax::PARTIAL, new MemGPReg(),    new Immediate::IdFunctor()),
			new BinarySyntax        (110, Opcode (0x84),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemGPReg(),    new GPRegister::IdFunctor()),
			new BinarySyntax        (120, Opcode (0xA8),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(), new Immediate::IdFunctor())),

		Instruction ("UCOMISD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2E), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		Instruction ("UCOMISS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2E),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		Instruction ("UD2",
			new ZerarySyntax        (100, Opcode (0x0F, 0x0B),       Syntax::NOTHING)),

		Instruction ("UNPCKHPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x15), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("UNPCKHPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x15),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("UNPCKLPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x14), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("UNPCKLPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x14),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("VERR",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x04), Syntax::NOTHING,                                                                 new MemWordReg())),

		Instruction ("VERW",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x05), Syntax::NOTHING,                                                                 new MemWordReg())),

		Instruction ("WAIT",
			new ZerarySyntax        (100, Opcode (0x9),              Syntax::NOTHING)),

		Instruction ("WBINVD",
			new ZerarySyntax        (100, Opcode (0x0F, 0x09),       Syntax::NOTHING)),

		Instruction ("WRMSR",
			new ZerarySyntax        (100, Opcode (0x0F, 0x30),       Syntax::NOTHING)),

		Instruction ("XADD",
			new BinarySyntax        (100, Opcode (0x0F, 0xC0),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemWordReg(), new WordReg())),

		Instruction ("XCHG",
			new BinarySyntax        (100, Opcode (0x86),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemGPReg(),   new GPRegister::IdFunctor()),
			new AdditiveUnarySyntax (100, Opcode (0x90),             Syntax::FIRST_ARGUMENT,                                                          new WordReg(),    new Accumulator())),

		Instruction ("XLAT",
			new StringSyntax        (100, Opcode (0xD7),             Syntax::NOTHING,               Argument::NONE,                                                 new Mem<8>(),     0, 0)),

		Instruction ("XLATB",
			new ZerarySyntax        (100, Opcode (0xD7),             Syntax::NOTHING)),

		Instruction ("XOR",
			new BinarySyntax        (110, Opcode (0x30),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,	       3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()),
			new BinarySyntax        (120, Opcode (0x80, 0x06),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()),
			new BinarySyntax        (200, Opcode (0x34),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()),
			new OptimizedBinarySyntax	(300, Opcode (0x83, 0x06),    Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED, false>())),

		Instruction ("XORPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x57), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		Instruction ("XORPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x57),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),
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

InvalidSyntax::InvalidSyntax (const Command *cmd, const vector<Argument *> args) throw ()
	: BadCommand(cmd), BadArguments(args), WhatString("Invalid combination of arguments for this command.")
{
}

const char *InvalidSyntax::what() const throw ()
{
	return WhatString.c_str();
}
