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

HashTable <Instruction *, HashFunctor, PointerComparator<BasicSymbol> > Instruction::InstructionTable =
	HashTable <Instruction *, HashFunctor, PointerComparator<BasicSymbol> > ();

typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister::IdFunctor>       MemGPReg;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           MMXRegister::IdFunctor>      MemMMXReg;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister16Bits::IdFunctor> MemReg16;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister32Bits::IdFunctor> MemReg32;
typedef BinaryCompose <logical_or<bool>,  GPRegister16Bits::IdFunctor, GPRegister32Bits::IdFunctor> WordReg;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           WordReg>                     MemWordReg;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister8Bits::IdFunctor>  Mem8Reg8;
typedef BinaryCompose <logical_or<bool>,  Memory::IdFunctor,           GPRegister16Bits::IdFunctor> Mem16Reg16;
typedef BinaryCompose <logical_or<bool>,  WordReg,                     Mem<0, Memory::INTEGER, Type::NEAR> > WordRegNearMem;

typedef BinaryCompose <logical_and<bool>, GPRegister::IdFunctor,       Register::CompareCodeFunctor<0> > Accumulator;
typedef BinaryCompose <logical_and<bool>, FPURegister::IdFunctor,      Register::CompareCodeFunctor<0> > ST;
typedef BinaryCompose <logical_and<bool>, GPRegister8Bits::IdFunctor,  Register::CompareCodeFunctor<0> > AL;
typedef BinaryCompose <logical_and<bool>, GPRegister16Bits::IdFunctor, Register::CompareCodeFunctor<0> > AX;
typedef BinaryCompose <logical_and<bool>, GPRegister16Bits::IdFunctor, Register::CompareCodeFunctor<2> > DX;
typedef BinaryCompose <logical_and<bool>, GPRegister8Bits::IdFunctor,  Register::CompareCodeFunctor<1> > CL;

typedef BinaryCompose <logical_and<bool>, SegmentRegister::IdFunctor,  Register::CompareCodeFunctor<0> > ES;
typedef BinaryCompose <logical_and<bool>, SegmentRegister::IdFunctor,  Register::CompareCodeFunctor<1> > CS;
typedef BinaryCompose <logical_and<bool>, SegmentRegister::IdFunctor,  Register::CompareCodeFunctor<2> > SS;
typedef BinaryCompose <logical_and<bool>, SegmentRegister::IdFunctor,  Register::CompareCodeFunctor<3> > DS;
typedef BinaryCompose <logical_and<bool>, SegmentRegister::IdFunctor,  Register::CompareCodeFunctor<4> > FS;
typedef BinaryCompose <logical_and<bool>, SegmentRegister::IdFunctor,  Register::CompareCodeFunctor<5> > GS;

template <unsigned int sz = 0, Memory::ContentsType ct = Memory::FLOAT>
class MemXMMReg : public BinaryCompose <logical_or<bool>,  Mem<sz, ct>, MMXRegister::IdFunctor>
{
};

// Instructions that take two arguments. Any combination of register, memory and immediate are allowed.
// Byte immediate values can be signed extended and accumulators have special optimized opcode.
// Classical examples are ADC, ADD, AND, etc.
class OptimizedBinaryInstruction : public Instruction
{
	public:
	OptimizedBinaryInstruction (const string &nm, const Opcode &Accum, const Opcode &Immed,
		const Opcode &Immed8, const Opcode &RegMem) throw ();
	~OptimizedBinaryInstruction () throw () {}
};

OptimizedBinaryInstruction::OptimizedBinaryInstruction (const string &nm, const Opcode &Accum,
	const Opcode &RawImmed, const Opcode &Immed8, const Opcode &RegMem) throw () : Instruction (nm)
{
	// GPRegister/Memory, GPRegister/Memory 			(except Memory to Memory)
	AddSyntax (new BinarySyntax (110,   RegMem, Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,
		3, BinarySyntax::PRESENT, new GPRegister::IdFunctor(),  new MemGPReg()));

	//	GPRegister/Memory, Immed
	AddSyntax (new BinarySyntax (120, RawImmed,  Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,
		1, BinarySyntax::PARTIAL, new GPRegister::IdFunctor(),  new Immediate::IdFunctor()));

	//	Accum, Immed
	AddSyntax (new BinarySyntax (200,   Accum,  Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,
		1, BinarySyntax::ABSENT,  new Accumulator(),            new Immediate::IdFunctor()));

	// GPRegister/Memory, SignExtend (Byte Immed)
	AddSyntax (new BinarySyntax (10300, Immed8, Syntax::FIRST_ARGUMENT, false, Argument::GREATER,
		1, BinarySyntax::PARTIAL, new MemWordReg(),             new Immed<8, Number::SIGNED>()));
}

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
	static Instruction *Instructions[] =
	{
		new Instruction ("AAA",
			new ZerarySyntax        (100, Opcode (0x37),             Syntax::NOTHING)),

		new Instruction ("AAD",
			new UnarySyntax         (100, Opcode (0xD5),             Syntax::NOTHING, new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax        (110, Opcode (0xD5, 0x0A),       Syntax::NOTHING)),

		new Instruction ("AAM",
			new UnarySyntax         (100, Opcode (0xD4),             Syntax::NOTHING, new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax        (110, Opcode (0xD4, 0x0A),       Syntax::NOTHING)),

		new Instruction ("AAS",
			new ZerarySyntax        (100, Opcode (0x3F),             Syntax::NOTHING)),

		new OptimizedBinaryInstruction ("ADC", Opcode (0x14), Opcode(0x80, 0x02), Opcode(0x83, 0x02), Opcode (0x10)),
		new OptimizedBinaryInstruction ("ADD", Opcode (0x04), Opcode(0x80, 0x00), Opcode(0x83, 0x00), Opcode (0x00)),

		new Instruction ("ADDPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x58), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ADDPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x58),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ADDSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x58), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("ADDSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x58), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new OptimizedBinaryInstruction ("AND", Opcode (0x24), Opcode(0x80, 0x04), Opcode(0x83, 0x04), Opcode (0x20)),

		new Instruction ("ANDPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x54), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ANDPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x54),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ANDNPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x55), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ANDNPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x55),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ARPL",
			new BinarySyntax        (100, Opcode (0x63),             Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemReg16(),   new GPRegister16Bits::IdFunctor())),

		new Instruction ("BOUND",
			new BinarySyntax        (100, Opcode (0x62),             Syntax::FIRST_ARGUMENT, false, Argument::HALF,         0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("BSF",
			new BinarySyntax        (100, Opcode (0x0F, 0xBC),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("BSR",
			new BinarySyntax        (100, Opcode (0x0F, 0xBD),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("BSWAP",
			new AdditiveUnarySyntax (100, Opcode (0x0F, 0xC8),       Syntax::FIRST_ARGUMENT,                                                   new GPRegister32Bits::IdFunctor())),

		new Instruction ("BT",
			new BinarySyntax        (100, Opcode (0x0F, 0xA3),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x04), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("BTC",
			new BinarySyntax        (100, Opcode (0x0F, 0xBB),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x07), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("BTR",
			new BinarySyntax        (100, Opcode (0x0F, 0xB3),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x06), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("BTS",
			new BinarySyntax        (100, Opcode (0x0F, 0xAB),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(), new WordReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBA, 0x05), Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MemWordReg(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("CALL",
			new UnarySyntax         (100, Opcode (0xFF, 0x02),       Syntax::FIRST_ARGUMENT,                                                          new WordRegNearMem()),
			new UnarySyntax         (110, Opcode (0xFF, 0x03),       Syntax::FULL_POINTER,                                                            new Mem<0, Memory::INTEGER, Type::FAR>()),
			new RelativeUnarySyntax (120, Opcode (0xE8),             Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR)),
			new UnarySyntax         (130, Opcode (0x9A),             Syntax::FIRST_ARGUMENT,                                                          new FullPointer::IdFunctor())),

		new Instruction ("CBW",
			new ZerarySyntax        (100, Opcode (0x98),             Syntax::MODE_16BITS)),

		new Instruction ("CDQ",
			new ZerarySyntax        (100, Opcode (0x99),             Syntax::MODE_32BITS)),

		new Instruction ("CLC",
			new ZerarySyntax        (100, Opcode (0xF8),             Syntax::NOTHING)),

		new Instruction ("CLD",
			new ZerarySyntax        (100, Opcode (0xFC),             Syntax::NOTHING)),

		new Instruction ("CLFLUSH",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x07), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("CLI",
			new ZerarySyntax        (100, Opcode (0xFA),             Syntax::NOTHING)),

		new Instruction ("CLTS",
			new ZerarySyntax        (100, Opcode (0x0F, 0X06),       Syntax::NOTHING)),

		new Instruction ("CMC",
			new ZerarySyntax        (100, Opcode (0xF5),             Syntax::NOTHING)),

		new Instruction ("CMOVA",
			new BinarySyntax        (100, Opcode (0x0F, 0x47),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVAE",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVB",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVBE",
			new BinarySyntax        (100, Opcode (0x0F, 0x46),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVC",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVE",
			new BinarySyntax        (100, Opcode (0x0F, 0x44),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVG",
			new BinarySyntax        (100, Opcode (0x0F, 0x4F),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVGE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4D),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVL",
			new BinarySyntax        (100, Opcode (0x0F, 0x4C),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVLE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4E),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNA",
			new BinarySyntax        (100, Opcode (0x0F, 0x46),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNAE",
			new BinarySyntax        (100, Opcode (0x0F, 0x42),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNB",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNBE",
			new BinarySyntax        (100, Opcode (0x0F, 0x47),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNC",
			new BinarySyntax        (100, Opcode (0x0F, 0x43),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNE",
			new BinarySyntax        (100, Opcode (0x0F, 0x45),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNG",
			new BinarySyntax        (100, Opcode (0x0F, 0x4E),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNGE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4C),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNL",
			new BinarySyntax        (100, Opcode (0x0F, 0x4D),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNLE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4F),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNO",
			new BinarySyntax        (100, Opcode (0x0F, 0x41),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNP",
			new BinarySyntax        (100, Opcode (0x0F, 0x4B),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNS",
			new BinarySyntax        (100, Opcode (0x0F, 0x49),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVNZ",
			new BinarySyntax        (100, Opcode (0x0F, 0x45),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVO",
			new BinarySyntax        (100, Opcode (0x0F, 0x40),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVP",
			new BinarySyntax        (100, Opcode (0x0F, 0x4A),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVPE",
			new BinarySyntax        (100, Opcode (0x0F, 0x4A),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVPO",
			new BinarySyntax        (100, Opcode (0x0F, 0x4B),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVS",
			new BinarySyntax        (100, Opcode (0x0F, 0x48),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("CMOVZ",
			new BinarySyntax        (100, Opcode (0x0F, 0x44),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new OptimizedBinaryInstruction ("CMP", Opcode (0x3C), Opcode(0x80, 0x07), Opcode(0x83, 0x07), Opcode (0x38)),

		new Instruction ("CMPPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("CMPPS",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("CMPS",
			new StringSyntax        (100, Opcode (0xA6),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<8>(),     new Mem<8>()),
			new StringSyntax        (110, Opcode (0xA7),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<16>(),    new Mem<16>()),
			new StringSyntax        (120, Opcode (0xA7),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<32>(),    new Mem<32>())),

		new Instruction ("CMPSB",
			new ZerarySyntax        (100, Opcode (0xA6),             Syntax::NOTHING)),

		new Instruction ("CMPSW",
			new ZerarySyntax        (100, Opcode (0xA7),             Syntax::MODE_16BITS)),

		new Instruction ("CMPSD",
			new ZerarySyntax        (100, Opcode (0xA7),             Syntax::MODE_32BITS),
			new BinarySyntax        (110, Opcode (0xF2, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("CMPSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0xC2), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("CMPXCHG",
			new BinarySyntax        (100, Opcode (0x0F, 0xB0),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemGPReg(),   new GPRegister::IdFunctor())),

		new Instruction ("CMPXCHG8B",
			new UnarySyntax         (100, Opcode (0x0F, 0xC7, 0x01), Syntax::NOTHING,                                                          new Mem<64>())),

		new Instruction ("COMISD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2F), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("COMISS",
			new BinarySyntax        (100, Opcode (0x66, 0x0F),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT,  new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CPUID",
			new ZerarySyntax        (100, Opcode (0x0F, 0xA2),       Syntax::NOTHING)),

		new Instruction ("CVTDQ2PD",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0xE6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::INTEGER>())),

		new Instruction ("CVTDQ2PS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5B),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::INTEGER>())),

		new Instruction ("CVTPD2DQ",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0xE6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("CVTPD2PI",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::INTEGER>())),

		new Instruction ("CVTPD2PS",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("CVTPI2PD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemMMXReg())),

		new Instruction ("CVTPI2PS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2A),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemMMXReg())),

		new Instruction ("CVTPS2DQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5B), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("CVTPS2PD",
			new BinarySyntax        (100, Opcode (0x0F, 0x5A),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CVTPS2PI",
			new BinarySyntax        (100, Opcode (0x0F, 0x2D),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CVTSD2SI",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x2D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CVTSD2SS",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CVTSI2SD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x2A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemReg32)),

		new Instruction ("CVTSI2SS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x2A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemReg32)),

		new Instruction ("CVTSS2SD",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5A), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("CVTSS2SI",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x2D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("CVTTPD2PI",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2C), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("CVTTPD2DQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xE6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("CVTTPS2DQ",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5B), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("CVTTPS2PI",
			new BinarySyntax        (100, Opcode (0x0F, 0x2C),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CVTTSD2SI",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x2C), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("CVTTSS2SI",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x2C), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("CWD",
			new ZerarySyntax        (100, Opcode (0x99),             Syntax::MODE_16BITS)),

		new Instruction ("CWDE",
			new ZerarySyntax        (100, Opcode (0x98),             Syntax::MODE_32BITS)),

		new Instruction ("DAA",
			new ZerarySyntax        (100, Opcode (0x27),             Syntax::NOTHING)),

		new Instruction ("DAS",
			new ZerarySyntax        (100, Opcode (0x2F),             Syntax::NOTHING)),

		new Instruction ("DEC",
			new UnarySyntax         (100, Opcode (0xFE, 0x01),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1),
			new AdditiveUnarySyntax (110, Opcode (0x48),             Syntax::FIRST_ARGUMENT,                                                   new WordReg())),

		new Instruction ("DIV",
			new UnarySyntax         (100, Opcode (0xF6, 0x06),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1)),

		new Instruction ("DIVPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5E), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("DIVPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5E),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("DIVSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5E), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("DIVSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5E), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("EMMS",
			new ZerarySyntax        (100, Opcode (0x0F, 0x77),       Syntax::NOTHING)),

		new Instruction ("ENTER",
			new BinarySyntax        (100, Opcode (0xC8),             Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::ABSENT,  new Immed<16, Number::UNSIGNED>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("F2XM1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF0),       Syntax::NOTHING)),

		new Instruction ("FABS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE1),       Syntax::NOTHING)),

		new Instruction ("FADD",
			new UnarySyntax         (100, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x00),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xC0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FADDP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xC1),       Syntax::NOTHING)),

		new Instruction ("FIADD",
			new UnarySyntax         (100, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xD8, 0x00),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FBLD",
			new UnarySyntax         (100, Opcode (0xDF, 0x04),       Syntax::NOTHING,                                                          new Mem<80, Memory::BCD>())),

		new Instruction ("FBSTP",
			new UnarySyntax         (100, Opcode (0xDF, 0x06),       Syntax::NOTHING,                                                          new Mem<80, Memory::BCD>())),

		new Instruction ("FCHS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE0),       Syntax::NOTHING)),

		new Instruction ("FCLEX",
			new ZerarySyntax        (100, Opcode (0x9B, 0xDB, 0xE2), Syntax::NOTHING)),

		new Instruction ("FNCLEX",
			new ZerarySyntax        (100, Opcode (0xDB, 0xE2),       Syntax::NOTHING)),

		new Instruction ("FCMOVB",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVE",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVBE",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xD0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVU",
			new FPUBinarySyntax     (100, Opcode (0xDA, 0xD8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNB",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xC0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNE",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNBE",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xD0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCMOVNU",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xD8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCOM",
			new UnarySyntax         (100, Opcode (0xD8, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x02),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xD8, 0xD0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD8, 0xD1),       Syntax::NOTHING)),

		new Instruction ("FCOMP",
			new UnarySyntax         (100, Opcode (0xD8, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x03),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xD8, 0xD8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD8, 0xD9),       Syntax::NOTHING)),

		new Instruction ("FCOMPP",
			new ZerarySyntax        (100, Opcode (0xDE, 0xD9),       Syntax::NOTHING)),

		new Instruction ("FCOMI",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCOMIP",
			new FPUBinarySyntax     (100, Opcode (0xDF, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FUCOMI",
			new FPUBinarySyntax     (100, Opcode (0xDB, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FUCOMIP",
			new FPUBinarySyntax     (100, Opcode (0xDF, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor())),

		new Instruction ("FCOS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFF),       Syntax::NOTHING)),

		new Instruction ("FDECSTP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF6),       Syntax::NOTHING)),

		new Instruction ("FDIV",
			new UnarySyntax         (100, Opcode (0xD8, 0x06),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x06),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xF8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FDIVP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xF8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xF9),       Syntax::NOTHING)),

		new Instruction ("FIDIV",
			new UnarySyntax         (100, Opcode (0xDA, 0x06),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x06),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FDIVR",
			new UnarySyntax         (100, Opcode (0xD8, 0x07),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x07),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xF8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xF0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FDIVRP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xF0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xF1),       Syntax::NOTHING)),

		new Instruction ("FIDIVR",
			new UnarySyntax         (100, Opcode (0xDA, 0x07),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FEMMS",
			new ZerarySyntax        (100, Opcode (0x0F, 0x0E),       Syntax::NOTHING)),

		new Instruction ("FFREE",
			new AdditiveUnarySyntax (100, Opcode (0xDD, 0xC0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FICOM",
			new UnarySyntax         (100, Opcode (0xDE, 0x02),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDA, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		new Instruction ("FICOMP",
			new UnarySyntax         (100, Opcode (0xDE, 0x03),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDA, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		new Instruction ("FILD",
			new UnarySyntax         (100, Opcode (0xDF, 0x00),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDB, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (120, Opcode (0xDF, 0x05),       Syntax::NOTHING,                                                          new Mem<64, Memory::INTEGER>())),

		new Instruction ("FINCSTP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF7),       Syntax::NOTHING)),

		new Instruction ("FINIT",
			new ZerarySyntax        (100, Opcode (0x9B, 0xDB, 0xE3), Syntax::NOTHING)),

		new Instruction ("FNINIT",
			new ZerarySyntax        (100, Opcode (0xDB, 0xE3),       Syntax::NOTHING)),

		new Instruction ("FIST",
			new UnarySyntax         (100, Opcode (0xDF, 0x02),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDB, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>())),

		new Instruction ("FISTP",
			new UnarySyntax         (100, Opcode (0xDF, 0x03),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDF, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (120, Opcode (0xDF, 0x07),       Syntax::NOTHING,                                                          new Mem<64, Memory::INTEGER>())),

		new Instruction ("FLD",
			new UnarySyntax         (100, Opcode (0xD9, 0x00),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x00),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new UnarySyntax         (120, Opcode (0xDB, 0x05),       Syntax::NOTHING,                                                          new Mem<80, Memory::FLOAT>()),
			new AdditiveUnarySyntax (130, Opcode (0xD9, 0xC0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FLD1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE8),       Syntax::NOTHING)),

		new Instruction ("FLDL2T",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FLDL2E",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEA),       Syntax::NOTHING)),

		new Instruction ("FLDPI",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEB),       Syntax::NOTHING)),

		new Instruction ("FLDLG2",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEC),       Syntax::NOTHING)),

		new Instruction ("FLDLN2",
			new ZerarySyntax        (100, Opcode (0xD9, 0xED),       Syntax::NOTHING)),

		new Instruction ("FLDZ",
			new ZerarySyntax        (100, Opcode (0xD9, 0xEE),       Syntax::NOTHING)),

		new Instruction ("FLDCW",
			new UnarySyntax         (100, Opcode (0xD9, 0x05),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FLDENV",
			new UnarySyntax         (100, Opcode (0xD9, 0x04),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FMUL",
			new UnarySyntax         (100, Opcode (0xD8, 0x01),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x01),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xC8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FMULP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xC8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xC9),       Syntax::NOTHING)),

		new Instruction ("FIMUL",
			new UnarySyntax         (100, Opcode (0xDA, 0x01),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x01),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FNOP",
			new ZerarySyntax        (100, Opcode (0xD9, 0xD0),       Syntax::NOTHING)),

		new Instruction ("FATAN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF3),       Syntax::NOTHING)),

		new Instruction ("FPREM",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF8),       Syntax::NOTHING)),

		new Instruction ("FPREM1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF5),       Syntax::NOTHING)),

		new Instruction ("FTAN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF2),       Syntax::NOTHING)),

		new Instruction ("FRDINT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFC),       Syntax::NOTHING)),

		new Instruction ("FRSTOR",
			new UnarySyntax         (100, Opcode (0xDD, 0x04),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FSAVE",
			new UnarySyntax         (100, Opcode (0x9B, 0xDD, 0x06), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FNSAVE",
			new UnarySyntax         (100, Opcode (0xDD, 0x06),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FSCALE",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFD),       Syntax::NOTHING)),

		new Instruction ("FSIN",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFE),       Syntax::NOTHING)),

		new Instruction ("FSINCOS",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFB),       Syntax::NOTHING)),

		new Instruction ("FSQRT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xFA),       Syntax::NOTHING)),

		new Instruction ("FST",
			new UnarySyntax         (100, Opcode (0xD9, 0x02),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x02),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xD0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FSTP",
			new UnarySyntax         (100, Opcode (0xD9, 0x03),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDD, 0x03),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new UnarySyntax         (120, Opcode (0xDB, 0x07),       Syntax::NOTHING,                                                          new Mem<80, Memory::FLOAT>()),
			new AdditiveUnarySyntax (130, Opcode (0xDD, 0xD8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor())),

		new Instruction ("FSTCW",
			new UnarySyntax         (100, Opcode (0x9B, 0xD9, 0x07), Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FNSTCW",
			new UnarySyntax         (100, Opcode (0xD9, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FSTENV",
			new UnarySyntax         (100, Opcode (0x9B, 0xD9, 0x06), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FNSTENV",
			new UnarySyntax         (100, Opcode (0xD9, 0x06),       Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FSTSW",
			new UnarySyntax         (100, Opcode (0x9B, 0xDD, 0x07), Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new ZerarySyntax        (110, Opcode (0x9B, 0xDF, 0XE0), Syntax::NOTHING)),

		new Instruction ("FNSTSW",
			new UnarySyntax         (100, Opcode (0xDD, 0x07),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>()),
			new ZerarySyntax        (110, Opcode (0xDF, 0XE0),       Syntax::NOTHING)),

		new Instruction ("FSUB",
			new UnarySyntax         (100, Opcode (0xD8, 0x04),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x04),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xE0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xE8),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FSUBP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FISUB",
			new UnarySyntax         (100, Opcode (0xDA, 0x04),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x04),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FSUBR",
			new UnarySyntax         (100, Opcode (0xD8, 0x05),       Syntax::NOTHING,                                                          new Mem<32, Memory::FLOAT>()),
			new UnarySyntax         (110, Opcode (0xDC, 0x05),       Syntax::NOTHING,                                                          new Mem<64, Memory::FLOAT>()),
			new FPUBinarySyntax     (120, Opcode (0xD8, 0xE8),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new FPUBinarySyntax     (130, Opcode (0xDC, 0xE0),                                                                                 new FPURegister::IdFunctor(), new ST())),

		new Instruction ("FSUBRP",
			new FPUBinarySyntax     (100, Opcode (0xDE, 0xE0),                                                                                 new ST(),                     new FPURegister::IdFunctor()),
			new ZerarySyntax        (110, Opcode (0xDE, 0xE1),       Syntax::NOTHING)),

		new Instruction ("FISUBR",
			new UnarySyntax         (100, Opcode (0xDA, 0x05),       Syntax::NOTHING,                                                          new Mem<32, Memory::INTEGER>()),
			new UnarySyntax         (110, Opcode (0xDE, 0x05),       Syntax::NOTHING,                                                          new Mem<16, Memory::INTEGER>())),

		new Instruction ("FTST",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE4),       Syntax::NOTHING)),

		new Instruction ("FUCOM",
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xE0),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xDD, 0xE1),       Syntax::NOTHING)),

		new Instruction ("FUCOMP",
			new AdditiveUnarySyntax (120, Opcode (0xDD, 0xE8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xDD, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FUCOMPP",
			new ZerarySyntax        (100, Opcode (0xDA, 0xE9),       Syntax::NOTHING)),

		new Instruction ("FWAIT",
			new ZerarySyntax        (100, Opcode (0x9B),             Syntax::NOTHING)),

		new Instruction ("FXAM",
			new ZerarySyntax        (100, Opcode (0xD9, 0xE5),       Syntax::NOTHING)),

		new Instruction ("FXCH",
			new AdditiveUnarySyntax (120, Opcode (0xD9, 0xC8),       Syntax::NOTHING,                                                          new FPURegister::IdFunctor()),
			new ZerarySyntax        (130, Opcode (0xD9, 0xc9),       Syntax::NOTHING)),

		new Instruction ("FXRSTOR",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x01), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FXSAVE",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x00), Syntax::NOTHING,                                                          new Mem<0, Memory::INTEGER>())),

		new Instruction ("FXTRACT",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF4),       Syntax::NOTHING)),

		new Instruction ("FYL2X",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF1),       Syntax::NOTHING)),

		new Instruction ("FYL2XP1",
			new ZerarySyntax        (100, Opcode (0xD9, 0xF9),       Syntax::NOTHING)),

		new Instruction ("HLT",
			new ZerarySyntax        (100, Opcode (0xF4),             Syntax::NOTHING)),

		new Instruction ("IDIV",
			new UnarySyntax         (100, Opcode (0xF6, 0x07),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1)),

		new Instruction ("IMUL",
			new UnarySyntax         (100, Opcode (0xF6, 0x05),       Syntax::FIRST_ARGUMENT,                                                           new MemGPReg(), 1),
			new BinarySyntax        (110, Opcode (0x0F, 0xAF),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new WordReg(),                     new MemWordReg()),
			new BinarySyntax        (120, Opcode (0x6B),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new WordReg(),                     new WordReg(),  new Immed<8, Number::SIGNED>()),
			new BinarySyntax        (125, Opcode (0x6B),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::REPEATED, new WordReg(),                     new Immed<8, Number::SIGNED>()),
			new BinarySyntax        (130, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new GPRegister16Bits::IdFunctor(), new MemReg16(), new Immed<16, Number::SIGNED>()),
			new BinarySyntax        (135, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::REPEATED, new GPRegister16Bits::IdFunctor(), new Immed<16, Number::UNSIGNED>()),
			new BinarySyntax        (140, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT,  new GPRegister32Bits::IdFunctor(), new MemReg32(), new Immed<32, Number::SIGNED>()),
			new BinarySyntax        (145, Opcode (0x69),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::REPEATED, new GPRegister32Bits::IdFunctor(), new Immed<32, Number::SIGNED>())),

		new Instruction ("IN",
			new BinarySyntax        (100, Opcode (0xE4),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::ABSENT,  new Accumulator(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (110, Opcode (0xEC),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::ABSENT,  new Accumulator(), new DX())),

		new Instruction ("INC",
			new UnarySyntax         (100, Opcode (0xFE, 0x00),       Syntax::FIRST_ARGUMENT,                                                   new MemGPReg(), 1),
			new AdditiveUnarySyntax (110, Opcode (0x40),             Syntax::FIRST_ARGUMENT,                                                   new WordReg())),

		new Instruction ("INS",
			new StringSyntax        (100, Opcode (0x6C),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<8>(),     new DX()),
			new StringSyntax        (110, Opcode (0x6D),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<16>(),    new DX()),
			new StringSyntax        (120, Opcode (0x6D),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<32>(),    new DX())),

		new Instruction ("INSB",
			new ZerarySyntax        (100, Opcode (0x6C),             Syntax::NOTHING)),

		new Instruction ("INSW",
			new ZerarySyntax        (100, Opcode (0x6D),             Syntax::MODE_16BITS)),

		new Instruction ("INSD",
			new ZerarySyntax        (100, Opcode (0x6D),             Syntax::MODE_32BITS)),

		new Instruction ("INT",
			new UnarySyntax         (100, Opcode (0xCD),             Syntax::NOTHING,                                                          new Immed<8, Number::UNSIGNED>()),
			new ZerarySyntax        (110, Opcode (0xCC),             Syntax::NOTHING,                                                          new ImmedEqual<3>)),

		new Instruction ("INTO",
			new ZerarySyntax        (100, Opcode (0xCE),             Syntax::NOTHING)),

		new Instruction ("INVD",
			new ZerarySyntax        (100, Opcode (0x0F, 0x08),       Syntax::NOTHING)),

		new Instruction ("INVLPG",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x07), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("IRET",
			new ZerarySyntax        (100, Opcode (0xCF),             Syntax::NOTHING)),

		new Instruction ("IRETW",
			new ZerarySyntax        (100, Opcode (0xCF),             Syntax::MODE_16BITS)),

		new Instruction ("IRETD",
			new ZerarySyntax        (100, Opcode (0xCF),             Syntax::MODE_32BITS)),

		new Instruction ("JA",
			new RelativeUnarySyntax (10100, Opcode (0x77),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x87),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JAE",
			new RelativeUnarySyntax (10100, Opcode (0x73),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x83),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JB",
			new RelativeUnarySyntax (10100, Opcode (0x72),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x82),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JBE",
			new RelativeUnarySyntax (10100, Opcode (0x76),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x86),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JC",
			new RelativeUnarySyntax (10100, Opcode (0x72),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x82),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

  		new Instruction ("JCXZ",
  			new RelativeUnarySyntax (100, Opcode (0xE3),             Syntax::MODE_16BITS,                                                             new RelativeArgument (Type::SHORT))),

  		new Instruction ("JECXZ",
  			new RelativeUnarySyntax (100, Opcode (0xE3),             Syntax::MODE_32BITS,                                                             new RelativeArgument (Type::SHORT))),

		new Instruction ("JE",
			new RelativeUnarySyntax (10100, Opcode (0x74),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x84),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JG",
			new RelativeUnarySyntax (10100, Opcode (0x7F),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8F),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JGE",
			new RelativeUnarySyntax (10100, Opcode (0x7D),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8D),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JL",
			new RelativeUnarySyntax (10100, Opcode (0x7C),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8C),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JLE",
			new RelativeUnarySyntax (10100, Opcode (0x7E),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8E),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNA",
			new RelativeUnarySyntax (10100, Opcode (0x76),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x86),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNAE",
			new RelativeUnarySyntax (10100, Opcode (0x72),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x82),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNB",
			new RelativeUnarySyntax (10100, Opcode (0x73),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x83),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNBE",
			new RelativeUnarySyntax (10100, Opcode (0x77),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x87),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNC",
			new RelativeUnarySyntax (10100, Opcode (0x73),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x83),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNE",
			new RelativeUnarySyntax (10100, Opcode (0x75),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x85),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNG",
			new RelativeUnarySyntax (10100, Opcode (0x7E),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8E),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNGE",
			new RelativeUnarySyntax (10100, Opcode (0x7C),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8C),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNL",
			new RelativeUnarySyntax (10100, Opcode (0x7D),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8D),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNLE",
			new RelativeUnarySyntax (10100, Opcode (0x7F),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8F),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNO",
			new RelativeUnarySyntax (10100, Opcode (0x71),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x81),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNP",
			new RelativeUnarySyntax (10100, Opcode (0x7B),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8B),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNS",
			new RelativeUnarySyntax (10100, Opcode (0x79),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x89),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JNZ",
			new RelativeUnarySyntax (10100, Opcode (0x75),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x85),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JO",
			new RelativeUnarySyntax (10100, Opcode (0x70),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x80),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JP",
			new RelativeUnarySyntax (10100, Opcode (0x7A),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8A),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JPE",
			new RelativeUnarySyntax (10100, Opcode (0x7A),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8A),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JPO",
			new RelativeUnarySyntax (10100, Opcode (0x7B),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x8B),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JS",
			new RelativeUnarySyntax (10100, Opcode (0x78),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x88),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JZ",
			new RelativeUnarySyntax (10100, Opcode (0x74),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (110,   Opcode (0x0F, 0x84),       Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR))),

		new Instruction ("JMP",
			new UnarySyntax         (100,   Opcode (0xFF, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new WordRegNearMem()),
			new UnarySyntax         (110,   Opcode (0xFF, 0x05),       Syntax::FULL_POINTER,                                                            new Mem<0, Memory::INTEGER, Type::FAR>()),
			new RelativeUnarySyntax (10125, Opcode (0xE8),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT)),
			new RelativeUnarySyntax (120,   Opcode (0xE9),             Syntax::FIRST_ARGUMENT,                                                          new RelativeArgument (Type::NEAR)),
			new UnarySyntax         (130,   Opcode (0xEA),             Syntax::FIRST_ARGUMENT,                                                          new FullPointer::IdFunctor())),

		new Instruction ("LAHF",
			new ZerarySyntax        (100, Opcode (0x9F),             Syntax::NOTHING)),

		new Instruction ("LAR",
			new BinarySyntax        (100, Opcode (0x0F, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("LDMXCSR",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x02), Syntax::NOTHING,                                                                 new GPRegister32Bits::IdFunctor())),

		new Instruction ("LFENCE",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAE, 0xE8), Syntax::NOTHING)),

		new Instruction ("LDS",
			new BinarySyntax        (100, Opcode (0xC5),             Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("LES",
			new BinarySyntax        (100, Opcode (0xC4),             Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("LFS",
			new BinarySyntax        (100, Opcode (0x0F, 0xB4),       Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("LGS",
			new BinarySyntax        (100, Opcode (0x0F, 0xB5),       Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("LSS",
			new BinarySyntax        (100, Opcode (0x0F, 0xB2),       Syntax::FIRST_ARGUMENT, false, Argument::MINUS_16BITS, 0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("LEA",
			new BinarySyntax        (100, Opcode (0x8D),             Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new WordReg(),    new Memory::IdFunctor())),

		new Instruction ("LEAVE",
			new ZerarySyntax        (100, Opcode (0xC9),             Syntax::NOTHING)),

		new Instruction ("LGDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x02), Syntax::NOTHING,                                                                 new Mem<48>())),

		new Instruction ("LIDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x03), Syntax::NOTHING,                                                                 new Mem<48>())),

		new Instruction ("LLDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x02), Syntax::NOTHING,                                                                 new MemReg16())),

		new Instruction ("LMSW",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x06), Syntax::NOTHING,                                                                 new MemReg16())),

		new Instruction ("LOCK",
			new ZerarySyntax        (100, Opcode (0xF0),             Syntax::NOTHING)),

		new Instruction ("LODS",
			new StringSyntax        (100, Opcode (0xAC),                                                                                              new Mem<8>(),  true),
			new StringSyntax        (110, Opcode (0xAD),                                                                                              new Mem<16>(), true),
			new StringSyntax        (120, Opcode (0xAD),                                                                                              new Mem<32>(), true)),

		new Instruction ("LODSB",
			new ZerarySyntax        (100, Opcode (0xAC),             Syntax::NOTHING)),

		new Instruction ("LODSW",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_16BITS)),

		new Instruction ("LODSD",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_32BITS)),

		new Instruction ("LOOP",
			new RelativeUnarySyntax (100, Opcode (0xE2),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		new Instruction ("LOOPE",
			new RelativeUnarySyntax (100, Opcode (0xE1),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		new Instruction ("LOOPZ",
			new RelativeUnarySyntax (100, Opcode (0xE1),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		new Instruction ("LOOPNE",
			new RelativeUnarySyntax (100, Opcode (0xE0),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		new Instruction ("LOOPNZ",
			new RelativeUnarySyntax (100, Opcode (0xE0),             Syntax::NOTHING,                                                                 new RelativeArgument (Type::SHORT))),

		new Instruction ("LSL",
			new BinarySyntax        (100, Opcode (0x0F, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new WordReg(),    new MemWordReg())),

		new Instruction ("LTR",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x03), Syntax::NOTHING,                                                                 new MemReg16())),

		new Instruction ("MASKMOVQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xF7),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MMXRegister::IdFunctor())),

		new Instruction ("MASKMOVDQU",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xF7), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("MAXPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("MAXPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5F),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("MAXSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5F), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("MAXSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5F), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("MFENCE",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAE, 0xF0), Syntax::NOTHING)),

		new Instruction ("MINPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5D), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("MINPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5D),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("MINSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("MINSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5D), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("MOV",
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

		new Instruction ("MOVAPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x28), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x29), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVAPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x28),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x29),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVD",
			new BinarySyntax        (100, Opcode (0x0F, 0x6E),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemReg32()),
			new BinarySyntax        (110, Opcode (0x0F, 0x7E),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemReg32(),               new MMXRegister::IdFunctor())),

		new Instruction ("MOVDQA",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x6F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x7F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVDQU",
			new BinarySyntax        (110, Opcode (0xF3, 0x0F, 0x6F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x7F), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemXMMReg<128, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVDQ2Q",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0xD6), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVHLPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x12),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVHPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x16), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x17), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVHPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x16),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x17),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVLHPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x16),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVLPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x12), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x13), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVLPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x12),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<64, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x13),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<64, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVMSKPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x50), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVMSKPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x50),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVNTDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xE7), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Mem<128, Memory::FLOAT>(),     new XMMRegister::IdFunctor())),

		new Instruction ("MOVNTI",
			new BinarySyntax        (100, Opcode (0x0F, 0xC3),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new Mem<32, Memory::INTEGER>(),    new GPRegister32Bits::IdFunctor())),

		new Instruction ("MOVNTPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2B), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Mem<128, Memory::FLOAT>(),     new XMMRegister::IdFunctor())),

		new Instruction ("MOVNTPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2B),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Mem<128, Memory::FLOAT>(),     new XMMRegister::IdFunctor())),

		new Instruction ("MOVNTQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xE7),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new Memory::IdFunctor(),      new MMXRegister::IdFunctor())),

		new Instruction ("MOVQ",
			new BinarySyntax        (100, Opcode (0x0F, 0x6F),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x7F),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemMMXReg(),              new MMXRegister::IdFunctor())),

		new Instruction ("MOVQ2DQ",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0xD6), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MMXRegister::IdFunctor())),

		new Instruction ("MOVS",
			new StringSyntax        (100, Opcode (0xAC),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<8>(),     new Mem<8>()),
			new StringSyntax        (110, Opcode (0xAD),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<16>(),    new Mem<16>()),
			new StringSyntax        (120, Opcode (0xAD),             Syntax::FIRST_ARGUMENT,        Argument::NONE,                                   new Mem<32>(),    new Mem<32>())),

		new Instruction ("MOVSB",
			new ZerarySyntax        (100, Opcode (0xAC),             Syntax::NOTHING)),

		new Instruction ("MOVSW",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_16BITS)),

		new Instruction ("MOVSD",
			new ZerarySyntax        (100, Opcode (0xAD),             Syntax::MODE_32BITS)),

		new Instruction ("MOVSS",
			new BinarySyntax        (110, Opcode (0xF3, 0x0F, 0x10), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x11), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemXMMReg<32, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVUPD",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x10), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x11), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemXMMReg<32, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVUPS",
			new BinarySyntax        (110, Opcode (0x0F, 0x10),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>()),
			new BinarySyntax        (100, Opcode (0x0F, 0x11),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MemXMMReg<32, Memory::FLOAT>(), new XMMRegister::IdFunctor())),

		new Instruction ("MOVSX",
			new BinarySyntax        (100, Opcode (0x0F, 0xBE),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new WordReg(),                new Mem8Reg8()),
			new BinarySyntax        (110, Opcode (0x0F, 0xBF),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new Mem16Reg16())),

		new Instruction ("MOVZX",
			new BinarySyntax        (100, Opcode (0x0F, 0xB6),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new WordReg(),                new Mem8Reg8()),
			new BinarySyntax        (110, Opcode (0x0F, 0xB7),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new Mem16Reg16())),

		new Instruction ("MUL",
			new UnarySyntax         (100, Opcode (0xF6, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(), 1)),

		new Instruction ("MULPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x59), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("MULPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x59),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("MULSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x59), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("MULSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x59), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("NEG",
			new UnarySyntax         (100, Opcode (0xF6, 0x03),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(), 1)),

		new Instruction ("NOT",
			new UnarySyntax         (100, Opcode (0xF6, 0x02),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(), 1)),

		new Instruction ("NOP",
			new ZerarySyntax        (100, Opcode (0x90),             Syntax::NOTHING)),

		new OptimizedBinaryInstruction ("OR",  Opcode (0x0C), Opcode(0x80, 0x01), Opcode(0x83, 0x01), Opcode (0x08)),

		new Instruction ("ORPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x56), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("ORPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x56),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("OUT",
			new ZerarySyntax        (100, Opcode (0xEF),             Syntax::SECOND_ARGUMENT,                                                         new DX(),         new Accumulator()),
			new ZerarySyntax        (110, Opcode (0xEE),             Syntax::NOTHING,                                                                 new DX(),         new AL()),
			new UnarySyntax         (120, Opcode (0xE6),             Syntax::SECOND_ARGUMENT,                                                         new Immed<8, Number::UNSIGNED>(), new Accumulator()),
			new UnarySyntax         (130, Opcode (0xE7),             Syntax::SECOND_ARGUMENT,                                                         new Immed<8, Number::UNSIGNED>(), new AL())),

		new Instruction ("OUTS",
			new StringSyntax        (100, Opcode (0x6E),             Syntax::SECOND_ARGUMENT,       Argument::NONE,                                   new DX(),         new Mem<8>()),
			new StringSyntax        (110, Opcode (0x6F),             Syntax::SECOND_ARGUMENT,       Argument::NONE,                                   new DX(),         new Mem<16>()),
			new StringSyntax        (120, Opcode (0x6F),             Syntax::SECOND_ARGUMENT,       Argument::NONE,                                   new DX(),         new Mem<32>())),

		new Instruction ("OUTSB",
			new ZerarySyntax        (100, Opcode (0x6E),             Syntax::NOTHING)),

		new Instruction ("OUTSW",
			new ZerarySyntax        (100, Opcode (0x6F),             Syntax::MODE_16BITS)),

		new Instruction ("OUTSD",
			new ZerarySyntax        (100, Opcode (0x6F),             Syntax::MODE_32BITS)),

		new Instruction ("PACKSSWB",
			new BinarySyntax        (100, Opcode (0x0F, 0x63),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x63), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PACKSSDW",
			new BinarySyntax        (100, Opcode (0x0F, 0x6B),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x6B), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PACKUSWB",
			new BinarySyntax        (100, Opcode (0x0F, 0x67),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x67), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDB",
			new BinarySyntax        (100, Opcode (0x0F, 0xFC),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFC), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDW",
			new BinarySyntax        (100, Opcode (0x0F, 0xFD),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFD), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDD",
			new BinarySyntax        (100, Opcode (0x0F, 0xFE),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFE), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xD4),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD4), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xEC),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEC), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xED),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xED), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDUSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xDC),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDC), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PADDUSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xDD),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDD), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PAND",
			new BinarySyntax        (100, Opcode (0x0F, 0xDB),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDB), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PANDN",
			new BinarySyntax        (100, Opcode (0x0F, 0xDF),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDF), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PAUSE",
			new ZerarySyntax        (100, Opcode (0xF3, 0x90),       Syntax::NOTHING)),

		new Instruction ("PAVGB",
			new BinarySyntax        (100, Opcode (0x0F, 0xE0),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE0), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PAVGW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE3),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE3), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PAVGUSB",
			new TDNowSyntax         (100, 0xBF)),

		new Instruction ("PCMPEQB",
			new BinarySyntax        (100, Opcode (0x0F, 0x74),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x74), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PCMPEQW",
			new BinarySyntax        (100, Opcode (0x0F, 0x75),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x75), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PCMPEQD",
			new BinarySyntax        (100, Opcode (0x0F, 0x76),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x76), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PCMPGTB",
			new BinarySyntax        (100, Opcode (0x0F, 0x64),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x64), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PCMPGTW",
			new BinarySyntax        (100, Opcode (0x0F, 0x65),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x65), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PCMPGTD",
			new BinarySyntax        (100, Opcode (0x0F, 0x66),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x66), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PEXTRW",
			new BinarySyntax        (100, Opcode (0x0F, 0xC5),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (110, Opcode (0x0F, 0xC5),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PF2ID",
			new TDNowSyntax         (100, 0x1D)),

		new Instruction ("PF2IW",
			new TDNowSyntax         (100, 0x1C)),

		new Instruction ("PFACC",
			new TDNowSyntax         (100, 0xAE)),

		new Instruction ("PFADD",
			new TDNowSyntax         (100, 0x9E)),

		new Instruction ("PFCMPEQ",
			new TDNowSyntax         (100, 0xB0)),

		new Instruction ("PFCMPGE",
			new TDNowSyntax         (100, 0x90)),

		new Instruction ("PFCMPGT",
			new TDNowSyntax         (100, 0xA0)),

		new Instruction ("PFMAX",
			new TDNowSyntax         (100, 0xA4)),

		new Instruction ("PFMIN",
			new TDNowSyntax         (100, 0x94)),

		new Instruction ("PFMUL",
			new TDNowSyntax         (100, 0xB4)),

		new Instruction ("PFNACC",
			new TDNowSyntax         (100, 0x8A)),

		new Instruction ("PFPNACC",
			new TDNowSyntax         (100, 0x8E)),

		new Instruction ("PFRCP",
			new TDNowSyntax         (100, 0x96)),

		new Instruction ("PFRCPIT1",
			new TDNowSyntax         (100, 0xA6)),

		new Instruction ("PFRCPIT2",
			new TDNowSyntax         (100, 0xB6)),

		new Instruction ("PFRSQIT1",
			new TDNowSyntax         (100, 0xA7)),

		new Instruction ("PFSUB",
			new TDNowSyntax         (100, 0x9A)),

		new Instruction ("PFSUBR",
			new TDNowSyntax         (100, 0xAA)),

		new Instruction ("PI2FD",
			new TDNowSyntax         (100, 0x0D)),

		new Instruction ("PI2FW",
			new TDNowSyntax         (100, 0x0C)),

		new Instruction ("PINSRW",
			new BinarySyntax        (100, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new GPRegister32Bits::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (110, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new Mem<16, Memory::INTEGER>(),    new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new GPRegister32Bits::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x0F, 0xC4),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new Mem<16, Memory::INTEGER>(),    new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PMADDWD",
			new BinarySyntax        (100, Opcode (0x0F, 0xF5),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF5), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMAXSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xEE),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEE), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMAXUB",
			new BinarySyntax        (100, Opcode (0x0F, 0xDE),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDE), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMINSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xEA),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEA), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMINUB",
			new BinarySyntax        (100, Opcode (0x0F, 0xDA),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xDA), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMOVMSKB",
			new BinarySyntax        (100, Opcode (0x0F, 0xD7),       Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new MMXRegister::IdFunctor()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD7), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PRESENT, new GPRegister32Bits::IdFunctor(), new XMMRegister::IdFunctor())),

		new Instruction ("PMULHRW",
			new TDNowSyntax         (100, 0xB7)),

		new Instruction ("PMULHUW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE4),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE4), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMULHW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE5),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE5), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMULLW",
			new BinarySyntax        (100, Opcode (0x0F, 0xD5),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD5), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PMULUDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xF4),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF4), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("POP",
			new UnarySyntax         (100, Opcode (0x8F, 0x00),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg()),
			new AdditiveUnarySyntax (110, Opcode (0x58),             Syntax::FIRST_ARGUMENT,                                                          new WordReg()),
			new UnarySyntax         (120, Opcode (0x1F),             Syntax::NOTHING,                                                                 new DS()),
			new UnarySyntax         (125, Opcode (0x0F),             Syntax::NOTHING,                                                                 new CS()),
			new UnarySyntax         (130, Opcode (0x07),             Syntax::NOTHING,                                                                 new ES()),
			new UnarySyntax         (140, Opcode (0x17),             Syntax::NOTHING,                                                                 new SS()),
			new UnarySyntax         (150, Opcode (0x0F, 0xA1),       Syntax::NOTHING,                                                                 new FS()),
			new UnarySyntax         (160, Opcode (0x0F, 0xA9),       Syntax::NOTHING,                                                                 new GS())),

		new Instruction ("POPA",
			new ZerarySyntax        (100, Opcode (0x61),             Syntax::NOTHING)),

		new Instruction ("POPAW",
			new ZerarySyntax        (100, Opcode (0x61),             Syntax::MODE_16BITS)),

		new Instruction ("POPAD",
			new ZerarySyntax        (100, Opcode (0x61),             Syntax::MODE_32BITS)),

		new Instruction ("POPF",
			new ZerarySyntax        (100, Opcode (0x9D),             Syntax::NOTHING)),

		new Instruction ("POPFW",
			new ZerarySyntax        (100, Opcode (0x9D),             Syntax::MODE_16BITS)),

		new Instruction ("POPFD",
			new ZerarySyntax        (100, Opcode (0x9D),             Syntax::MODE_32BITS)),

		new Instruction ("POR",
			new BinarySyntax        (100, Opcode (0x0F, 0xEB),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xEB), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PREFETCH",
			new UnarySyntax         (100, Opcode (0x0F, 0x0D, 0x00), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("PREFETCH0",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x01), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("PREFETCH1",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x02), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("PREFETCH2",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x03), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("PREFETCHNTA",
			new UnarySyntax         (100, Opcode (0x0F, 0x18, 0x00), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("PREFETCHW",
			new UnarySyntax         (100, Opcode (0x0F, 0x0D, 0x01), Syntax::NOTHING,                                                                 new Memory::IdFunctor())),

		new Instruction ("PSADBW",
			new BinarySyntax        (100, Opcode (0x0F, 0xF6),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF6), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSHUFD",
			new BinarySyntax        (100, Opcode (0x0F, 0x70),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSHUFHW",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x70), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSHUFLW",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x70), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSHUFW",
			new BinarySyntax        (100, Opcode (0x0F, 0x70),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSLLW",
			new BinarySyntax        (100, Opcode (0x0F, 0xF1),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x71, 0x06), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x71, 0x06), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xF1), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSLLD",
			new BinarySyntax        (100, Opcode (0x0F, 0xF2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x72, 0x06), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x72, 0x06), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xF2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSLLDQ",
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x73, 0x07), Syntax::NOTHING,        false, Argument::NONE,   0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSLLQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xF3),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x73, 0x06), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0xF3), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x73, 0x06), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSRAW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE1),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x71, 0x04), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0xE1), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x71, 0x04), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSRAD",
			new BinarySyntax        (100, Opcode (0x0F, 0xE2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x72, 0x04), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0xE2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x72, 0x04), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSRLW",
			new BinarySyntax        (100, Opcode (0x0F, 0xD1),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x71, 0x02), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x71, 0x02), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xD1), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSRLD",
			new BinarySyntax        (100, Opcode (0x0F, 0xD2),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x72, 0x02), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (120, Opcode (0x66, 0x0F, 0x72, 0x02), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xD2), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSRLDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x73, 0x03), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("PSRLQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xD3),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x0F, 0x73, 0x02), Syntax::NOTHING,        false, Argument::NONE,         0, BinarySyntax::PARTIAL, new MMXRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0x73, 0x02), Syntax::NOTHING,  false, Argument::NONE,         0, BinarySyntax::PARTIAL, new XMMRegister::IdFunctor(), new Immed<8, Number::UNSIGNED>()),
			new BinarySyntax        (130, Opcode (0x66, 0x0F, 0xD3), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBB",
			new BinarySyntax        (100, Opcode (0x0F, 0xF8),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF8), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBW",
			new BinarySyntax        (100, Opcode (0x0F, 0xF9),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xF9), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBD",
			new BinarySyntax        (100, Opcode (0x0F, 0xFA),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFA), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBQ",
			new BinarySyntax        (100, Opcode (0x0F, 0xFB),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xFB), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xE8),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE8), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xE9),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xE9), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBUSB",
			new BinarySyntax        (100, Opcode (0x0F, 0xD8),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD8), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSUBUSW",
			new BinarySyntax        (100, Opcode (0x0F, 0xD9),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0xD9), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PSWAPD",
			new TDNowSyntax         (100, 0xBB)),

		new Instruction ("PUNPCKHBW",
			new BinarySyntax        (100, Opcode (0x0F, 0x68),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x68), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKHWD",
			new BinarySyntax        (100, Opcode (0x0F, 0x69),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x69), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKHDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0x6A),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x6A), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKHQDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x6D), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKLBW",
			new BinarySyntax        (100, Opcode (0x0F, 0x60),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x60), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKLWD",
			new BinarySyntax        (100, Opcode (0x0F, 0x61),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x61), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKLDQ",
			new BinarySyntax        (100, Opcode (0x0F, 0x62),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (110, Opcode (0x66, 0x0F, 0x62), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUNPCKLQDQ",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x6C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("PUSH",
			new UnarySyntax         (100, Opcode (0xFF, 0x06),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg()),
			new UnarySyntax         (110, Opcode (0x50),             Syntax::FIRST_ARGUMENT,                                                          new WordReg()),
			new UnarySyntax         (115, Opcode (0x68),             Syntax::FIRST_ARGUMENT,                                                          new DefaultImmed()),
			new UnarySyntax         (120, Opcode (0x1E),             Syntax::NOTHING,                                                                 new DS()),
			new UnarySyntax         (125, Opcode (0x0E),             Syntax::NOTHING,                                                                 new CS()),
			new UnarySyntax         (130, Opcode (0x06),             Syntax::NOTHING,                                                                 new ES()),
			new UnarySyntax         (140, Opcode (0x16),             Syntax::NOTHING,                                                                 new SS()),
			new UnarySyntax         (150, Opcode (0x0F, 0xA0),       Syntax::NOTHING,                                                                 new FS()),
			new UnarySyntax         (160, Opcode (0x0F, 0xA8),       Syntax::NOTHING,                                                                 new GS()),
			new UnarySyntax         (170, Opcode (0x6A),             Syntax::NOTHING,                                                                 new Immed<8, Number::SIGNED>())),

		new Instruction ("PUSHA",
			new ZerarySyntax        (100, Opcode (0x60),             Syntax::NOTHING)),

		new Instruction ("PUSHAW",
			new ZerarySyntax        (100, Opcode (0x60),             Syntax::MODE_16BITS)),

		new Instruction ("PUSHAD",
			new ZerarySyntax        (100, Opcode (0x60),             Syntax::MODE_32BITS)),

		new Instruction ("PUSHF",
			new ZerarySyntax        (100, Opcode (0x9C),             Syntax::NOTHING)),

		new Instruction ("PUSHFW",
			new ZerarySyntax        (100, Opcode (0x9C),             Syntax::MODE_16BITS)),

		new Instruction ("PUSHFD",
			new ZerarySyntax        (100, Opcode (0x9C),             Syntax::MODE_32BITS)),

		new Instruction ("PXOR",
			new BinarySyntax        (100, Opcode (0x0F, 0xEF),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MMXRegister::IdFunctor(), new MemMMXReg()),
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xEF), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("RCL",
			new UnarySyntax         (120, Opcode (0xD0, 0x02),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x02),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("RCR",
			new UnarySyntax         (120, Opcode (0xD0, 0x03),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x03),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("ROL",
			new UnarySyntax         (120, Opcode (0xD0, 0x00),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("ROR",
			new UnarySyntax         (120, Opcode (0xD0, 0x01),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x01),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x01),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("RCPPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x53),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("RCPSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x53), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("RDMSR",
			new ZerarySyntax        (100, Opcode (0x0F, 0x32),       Syntax::NOTHING)),

		new Instruction ("RDPMC",
			new ZerarySyntax        (100, Opcode (0x0F, 0x33),       Syntax::NOTHING)),

		new Instruction ("RDTSC",
			new ZerarySyntax        (100, Opcode (0x0F, 0x31),       Syntax::NOTHING)),

		new Instruction ("REP",
			new ZerarySyntax        (100, Opcode (0xF3),             Syntax::NOTHING)),

		new Instruction ("REPE",
			new ZerarySyntax        (100, Opcode (0xF3),             Syntax::NOTHING)),

		new Instruction ("REPZ",
			new ZerarySyntax        (100, Opcode (0xF3),             Syntax::NOTHING)),

		new Instruction ("REPNE",
			new ZerarySyntax        (100, Opcode (0xF2),             Syntax::NOTHING)),

		new Instruction ("REPNZ",
			new ZerarySyntax        (100, Opcode (0xF2),             Syntax::NOTHING)),

		new Instruction ("RSM",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAA),       Syntax::NOTHING)),

		new Instruction ("RSQRTPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x52),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("RSQRTSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x52), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("RET",
			new ZerarySyntax        (100, Opcode (0xC3),             Syntax::NOTHING),
			new UnarySyntax         (110, Opcode (0xC2),             Syntax::NOTHING,                                                                 new Immed<16, Number::UNSIGNED>())),

		new Instruction ("RETN",
			new ZerarySyntax        (100, Opcode (0xC3),             Syntax::NOTHING),
			new UnarySyntax         (110, Opcode (0xC2),             Syntax::NOTHING,                                                                 new Immed<16, Number::UNSIGNED>())),

		new Instruction ("RETF",
			new ZerarySyntax        (100, Opcode (0xCB),             Syntax::NOTHING),
			new UnarySyntax         (110, Opcode (0xCA),             Syntax::NOTHING,                                                                 new Immed<16, Number::UNSIGNED>())),

		new Instruction ("SAHF",
			new ZerarySyntax        (100, Opcode (0x9E),             Syntax::NOTHING)),

		new Instruction ("SAL",
			new UnarySyntax         (120, Opcode (0xD0, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SAR",
			new UnarySyntax         (120, Opcode (0xD0, 0x07),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x07),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x07),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new OptimizedBinaryInstruction ("SBB", Opcode (0x1C), Opcode(0x80, 0x03), Opcode(0x83, 0x03), Opcode (0x18)),

		new Instruction ("SCAS",
			new StringSyntax        (100, Opcode (0xAE),                                                                                              new Mem<8>(),  false),
			new StringSyntax        (110, Opcode (0xAF),                                                                                              new Mem<16>(), false),
			new StringSyntax        (120, Opcode (0xAF),                                                                                              new Mem<32>(), false)),

		new Instruction ("SCASB",
			new ZerarySyntax        (100, Opcode (0xAE),             Syntax::NOTHING)),

		new Instruction ("SCASW",
			new ZerarySyntax        (100, Opcode (0xAF),             Syntax::MODE_16BITS)),

		new Instruction ("SCASD",
			new ZerarySyntax        (100, Opcode (0xAF),             Syntax::MODE_32BITS)),

		new Instruction ("SETA",
			new UnarySyntax         (120, Opcode (0x0F, 0x97, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETAE",
			new UnarySyntax         (120, Opcode (0x0F, 0x93, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETB",
			new UnarySyntax         (120, Opcode (0x0F, 0x92, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETBE",
			new UnarySyntax         (120, Opcode (0x0F, 0x96, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETC",
			new UnarySyntax         (120, Opcode (0x0F, 0x92, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETE",
			new UnarySyntax         (120, Opcode (0x0F, 0x94, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETG",
			new UnarySyntax         (120, Opcode (0x0F, 0x9F, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETGE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9D, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETL",
			new UnarySyntax         (120, Opcode (0x0F, 0x9C, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETLE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9E, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNA",
			new UnarySyntax         (120, Opcode (0x0F, 0x96, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNAE",
			new UnarySyntax         (120, Opcode (0x0F, 0x92, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNB",
			new UnarySyntax         (120, Opcode (0x0F, 0x93, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNBE",
			new UnarySyntax         (120, Opcode (0x0F, 0x97, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNC",
			new UnarySyntax         (120, Opcode (0x0F, 0x93, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNE",
			new UnarySyntax         (120, Opcode (0x0F, 0x95, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNG",
			new UnarySyntax         (120, Opcode (0x0F, 0x9E, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNGE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9C, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNL",
			new UnarySyntax         (120, Opcode (0x0F, 0x9D, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNLE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9F, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNO",
			new UnarySyntax         (120, Opcode (0x0F, 0x91, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNP",
			new UnarySyntax         (120, Opcode (0x0F, 0x9B, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNS",
			new UnarySyntax         (120, Opcode (0x0F, 0x99, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETNZ",
			new UnarySyntax         (120, Opcode (0x0F, 0x95, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETO",
			new UnarySyntax         (120, Opcode (0x0F, 0x90, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETP",
			new UnarySyntax         (120, Opcode (0x0F, 0x9A, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETPE",
			new UnarySyntax         (120, Opcode (0x0F, 0x9A, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETPO",
			new UnarySyntax         (120, Opcode (0x0F, 0x9B, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETS",
			new UnarySyntax         (120, Opcode (0x0F, 0x98, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SETZ",
			new UnarySyntax         (120, Opcode (0x0F, 0x94, 0x00), Syntax::NOTHING,                                                                 new Mem8Reg8())),

		new Instruction ("SFENCE",
			new ZerarySyntax        (100, Opcode (0x0F, 0xAE, 0xF8), Syntax::NOTHING)),

		new Instruction ("SGDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x00), Syntax::FIRST_ARGUMENT,                                                          new Mem<48>())),

		new Instruction ("SHL",
			new UnarySyntax         (120, Opcode (0xD0, 0x04),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x04),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SHLD",
			new BinarySyntax        (100, Opcode (0x0F, 0xA4),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new CL()),
			new BinarySyntax        (110, Opcode (0x0F, 0xA5),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SHR",
			new UnarySyntax         (120, Opcode (0xD0, 0x05),       Syntax::FIRST_ARGUMENT,                                                          new MemGPReg(),               new ImmedEqual<1>(), 1),
			new BinarySyntax        (110, Opcode (0xD2, 0x05),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new CL()),
			new BinarySyntax        (100, Opcode (0xC0, 0x05),       Syntax::FIRST_ARGUMENT, false, Argument::NONE,         1, BinarySyntax::PARTIAL, new MemGPReg(),               new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SHRD",
			new BinarySyntax        (100, Opcode (0x0F, 0xAC),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new CL()),
			new BinarySyntax        (110, Opcode (0x0F, 0xAD),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new MemWordReg(),             new WordReg(),                     new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SHUFPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0xC6), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SHUFPS",
			new BinarySyntax        (100, Opcode (0x0F, 0xC6),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>(), new Immed<8, Number::UNSIGNED>())),

		new Instruction ("SIDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x01, 0x01), Syntax::FIRST_ARGUMENT,                                                          new Mem<48>())),

		new Instruction ("SLDT",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x00), Syntax::FIRST_ARGUMENT,                                                          new MemWordReg())),

		new Instruction ("SQRTPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x51), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("SQRTPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x51),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("SQRTSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x51), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("SQRTSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x51), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("STC",
			new ZerarySyntax        (100, Opcode (0xF9),             Syntax::NOTHING)),

		new Instruction ("STD",
			new ZerarySyntax        (100, Opcode (0xFD),             Syntax::NOTHING)),

		new Instruction ("STI",
			new ZerarySyntax        (100, Opcode (0xFB),             Syntax::NOTHING)),

		new Instruction ("STMXCSR",
			new UnarySyntax         (100, Opcode (0x0F, 0xAE, 0x03), Syntax::NOTHING,                                                                 new GPRegister32Bits::IdFunctor())),

		new Instruction ("STOS",
			new StringSyntax        (100, Opcode (0xAA),                                                                                              new Mem<8>(),  false),
			new StringSyntax        (110, Opcode (0xAB),                                                                                              new Mem<16>(), false),
			new StringSyntax        (120, Opcode (0xAB),                                                                                              new Mem<32>(), false)),

		new Instruction ("STOSB",
			new ZerarySyntax        (100, Opcode (0xAA),             Syntax::NOTHING)),

		new Instruction ("STOSW",
			new ZerarySyntax        (100, Opcode (0xAB),             Syntax::MODE_16BITS)),

		new Instruction ("STOSD",
			new ZerarySyntax        (100, Opcode (0xAB),             Syntax::MODE_32BITS)),

		new Instruction ("STR",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x01), Syntax::FIRST_ARGUMENT,                                                          new MemWordReg())),

		new OptimizedBinaryInstruction ("SUB", Opcode (0x2C), Opcode(0x80, 0x05), Opcode(0x83, 0x05), Opcode (0x28)),

		new Instruction ("SUBPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x5C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("SUBPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x5C),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("SUBSD",
			new BinarySyntax        (100, Opcode (0xF2, 0x0F, 0x5C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("SUBSS",
			new BinarySyntax        (100, Opcode (0xF3, 0x0F, 0x5C), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("SYSENTER",
			new ZerarySyntax        (100, Opcode (0x0F, 0x34),       Syntax::NOTHING)),

		new Instruction ("SYSEXIT",
			new ZerarySyntax        (100, Opcode (0x0F, 0x35),       Syntax::NOTHING)),

		new Instruction ("TEST",
			new BinarySyntax        (100, Opcode (0xF6, 0x00),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,	       1, BinarySyntax::PARTIAL, new MemGPReg(),    new Immediate::IdFunctor()),
			new BinarySyntax        (110, Opcode (0x84),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemGPReg(),    new GPRegister::IdFunctor()),
			new BinarySyntax        (120, Opcode (0xA8),             Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::ABSENT,  new Accumulator(), new Immediate::IdFunctor())),

		new Instruction ("UCOMISD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x2E), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<64, Memory::FLOAT>())),

		new Instruction ("UCOMISS",
			new BinarySyntax        (100, Opcode (0x0F, 0x2E),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<32, Memory::FLOAT>())),

		new Instruction ("UD2",
			new ZerarySyntax        (100, Opcode (0x0F, 0x0B),       Syntax::NOTHING)),

		new Instruction ("UNPCKHPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x15), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("UNPCKHPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x15),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("UNPCKLPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x14), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("UNPCKLPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x14),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("VERR",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x04), Syntax::NOTHING,                                                                 new MemWordReg())),

		new Instruction ("VERW",
			new UnarySyntax         (100, Opcode (0x0F, 0x00, 0x05), Syntax::NOTHING,                                                                 new MemWordReg())),

		new Instruction ("WAIT",
			new ZerarySyntax        (100, Opcode (0x9),              Syntax::NOTHING)),

		new Instruction ("WBINVD",
			new ZerarySyntax        (100, Opcode (0x0F, 0x09),       Syntax::NOTHING)),

		new Instruction ("WRMSR",
			new ZerarySyntax        (100, Opcode (0x0F, 0x30),       Syntax::NOTHING)),

		new Instruction ("XADD",
			new BinarySyntax        (100, Opcode (0x0F, 0xC0),       Syntax::FIRST_ARGUMENT, false, Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemWordReg(), new WordReg())),

		new Instruction ("XCHG",
			new BinarySyntax        (100, Opcode (0x86),             Syntax::FIRST_ARGUMENT, true,  Argument::EQUAL,        1, BinarySyntax::PRESENT, new MemGPReg(),   new GPRegister::IdFunctor()),
			new AdditiveUnarySyntax (100, Opcode (0x90),             Syntax::FIRST_ARGUMENT,                                                          new WordReg(),    new Accumulator())),

		new Instruction ("XLAT",
			new StringSyntax        (100, Opcode (0xD7),                                                                                              new Mem<8>(),  true)),

		new Instruction ("XLATB",
			new ZerarySyntax        (100, Opcode (0xD7),             Syntax::NOTHING)),

		new OptimizedBinaryInstruction ("XOR",  Opcode (0x34), Opcode(0x80, 0x06), Opcode(0x83, 0x06), Opcode (0x30)),

		new Instruction ("XORPD",
			new BinarySyntax        (100, Opcode (0x66, 0x0F, 0x57), Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),

		new Instruction ("XORPS",
			new BinarySyntax        (100, Opcode (0x0F, 0x57),       Syntax::NOTHING,        false, Argument::EQUAL,        0, BinarySyntax::PRESENT, new XMMRegister::IdFunctor(), new MemXMMReg<128, Memory::FLOAT>())),
	};

	unsigned int n = sizeof (Instructions) / sizeof (Instruction *);
	for (unsigned int i = 0; i < n; i++)
	{
		try
		{
			InstructionTable.Insert (Instructions[i]);
		}
		catch (exception &e)
		{
			cout << e.what() << ": " << Instructions[i]->GetName() << endl;
		}
	}
}

Instruction::~Instruction () throw ()
{
	for (ContainerType::const_iterator i = Syntaxes.begin(); i != Syntaxes.end(); i++)
		delete *i;
}

InvalidSyntax::InvalidSyntax (const Command *cmd, const vector<Argument *> args) throw ()
	: BadCommand(cmd), BadArguments(args), WhatString("Invalid combination of arguments for this command.")
{
}

const char *InvalidSyntax::what() const throw ()
{
	return WhatString.c_str();
}
